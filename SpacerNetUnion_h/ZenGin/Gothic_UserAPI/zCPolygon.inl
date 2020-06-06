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