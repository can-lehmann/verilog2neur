EXAMPLES := $(wildcard examples/*.v)
TARGETS := \
	$(patsubst %.v,%.neur,$(EXAMPLES)) \
	$(patsubst %.v,%.svg,$(EXAMPLES))

all: bin/neur bin/verilog2neur $(TARGETS)

bin/verilog2neur: src/main.cpp
	clang++ -g -o $@ $<

bin/neur: neur/neur.c
	clang -O3 -o $@ $<

examples/%.neur: examples/%.textir bin/verilog2neur
	bin/verilog2neur compile $< $@

examples/%.dot: examples/%.textir bin/verilog2neur
	bin/verilog2neur show $< $@

examples/%.textir: examples/%.v
	yosys -m hdl -p "read_verilog $<; hierarchy; proc; flatten; opt_expr; opt_clean; write_hdl -top top $@"

examples/%.svg: examples/%.dot
	dot -Tsvg $< -o $@
