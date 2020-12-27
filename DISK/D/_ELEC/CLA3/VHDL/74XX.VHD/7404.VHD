-- VHDL Model of an LS7404
--  CLA LIBRARY:74xx
--  Written 4/9/93 by Craig Graham
-------------------------------------------
ENTITY LS7404 IS
	PORT(a1:in bit; o1:out bit;
		 a2:in bit; o2:out bit;
		 a3:in bit; o3:out bit;
		 gnd:in bit;
		 vcc:in bit;
		 a4:in bit; o4:out bit;
		 a5:in bit; o5:out bit;
		 a6:in bit; o6:out bit;
		);
END LS7400;

ARCHITECTURE dataflow OF ls7400 is
begin
	o1<=not a1;
	o2<=not a2;
	o3<=not a3;
	o4<=not a4;
	o5<=not a5;
	o6<=not a6;
end;
