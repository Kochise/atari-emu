-- VHDL Model of an LS7400 
--  CLA LIBRARY:74xx 
--  Written 4/9/93 by Craig Graham 
------------------------------------------- 
ENTITY ls7400 IS 
   PORT(a1:in bit; b1:in bit; o1:out bit; 
       a2:in bit; b2:in bit; o2:out bit; 
       gnd:in bit; vcc:in bit; 
       a3:in bit; b3:in bit; o3:out bit; 
       a4:in bit; b4:in bit; o4:out bit; 
 
      ); 
END ls7400; 
 
ARCHITECTURE dataflow OF ls7400 is 
begin 
   o1<=a1 nand b1; 
   o2<=a2 nand b2; 
   o3<=a3 nand b3; 
   o4<=a4 nand b4; 
end;
