// Supported with union (c) 2020 Union team

// User API for zTBBox3D
// Add your methods here

void Translate(const zVEC3& trans) { mins += trans; maxs += trans; };

float GetMaxExtent();