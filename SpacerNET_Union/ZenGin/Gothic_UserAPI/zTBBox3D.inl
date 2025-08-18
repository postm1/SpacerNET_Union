// Supported with union (c) 2020 Union team

// User API for zTBBox3D
// Add your methods here

void Translate(const zVEC3& trans) { mins += trans; maxs += trans; };

float GetMaxExtent();

zVEC3 GetCenterTop()
{
    return zVEC3(
        (mins[0] + maxs[0]) * 0.5f,  // Центр по X
        maxs[1],                         // Верхняя граница по Y
        (mins[2] + maxs[2]) * 0.5f   // Центр по Z
    );
}

float GetHeight() const {
    return maxs.n[VY] - mins.n[VY];
}

#if ENGINE == Engine_G1
//FIXME_G1
//zBOOL IsIntersecting(const zPOINT3& rayOrigin, const zVEC3& rayDirection, zREAL& scaleMin, zREAL& scaleMax);

#endif

#if ENGINE < Engine_G2A
zBOOL IsIntersecting(const zVEC3& rayOrigin, const zVEC3& rayDirection, zREAL& scaleMin, zREAL& scaleMax) const;
#endif