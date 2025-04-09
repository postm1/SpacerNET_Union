// Supported with union (c) 2020 Union team

// User API for zCVobLight
// Add your methods here

static void LoadLightPresets_Hook();

zCOLOR			GetColor() const { return lightData.lightColor; };
zREAL			GetRange() const { return lightData.range; };
zREAL			GetRangeInv() const { return lightData.rangeInv; };

zTVobLightType	GetLightType()  const { return lightData.GetLightType(); };

