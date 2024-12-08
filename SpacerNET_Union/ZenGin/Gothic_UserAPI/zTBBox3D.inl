// Supported with union (c) 2020 Union team

// User API for zTBBox3D
// Add your methods here

void Translate(const zVEC3& trans) { mins += trans; maxs += trans; };

float GetMaxExtent();

#if ENGINE == Engine_G1
//FIXME_G1
//zBOOL IsIntersecting(const zPOINT3& rayOrigin, const zVEC3& rayDirection, zREAL& scaleMin, zREAL& scaleMax);

#endif

#if ENGINE < Engine_G2A
zBOOL IsIntersecting(const zVEC3& rayOrigin, const zVEC3& rayDirection, zREAL& scaleMin, zREAL& scaleMax) const;
#endif