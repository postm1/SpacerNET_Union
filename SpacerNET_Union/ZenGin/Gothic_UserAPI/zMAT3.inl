// Supported with union (c) 2020 Union team

// User API for zMAT3
// Add your methods here

zVEC3	GetUpVector3()			const { return zVEC3(v[0][1], v[1][1], v[2][1]); };
zVEC3	GetRightVector3()			const { return zVEC3(v[0][0], v[1][0], v[2][0]); };
zVEC3	GetAtVector3()			const { return zVEC3(v[0][2], v[1][2], v[2][2]); };