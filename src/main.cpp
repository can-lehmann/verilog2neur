// Copyright (c) 2026 Can Joshua Lehmann

#include <iostream>
#include <fstream>
#include <cassert>

#include "../modules/hdl.cpp/hdl_textir.hpp"
#include "../modules/hdl.cpp/hdl_flatten.hpp"

using namespace hdl;

class Net {
public:
  using Id = size_t;

  struct Neuron {
    std::vector<Id> excite;
    std::vector<Id> inhibit;
    int threshold = 0;
    std::string name;

    Neuron() {}
    Neuron(int _threshold, std::string _name):
      threshold(_threshold), name(_name) {}
  };
private:
  std::vector<Neuron> _neurons;
  std::vector<Id> _init;
public:
  Net() {}

  Id add(Id threshold, const std::string& name = "") {
    Id id = _neurons.size();
    _neurons.emplace_back(threshold, name);
    return id;
  }

  void excite(Id from, Id to) {
    _neurons[to].excite.push_back(from);
  }

  void inhibit(Id from, Id to) {
    _neurons[to].inhibit.push_back(from);
  }

  void init(Id id) {
    _init.push_back(id);
  }

private:
  std::string neuron_name(Id id) const {
    std::ostringstream name;
    if (_neurons[id].name.empty()) {
      name << 'n' << id;
    } else {
      name << _neurons[id].name;
    }
    name << '/' << _neurons[id].threshold;
    name << '*';
    return name.str();
  }
public:
  void write(std::ostream& stream) const {
    for (Id id = 0; id < _neurons.size(); id++) {
      for (Id from : _neurons[id].excite) {
        stream << neuron_name(from) << " : " << neuron_name(id) << " .\n";
      }
      for (Id from : _neurons[id].inhibit) {
        stream << neuron_name(from) << " ; " << neuron_name(id) << " .\n";
      }
    }

    for (Id id : _init) {
      stream << neuron_name(id) << " .\n";
    }
  }

  void save(const char* path) const {
    std::ofstream stream;
    stream.open(path);
    if (!stream) {
      throw std::runtime_error("Unable to open file for writing.");
    }
    write(stream);
  }

  void write_dot(std::ostream& stream) const {
    stream << "digraph {\n";
    for (Id id = 0; id < _neurons.size(); id++) {
      stream << "n" << id << " [label=\"" << neuron_name(id) << "\"];\n";
    }
    for (Id id = 0; id < _neurons.size(); id++) {
      for (Id from : _neurons[id].excite) {
        stream << "n" << from << " -> " << "n" << id << " [color=black];\n";
      }
      for (Id from : _neurons[id].inhibit) {
        stream << "n" << from << " -> " << "n" << id << " [color=red];\n";
      }
    }
    stream << "}\n";
  }

  void save_dot(const char* path) const {
    std::ofstream stream;
    stream.open(path);
    if (!stream) {
      throw std::runtime_error("Unable to open file for writing.");
    }
    write_dot(stream);
  }
};

class SynthNet {
private:
  Net& _net;
  std::unordered_map<Value*, Net::Id> _values;

  Net::Id _always = 0;
  Net::Id _never = 0;
  Net::Id _clock = 0;

  Net::Id build_and(Net::Id a, Net::Id b) {
    Net::Id id = _net.add(2);
    _net.excite(a, id);
    _net.excite(b, id);
    return id;
  }

  Net::Id build_or(Net::Id a, Net::Id b) {
    Net::Id id = _net.add(1);
    _net.excite(a, id);
    _net.excite(b, id);
    return id;
  }

  Net::Id build_not(Net::Id a) {
    Net::Id id = _net.add(1);
    _net.excite(_always, id);
    _net.inhibit(a, id);
    return id;
  }

  Net::Id build_xor(Net::Id a, Net::Id b) {
    return build_and(
      build_or(a, b),
      build_not(build_and(a, b))
    );
  }
public:
  SynthNet(Net& net): _net(net) {
    _always = _net.add(1, "_always");
    _net.excite(_always, _always);
    _net.init(_always);

    _never = _net.add(1, "_never");

    _clock = _net.add(1, "_clock");
    _net.init(_clock);
  }

