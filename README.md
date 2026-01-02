# Synthesizing McCulloch-Pitts Neural Networks from Verilog

McCulloch-Pitts neurons are a simple model of biological neurons.
It is possible to build a functionally complete set of logic gates using McCulloch-Pitts neurons.
This means that any digital circuit can be represented using McCulloch-Pitts neural networks.
verilog2neur is a tool that synthesizes McCulloch-Pitts neural networks from Verilog descriptions of digital circuits.
For now, it only supports combinatorial circuits.

## Adding 2 + 3 using Neurons

Consider the following Verilog module that implements a 4-bit adder:

```verilog
module top(input [3:0] a, input [3:0] b, output [3:0] c);
  assign c = a + b;
endmodule
```

Running verilog2neur on this module produces the following neural network description:

```
_always/1* : _always/1* .
a_0/1* : n4/1* .
b_0/1* : n4/1* .
a_0/1* : n5/2* .
b_0/1* : n5/2* .
_always/1* : n6/1* .
n5/2* ; n6/1* .

...

n50/1* : n51/2* .
n51/2* : c_3/1* .
_always/1* .
```

We can now add e.g. the numbers 2 (`0010`) and 3 (`0011`) by providing the corresponding input bits to the module.

```
_always/1* : a_1/1* .
_always/1* : b_0/1* .
_always/1* : b_1/1* .
```

Simulating this network in the `neur` simulator produces the following output:

```
00 _always/1 
01 _always/1 n6/1 n14/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 
02 _always/1 n6/1 n14/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 
03 _always/1 n6/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 n15/2 n7/2 n25/1 
04 _always/1 n6/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 n7/2 n25/1 n16/1 c_0/1 n32/1 
05 _always/1 n6/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 n7/2 n25/1 c_0/1 n32/1 n19/2 n35/2 
06 _always/1 n6/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 n7/2 n25/1 c_0/1 n32/1 n35/2 c_1/1 c_2/1 
07 _always/1 n6/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 n7/2 n25/1 c_0/1 n32/1 n35/2 c_2/1 
08 _always/1 n6/1 n18/1 n30/1 n34/1 n46/1 n50/1 a_1/1 b_0/1 b_1/1 n12/1 n4/1 n13/2 n24/2 n7/2 n25/1 c_0/1 n32/1 n35/2 c_2/1 
```

As you can see, it stabilizes with the output bits `c_0` and `c_2` activated, which corresponds to the binary representation of 5 (`0101`).

