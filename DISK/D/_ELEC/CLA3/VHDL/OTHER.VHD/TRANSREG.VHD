-- VHDL Model of a tranparent register (active high)
--  CLA LIBRARY:xxx 
--  Written 30/5/95 by Craig Graham 
-----------------------------------------------------
ENTITY t_reg IS 
   PORT(d:in bit; enable:in bit; q:out bit; qbar:out bit; 
   ); 
END t_reg; 
 
ARCHITECTURE dataflow OF t_reg IS 
SIGNAL s1:bit, s2:bit, s:bit, r:bit; 
BEGIN 
    s<=enable AND d;
    r<=enable AND NOT d;

-- s1 & s2 create an SR-bistable
    s1<=NOT(s2 AND NOT s);
    s2<=NOT(s1 AND NOT r);
 
-- derive outputs from internal state of the bistable
   q<= s1; 
   qbar<=NOT s1;
 
END;
