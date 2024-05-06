// Supported with union (c) 2020 Union team

// User API for zCQuat
// Add your methods here

zSTRING ToString()
{
	return "(" + zSTRING(q[0], 10) + ",  " + zSTRING(q[1], 10) + ",  " + zSTRING(q[2], 10) + ",  " + zSTRING(q[3], 10) + ")";
}