  Net::Id build(Value* value) {
    if (_values.find(value) != _values.end()) {
      return _values.at(value);
    }

    Net::Id id = 0;
    if (Constant* constant = dynamic_cast<Constant*>(value)) {
      if (constant->value.as_bool()) {
        id = _always;
      } else {
        id = _never;
      }
    } else if (Input* input = dynamic_cast<Input*>(value)) {
      id = _net.add(1, input->name);
    } else if (Op* op = dynamic_cast<Op*>(value)) {
      switch (op->kind) {
        case Op::Kind::And: id = build_and(build(op->args[0]), build(op->args[1])); break;
        case Op::Kind::Or: id = build_or(build(op->args[0]), build(op->args[1])); break;
        case Op::Kind::Not: id = build_not(build(op->args[0])); break;
        case Op::Kind::Xor: id = build_xor(build(op->args[0]), build(op->args[1])); break;
        default: assert(false && "Unsupported Op");
      }
    } else {
      assert(false && "Unsupported Value");
    }

    _values[value] = id;
    return id;
  }

  void build_clock(Net::Id clock, size_t period) {
    _net.init(clock);
    Net::Id prev = clock;
    for (size_t it = 0; it < period; it++) {
      Net::Id next = _net.add(1);
      _net.excite(prev, next);
      prev = next;
    }
    _net.excite(prev, clock);
  }

  void run(Module& module) {
    for (Reg* reg : module.regs()) {
      _values[reg] = _net.add(1, reg->name);
      if (reg->initial.as_bool()) {
        _net.init(_values[reg]);
      }
    }

    for (Reg* reg : module.regs()) {
      Net::Id clock = _clock; // We ignore the reg->clock for now
      Net::Id next = build(reg->next);
      Net::Id set = _net.add(2);
      Net::Id reset = _net.add(1);

      _net.excite(clock, set);
      _net.excite(next, set);

      _net.excite(clock, reset);
      _net.inhibit(next, reset);

      Net::Id q = _values[reg];

      _net.excite(set, q);
      _net.inhibit(reset, q);
      _net.excite(q, q);
    }

    for (Output output : module.outputs()) {
      Net::Id id = build(output.value);
      _net.excite(id, _net.add(1, output.name));
    }

    build_clock(_clock, 10);
  }
};

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cerr << "Usage:\n\t" << argv[0] << " [compile/show] <input_file.textir> <output_file.neur>\n";
    return 1;
  }

  enum class Mode {
    Compile,
    Show
  };

  Mode mode;
  if (std::string(argv[1]) == "compile") {
    mode = Mode::Compile;
  } else if (std::string(argv[1]) == "show") {
    mode = Mode::Show;
  } else {
    std::cerr << "Invalid mode \"" << argv[1] << "\". Expected \"compile\" or \"show\".\n";
    return 1;
  }

  Module module = textir::Reader::load_module(argv[2]);

  Module flattened_module("flattened");
  flatten::Flattening flattening(flattened_module);

  for (Input* input : module.inputs()) {
    std::vector<Value*> bits;
    for (size_t it = 0; it < input->width; it++) {
      std::string name = input->name + "_" + std::to_string(it);
      bits.push_back(flattened_module.input(name, 1));
    }
    flattening.define(input, bits);
  }

  for (Reg* reg : module.regs()) {
    std::vector<Value*> bits;
    for (size_t it = 0; it < reg->width; it++) {
      Reg* bit = flattened_module.reg(BitString::from_bool(reg->initial.at(it)), nullptr);
      bit->name = reg->name + "_" + std::to_string(it);
      bits.push_back(bit);
    }
    flattening.define(reg, bits);
  }

  for (Reg* reg : module.regs()) {
    flattening.flatten(reg->next);
    flattening.flatten(reg->clock);
    
    for (size_t it = 0; it < reg->width; it++) {
      Reg* bit_reg = (Reg*) flattening[reg][it];
      bit_reg->next = flattening[reg->next][it];
      bit_reg->clock = flattening[reg->clock][0];
    }
  }

  for (Output output : module.outputs()) {
    flattening.flatten(output.value);
    std::vector<Value*> bits = flattening[output.value];
    assert(bits.size() == output.value->width);
    for (size_t it = 0; it < output.value->width; it++) {
      std::string name = output.name + "_" + std::to_string(it);
      flattened_module.output(name, bits[it]);
    }
  }

  Net net;
  SynthNet synth(net);
  synth.run(flattened_module);

  if (mode == Mode::Show) {
    net.save_dot(argv[3]);
  } else {
    net.save(argv[3]);
  }
  
  return 0;
}