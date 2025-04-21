// Supported with union (c) 2020 Union team

// User API for zCMaterial
// Add your methods here

zBOOL					SetName(const zSTRING& name) { return SetObjectName(name); };
void					SetNoCollDet(const zBOOL b) { noCollDet = b; };

zBOOL					GetNoCollDet(const zBOOL a_bIgnoreOverrides = FALSE) const
{
	if (a_bIgnoreOverrides)		  return noCollDet;
};

void					SetUsage(const zBYTE usage) { matUsage = usage; };
void					SetUsageFromDefault() { matUsage = s_matUsageDefault; };
zCOLOR					GetColor() const { return color; };
zTRnd_AlphaBlendFunc	GetAlphaBlendFunc() const { return rndAlphaBlendFunc; };
zBOOL					GetTexAniMapping() const { return texAniMap; };
zVEC2					GetTexAniMappingDir() const { return texAniMapDelta; };



zTMat_Group				GetMatGroup() const { return matGroup; };
void					SetMatGroup(const zTMat_Group group_) { matGroup = group_; };

zCTexture*				GetDetailTexture() const { return detailTexture; }
void					SetDetailTextureScale(const zREAL scale) { detailTextureScale = scale; }
zREAL					GetDetailTextureScale() const { return detailTextureScale; }
zCTexture*				GetTexture() const { return texture; };
zBOOL					GetSmooth() const { return smooth; };



void					SetAlpha(const zBYTE a) { color.alpha = a; };
zBYTE					GetAlpha() const { return color.alpha; };

#if ENGINE >= Engine_G2
zBOOL					GetEnvMapEnabled() const { return m_bEnvironmentalMapping; };
void					SetEnvMapEnabled(const zBOOL a_bEnEnvMap) { m_bEnvironmentalMapping = a_bEnEnvMap; };
void					SetEnvMapStrength(const zREAL a_bEnvMapStr) { m_bEnvironmentalMappingStrength = a_bEnvMapStr; };
zREAL					GetEnvMapStrength()	const { return m_bEnvironmentalMappingStrength; };

zBOOL					GetOccluder() const { return forceOccluder; };
void					SetOccluder(const zBOOL b) { forceOccluder = b; };

#endif

