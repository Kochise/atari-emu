-- VHDL Model of a d-flipflip (rising edge triggered)
--  CLA LIBRARY:xxx 
--  Written 27/4/94 by Craig Graham 
-----------------------------------------------------
ENTITY dflip IS 
   PORT(d:in bit; clock:in bit; q:out bit; qbar:out bit; 
   ); 
END dflip; 
 
ARCHITECTURE dataflow OF dflip IS 
SIGNAL c_state:bit; 
BEGIN 
 
-- update the current internal state on the rising edge of clock
   PROCESS (clock) 
   BEGIN 
      c_state<=d; 
   END PROCESS ; 

-- derive outputs from internal state
   q<= d; 
   qbar<=NOT d;
 
END;
