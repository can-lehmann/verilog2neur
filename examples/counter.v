module top(input clock, output [3:0] count);
  reg [3:0] counter = 0;
  
  always @(posedge clock)
    counter <= counter + 1;
  
  assign count = counter;
endmodule
