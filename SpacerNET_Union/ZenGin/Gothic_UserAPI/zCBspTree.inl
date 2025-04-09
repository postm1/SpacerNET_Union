// Supported with union (c) 2020 Union team

// User API for zCBspTree
// Add your methods here

void AddVob_Hook(zCVob* vob);
void RenderVobList_Hook();

zBOOL					CollectPolysInBBox3D(const zTBBox3D& testbbox3D, zCPolygon**& foundPolyList, int& foundPolyNum) const { return bspRoot->CollectPolysInBBox3D(testbbox3D, foundPolyList, foundPolyNum); };