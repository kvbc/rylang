// assuming :std has $str
:std = "std";

$person = {
	name :std:$str;
	age usize;	
};

$anon = {
	age usize;
};

#rgb = {
	RED;
	GREEN;
	BLUE;
};

#hsv = {
	HUE;
	SAT;
	VAL;
};

$^guy = $person;

^guyPtr = * $person;

List( comp T: type ) type = {
	return ${
		x T;
	};
};
list List(i32) = { .x = 3; };

comp guy type = $person;
p guy = { .age = 32; };

