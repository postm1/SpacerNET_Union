// Supported with union (c) 2020 Union team

// User API for zMAT4
// Add your methods here


void GetTranslation(zVEC3& t) const {
	t.n[VX] = v[0][3];
	t.n[VY] = v[1][3];
	t.n[VZ] = v[2][3];
};