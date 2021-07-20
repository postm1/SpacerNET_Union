// Supported with union (c) 2020 Union team

// User API for zCMesh
// Add your methods here

#define zERR_ASSERT(e)					((void)(0))


zCVertex       *Vertex(int i) const { zERR_ASSERT(0 <= i && i < numVert && vertList); return vertList[i]; }
zCPolygon      *Poly(int i) const { zERR_ASSERT(0 <= i && i < numPoly && polyList); return polyList[i]; }
zCVertFeature  *Feature(int i) const { zERR_ASSERT(0 <= i && i < numFeat && featList); return featList[i]; }
void SortPolysByList_Hook(zCPolygon** list, int listLength);
void ShareFeatures_Union();