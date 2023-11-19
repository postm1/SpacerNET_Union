// Supported with union (c) 2020 Union team

// User API for zCRenderManager
// Add your methods here

void __fastcall zCRenderManager::BuildShader_Union(zCMaterial		*mat,				// IN
	zCTexture		*lightmap,
	zDWORD			dlightmap,
	zBOOL			useScratchPadShader,
	zBOOL			bInSector,
	zCShader*		&shader
);


void __fastcall zCRenderManager::DrawVertexBuffer_Union(zCVertexBuffer *vertexBufferIn,
	int			firstVert,
	int			numVert,
	zWORD			*indexList,
	zDWORD			numIndex,
	zCMaterial		*material,
	zBOOL			bInSector
);

void __fastcall zCRenderManager::PackVB_union(const zCArray<zCPolygon*> &polyList, zCShader *shaderIn);

zBOOL			S_GetDetailTexturesEnabled() { return s_detailTexturesEnabled; };
void __fastcall zCRenderManager::SetTextureTransform_Union(zCShader *shader, zDWORD shaderStageIndex, zDWORD hardwareStageIndex);