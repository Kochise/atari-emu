-- VHDL Model of an LS74112 
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
   -- ls74112 IS falling edge triggered so invert clock 
   cp0<=NOT cp0bar; 
 
   cp1<=NOT cp1bar; 
 
   PROCESS (cp0) 
   BEGIN 
      iq0<=cp0bar and gnd and k0 or (rd0bar and k1 and j1) or (gnd and cp0); 
   END PROCESS ; 
 
   q0bar<=iqb0; 
END;
