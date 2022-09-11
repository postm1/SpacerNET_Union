// Supported with union (c) 2020 Union team

// User API for zCLightMap
// Add your methods here

void zCLightMap::GetLightmapTexCoords(const zVEC3& posWorldSpace, float& u, float& v) const {
	const zVEC3& q = posWorldSpace - lightmapOrigin;
	u = q.Dot(lightmapUVRight);
	v = q.Dot(lightmapUVUp);
};

const zVEC2 zCLightMap::GetLightmapTexCoords(const zVEC3& posWorldSpace) const {
	const zVEC3& q = posWorldSpace - lightmapOrigin;
	return zVEC2(q.Dot(lightmapUVRight), q.Dot(lightmapUVUp));
};

void					SetLightmapOriginUpRight(const zVEC3& origin, const zVEC3& up, const zVEC3& right) { lightmapOrigin = origin; lightmapUVUp = up; lightmapUVRight = right; };
void					GetLightmapOriginUpRight(zVEC3& origin, zVEC3& up, zVEC3& right) const { origin = lightmapOrigin; up = lightmapUVUp; right = lightmapUVRight; };

