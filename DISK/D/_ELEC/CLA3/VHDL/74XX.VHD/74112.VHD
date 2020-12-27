-- VHDL Model of an LS74112 
--  Dual JK flip-flop
--  CLA LIBRARY:74xx 
--  Written 27/4/94 by Craig Graham 
------------------------------------------- 
ENTITY ls74112 IS 
   PORT(cp0bar:in bit; k0:in bit; j0:in bit; sd0bar:in bit; 
       q0:OUT bit; q0bar:OUT bit; 
       q1bar:OUT bit; 
       gnd:in bit; vcc:in bit; 
       rd0bar:in bit; rd1bar:in bit; cp1bar:in bit; 
       k1:in bit; j1:in bit; sd1bar:in bit; q1:out bit; 
      ); 
END 74112; 
 
ARCHITECTURE dataflow OF ls74112 IS 
SIGNAL cp0:bit, cp1:bit, iq0:bit, iq1:bit, iqb0:bit, iqb1:bit; 
BEGIN 
   -- ls74112 is falling edge triggered so invert clock 
   cp0<=NOT cp0bar; 
 
   cp1<=NOT cp1bar; 
 
   PROCESS (cp0) 
   BEGIN 
      iq0<=sd0bar NAND iqb0 NAND (j0 NAND iqb0); 
      iqb0<=rd0bar NAND iq0 NAND (k0 NAND iq0); 
   END PROCESS ; 
 
   -- Notice that the two JK's are in different process' to allow independent 
   --  operation. 
 
   PROCESS (cp1) 
   BEGIN 
      iq1<=sd1bar NAND iqb1 NAND (j1 NAND iqb1); 
      iqb1<=rd1bar NAND iq1 NAND (k1 NAND iq1); 
   END PROCESS; 
 
   q0<=iq0; 
   q1<=iq1; 
   q0bar<=iqb0; 
   q1bar<=iqb1; 
 
END;
