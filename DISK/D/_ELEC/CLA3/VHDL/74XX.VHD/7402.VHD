-- VHDL Model of an LS7402
--  CLA LIBRARY:74xx
--  Written 4/9/93 by Craig Graham
-------------------------------------------
ENTITY LS7402 IS
	PORT(o1:out bit; a1:in bit; b1:in bit;
		 o2:out bit; a2:in bit; b2:in bit;
		 gnd:in bit;
		 vcc:in bit;
		 o3:out bit; a3:in bit; b3:in bit;
		 o4:out bit; a4:in bit; b4:in bit;
		);
END LS7400;

ARCHITECTURE dataflow OF ls7402 is
begin
	o1<=a1 nor b1;
	o2<=a2 nor b2;
	o3<=a3 nor b3;
	o4<=a4 nor b4;
end;
