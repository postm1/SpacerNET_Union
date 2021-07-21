// Supported with union (c) 2020 Union team

// User API for zVEC3
// Add your methods here


zVEC3& Minus()
{
	n[0] = -n[0];
	n[1] = -n[1];
	n[2] = -n[2];
	return *this;
}

zSTRING ToString()
{
	return "(" + zSTRING(n[0], 10) + ",  " + zSTRING(n[1], 10) + ",  " + zSTRING(n[2], 10) + ")";
}