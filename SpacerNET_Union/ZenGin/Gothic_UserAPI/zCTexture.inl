// Supported with union (c) 2020 Union team

// User API for zCTexture
// Add your methods here

const zVEC2 GetTexCoords(const zVEC3& posWorldSpace, zVEC3& polyCenter) const {
	const zVEC3& q = posWorldSpace - polyCenter;

	return zVEC2(q.Dot(zVEC3(1, 0, 0)), q.Dot(zVEC3(0, 1, 0)));
};