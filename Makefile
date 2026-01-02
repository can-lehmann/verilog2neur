EXAMPLES := $(patsubst %.v,%.neur,$(wildcard examples/*.v))

all: bin/neur bin/verilog2neur $(EXAMPLES)

bin/verilog2neur: src/main.cpp
	clang++ -g -o $@ $<

bin/neur: neur/neur.c
	clang -O3 -o $@ $<

examples/%.neur: examples/%.textir bin/verilog2neur
	bin/verilog2neur $< $@

examples/%.textir: examples/%.v
	yosys -m hdl -p "read_verilog $<; hierarchy; proc; flatten; opt_expr; opt_clean; write_hdl -top top $@"
