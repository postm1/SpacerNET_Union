// Supported with union (c) 2020 Union team

// User API for zCCBspTree
// Add your methods here

void zCCBspTree::AddMesh_Union(zCMesh* inMesh, const zMAT4& trafoObjToWorld);
void zCCBspTree::GetPolyNeighbours_Union(zCPolygon* sourcePoly, zCPolygon**& foundPolyList, int& foundPolyNum);