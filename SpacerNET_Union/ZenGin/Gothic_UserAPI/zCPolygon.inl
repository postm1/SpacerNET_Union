// Supported with union (c) 2020 Union team

// User API for zCPolygon
// Add your methods here

zCVertex*		__fastcall			GetVertexPtr(int index)
{
	return vertex[index];
}
void			__fastcall			SetVertexPtr(zCVertex* vert, int index)
{
	vertex[index] = vert;
}

void			__fastcall			SetVertexPtr(zCVertex& vert, int index)
{
	zCVertex *newVert = zNEW(zCVertex);
	*newVert = vert;
	vertex[index] = newVert;
}

zCMaterial* GetMaterial() const { return material; };

#if ENGINE == Engine_G1
void SetMaterial(zCMaterial* mat);
#endif

zCLightMap* GetLightmap() const { return lightmap; };
zBOOL			GetPortal() const { return flags.portalPoly != zPORTAL_TYPE_NONE; };
zBOOL			GetSectorFlag() const { return flags.sectorPoly; };
const zVEC3& GetNormal() const { return polyPlane.normal; };

zBOOL			IsPortal() const { return flags.portalPoly != zPORTAL_TYPE_NONE; };
const zTPlane& GetPlane() const { return polyPlane; };
//void			SetLightmap(zCLightMap* lm) { if (lightmap == lm) return; if (lightmap) lightmap->Release(); lightmap = lm; if (lm) lm->AddRef(); };

zBOOL			GetGhostOccluder() const { return flags.ghostOccluder; };

zBOOL			GetOccluder() const { return flags.occluder; };
void			SetOccluder(const zBOOL p) { flags.occluder = p; };
zBOOL			IsOccluder() const { return flags.occluder; };


