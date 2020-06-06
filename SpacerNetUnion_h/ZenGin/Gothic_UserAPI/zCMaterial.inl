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