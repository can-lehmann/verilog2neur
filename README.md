# Synthesizing McCulloch-Pitts Neural Networks from Verilog

McCulloch-Pitts neurons are a simple model of biological neurons.
It is possible to build a functionally complete set of logic gates using McCulloch-Pitts neurons.
This means that any digital circuit can be represented using McCulloch-Pitts neural networks.
verilog2neur is a tool that synthesizes McCulloch-Pitts neural networks from Verilog descriptions of digital circuits.
It supports combinational as well as sequential circuits.

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

## A Counter

Now let's look at a slightly more complex example: a 4-bit counter that increments its value on each clock cycle.

```verilog
module top(input clock, output [3:0] count);
  reg [3:0] counter = 0;
  
  always @(posedge clock)
    counter <= counter + 1;
  
  assign count = counter;
endmodule
```

Simulating the synthesized neural network for this module produces the following output over several clock cycles:

```
00 _always/1 _clock/1 
01 _always/1 n7/1 n12/1 n19/1 n26/1 n9/1 n15/1 n22/1 n29/1 n34/1 
02 _always/1 n7/1 n12/1 n19/1 n26/1 n35/1 
03 _always/1 n7/1 n12/1 n19/1 n26/1 n36/1 
04 _always/1 n7/1 n12/1 n19/1 n26/1 n37/1 
05 _always/1 n7/1 n12/1 n19/1 n26/1 n38/1 
06 _always/1 n7/1 n12/1 n19/1 n26/1 n39/1 
07 _always/1 n7/1 n12/1 n19/1 n26/1 n40/1 
08 _always/1 n7/1 n12/1 n19/1 n26/1 n41/1 
09 _always/1 n7/1 n12/1 n19/1 n26/1 n42/1 
10 _always/1 n7/1 n12/1 n19/1 n26/1 n43/1 
11 _always/1 n7/1 n12/1 n19/1 n26/1 _clock/1 
12 _always/1 n7/1 n12/1 n19/1 n26/1 n8/2 n15/1 n22/1 n29/1 n34/1 
13 _always/1 n7/1 n12/1 n19/1 n26/1 counter_0/1 n35/1 
14 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n36/1 
15 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n37/1 
16 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n38/1 
17 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n39/1 
18 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n40/1 
19 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n41/1 
20 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n42/1 
21 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n43/1 
22 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 _clock/1 
23 _always/1 n12/1 n19/1 n26/1 counter_0/1 n10/1 count_0/1 n13/2 n9/1 n14/2 n22/1 n29/1 n34/1 
24 _always/1 n12/1 n19/1 n26/1 n10/1 count_0/1 n13/2 counter_1/1 n35/1 
25 _always/1 n7/1 n12/1 n19/1 n26/1 n13/2 counter_1/1 n10/1 count_1/1 n36/1 
26 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n37/1 
27 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n38/1 
28 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n39/1 
29 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n40/1 
30 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n41/1 
31 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n42/1 
32 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n43/1 
33 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 _clock/1 
34 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 n8/2 n14/2 n22/1 n29/1 n34/1 
35 _always/1 n7/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 counter_0/1 n35/1 
36 _always/1 n12/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 counter_0/1 n11/2 n16/2 count_0/1 n36/1 
37 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 n13/2 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n37/1 
38 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n38/1 
39 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n39/1 
40 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n40/1 
41 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n41/1 
42 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n42/1 
43 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n43/1 
44 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 _clock/1 
45 _always/1 n19/1 n26/1 counter_1/1 n10/1 count_1/1 counter_0/1 n11/2 n16/2 count_0/1 n17/1 n20/2 n9/1 n15/1 n21/2 n29/1 n34/1 
46 _always/1 n19/1 n26/1 n10/1 count_1/1 n11/2 n16/2 count_0/1 n17/1 n20/2 counter_2/1 n35/1 
47 _always/1 n7/1 n19/1 n26/1 n17/1 n20/2 counter_2/1 n18/2 n23/2 count_2/1 n36/1 
48 _always/1 n7/1 n12/1 n26/1 n20/2 counter_2/1 n17/1 count_2/1 n24/1 n37/1 
49 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n27/2 n38/1 
50 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 n39/1 
51 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 n40/1 
52 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 n41/1 
53 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 n42/1 
54 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 n43/1 
55 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 _clock/1 
56 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 n8/2 n15/1 n21/2 n29/1 n34/1 
57 _always/1 n7/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n35/1 
58 _always/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n10/1 count_0/1 n36/1 
59 _always/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n10/1 count_0/1 n13/2 n37/1 
60 _always/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n10/1 count_0/1 n13/2 n38/1 
61 _always/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n10/1 count_0/1 n13/2 n39/1 
62 _always/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n10/1 count_0/1 n13/2 n40/1 
63 _always/1 n12/1 n19/1 n26/1 counter_2/1 n17/1 count_2/1 n20/2 counter_0/1 n10/1 count_0/1 n13/2 n41/1 
```

We can see that it starts off at 0 in step 0, then activates the counter_0 neuron in step 13 (indicating a count of 1), then switches to counter_1 in step 24 (indicating a count of 2), and continues incrementing the count to 3, 4 and 5 in subsequent steps.
