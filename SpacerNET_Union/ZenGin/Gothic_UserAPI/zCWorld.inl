// Supported with union (c) 2020 Union team

// User API for zCWorld
// Add your methods here


void				SetWorldRenderMode(const zTWld_RenderMode& rmode) { worldRenderMode = rmode; bspTree.worldRenderMode = rmode; };
zCWayNet*			GetWayNet() const { return wayNet; };
zBOOL				IsCompiled() const { return compiled; };
void				CollectVobsInBBox3D(zCArray<zCVob*>& resVobList, const zTBBox3D& inbbox3D) { bspTree.bspRoot->CollectVobsInBBox3D(resVobList, inbbox3D); };
void				SetProgressBar(zCViewProgressBar *progressBar) { this->progressBar = progressBar; };

zCWorld* zCWorld_Hook();

zBOOL zCWorld::SaveWorldNew(const zSTRING& fileName, const zTWorldSaveMode saveMode, const zBOOL writeBinary, const zBOOL _saveLevelMesh);

BOOL zCWorld::LoadWorld_Union(const zSTRING& fileName, const zTWorldLoadMode loadMode);

zBOOL __fastcall zCWorld::PickScene_Material(zCCamera& cam, int xscr, int yscr, zREAL rayLength);

void zCWorld::GenerateSurfaces_Union(const zBOOL doRaytracing, zTBBox3D* updateBBox3D);