// Supported with union (c) 2020 Union team

// User API for zTPlane
// Add your methods here

float		GetDistanceToPlane(const zVEC3& p) const { return (normal.Dot(p) - distance); };