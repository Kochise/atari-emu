-- VHDL Model of an RS-flipflop
--  CLA LIBRARY:xxx
--  Written 30/5/95 by Craig Graham 
------------------------------------------- 
ENTITY sr_bistable IS
   PORT(P:in bit; C:in bit; R:out bit);
END sr_bistable;

ARCHITECTURE dataflow OF sr_bistable IS
SIGNAL s1:bit;
SIGNAL s2:bit;
BEGIN
   R<=S1;
   s2<=NOT(C AND S1);
   s1<=NOT(P AND S2);
END;
