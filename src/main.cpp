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

private:
  std::string neuron_name(Id id) const {
    std::ostringstream name;
    if (_neurons[id].name.empty()) {
      name << 'n' << id;
    } else {
      name << _neurons[id].name;
    }
    name << '/' << _neurons[id].threshold;
    return name.str();
  }
public:
  void write(std::ostream& stream) const {
    for (Id id = 0; id < _neurons.size(); id++) {
      stream << " : " << neuron_name(id) << " .\n";
      for (Id from : _neurons[id].excite) {
        stream << neuron_name(from) << " : " << neuron_name(id) << " .\n";
      }
      for (Id from : _neurons[id].inhibit) {
        stream << neuron_name(from) << " ; " << neuron_name(id) << " .\n";
      }
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
};

class NetBuilder {
private:
  Net& _net;
  std::unordered_map<Value*, Net::Id> _values;

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
    Net::Id id = _net.add(0);
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
  NetBuilder(Net& net): _net(net) {}

  Net::Id build(Value* value) {
    if (_values.find(value) != _values.end()) {
      return _values.at(value);
    }

    Net::Id id = 0;
    if (Constant* constant = dynamic_cast<Constant*>(value)) {
      if (constant->value.as_bool()) {
        id = _net.add(0); // Always firing neuron
      } else {
        id = _net.add(1); // Never firing neuron
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

  void build(Module& module) {
    for (Output output : module.outputs()) {
      Net::Id id = build(output.value);
      _net.excite(id, _net.add(1, output.name));
    }
  }
};

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage:\n\t" << argv[0] << " <input_file.textir> <output_file.neur>\n";
    return 1;
  }

  Module module = textir::Reader::load_module(argv[1]);

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
  NetBuilder net_builder(net);
  net_builder.build(flattened_module);

  net.save(argv[2]);

  return 0;
}