// Supported with union (c) 2020 Union team

// User API for zCVobLightData
// Add your methods here

zREAL				GetSpotConeAngle() const { return spotConeAngleDeg; };
zTVobLightType		GetLightType()  const { return zTVobLightType(lightType); };