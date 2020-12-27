-- VHDL Model of a Toggle
--  CLA LIBRARY:USERLIB
--  Written 29/5/95 by Craig Graham
-------------------------------------------
ENTITY toggle IS
   PORT(tog:in bit; op1:out bit; op2:out bit );
END toggle;

ARCHITECTURE dataflow OF toggle IS
-- c_state is an internal register which holds the current state of the toggle
SIGNAL c_state:bit;
BEGIN

-- Every positive pulse on tog will trigger this process
   PROCESS(tog)
   BEGIN
--  Invert the current state. 'c_state' is a register because it is assigned inside the PROCESS
      c_state<=NOT(c_state);
   END PROCESS;

-- Derive the outputs from the current state of the toggle
   op1<=c_state;
   op2<=NOT(c_state);
END;
