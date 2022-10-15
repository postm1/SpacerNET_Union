// Supported with union (c) 2020 Union team
// Union SOURCE file

#include <unordered_map>

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	/*
	static int Compare_Verts_Merge(const void *arg1, const void *arg2)
	{
		const zVALUE	ALMOST_ZERO = 0.01F;		// = cm
		const zVEC3		p1 = (*((zCVertex**)arg1))->position;
		const zVEC3		p2 = (*((zCVertex**)arg2))->position;
		const zREAL		p1x = p1.n[VX];
		const zREAL		p1y = p1.n[VY];
		const zREAL		p1z = p1.n[VZ];
		const zREAL		p2x = p2.n[VX];
		const zREAL		p2y = p2.n[VY];
		const zREAL		p2z = p2.n[VZ];

		// erster test auf gleichheit
		if ((zAbs(p1x - p2x) < ALMOST_ZERO) &&
			(zAbs(p1y - p2y) < ALMOST_ZERO) &&
			(zAbs(p1z - p2z) < ALMOST_ZERO)) return 0;			// equal	

		if ((p1x - p2x) < 0) return -1;
		else if ((p1x - p2x) > 0) return +1;
		else
		{
			// die x koords sind gleich, dann die y coords vergleichen
			if ((p1y - p2y) < 0) return -1;
			else if ((p1y - p2y) > 0) return +1;
			else
			{
				// die x und die y coords sind gleich, kann dann nur noch z unterschiedlich sein
				if ((p1z - p2z) < 0) return -1;
				else if ((p1z - p2z) > 0) return +1;
			}
		}

	//	zERR_FAULT("C: Compare_Verts_Merge: Bug!!! should be never here!");
		// hier darf man niemals hinkommen
		return 0;

	}

	//0x005680A0 public: static void __cdecl zCMesh::S_InitVertexMergeCache(class zCMesh *)

	//Common::Map<size_t, zCVertex*> fastMap;
	std::unordered_map<std::string, zCVertex*> fastMap;
	std::hash<std::string> hasher;

	void  __cdecl zCMeshS_InitVertexMergeCache(zCMesh*);
	CInvoke <void(__cdecl *) (zCMesh *)> pS_InitVertexMergeCache(0x005680A0, zCMeshS_InitVertexMergeCache, IVK_AUTO);
	void  __cdecl zCMeshS_InitVertexMergeCache(zCMesh* a_pMesh)
	{
		cmd << "zCMeshS_InitVertexMergeCache" << endl;



		//zERR_MESSAGE(6, 0, "C: Meshmagic: Initializing cache for vertex merging");
		zCMesh::s_vertexCache.DeleteList();

		fastMap.clear();

		RX_Begin(1);

		zCMesh::s_vertexCache.SetCompare(Compare_Verts_Merge);
		// alle gespeicherten verts (falls vorhanden) 
		// in die neue sortierte liste kopieren
	
			//zCMesh::s_vertexCache.Insert(a_pMesh->vertList[i]);

		for (int j = 0; j < a_pMesh->numVert; j++)
		{
			zSTRING str = a_pMesh->vertList[j]->position.ToString();
			Stack_PushString(str);
			theApp.exports.getShaString();
			std::string hash = Stack_PeekString();

			//cmd << hash.c_str() << endl;
			//Message::Box("stop");

			if (fastMap.find(hash) == fastMap.end())
			{
				fastMap[hash] = a_pMesh->vertList[j];
			}
			else
			{
				cmd << "!! in list!!! " << hash.c_str() << endl;
				//cmd << "already in list hash!" << endl;
			}
				

			//Message::Box("stop 2");
			//cmd << "Added into: " + hash << endl;

			if (j % 1000 == 0)
			{
				//cmd << "Vertex: " << j << endl;
			}
		}

	
		RX_End(1);

		cmd << RX_PerfString(1) << endl;
			
		
		cmd << "zCMeshS_InitVertexMergeCache OK..." << endl;
		//zCMesh::s_vertexCache.BestSort();

		zCMesh::s_usesVertexCache = TRUE;
	}


	void  __cdecl zCMeshS_DeleteVertexMergeCache();
	CInvoke <void(__cdecl *) ()> pzCMeshS_DeleteVertexMergeCache(0x00568320, zCMeshS_DeleteVertexMergeCache, IVK_AUTO);
	void  __cdecl zCMeshS_DeleteVertexMergeCache()
	{
		zCMesh::s_vertexCache.DeleteList();
		zCMesh::s_usesVertexCache = FALSE;
		fastMap.clear();
	}

	//0x00568350 private: class zCVertex * __thiscall zCMesh::VertexInMesh(class zVEC3 const &)

	HOOK ivk_VertexInMesh_Union AS(&zCMesh::VertexInMesh, &zCMesh::VertexInMesh_Union);
	zCVertex* zCMesh::VertexInMesh_Union(zVEC3 const & v)
	{

		ArraysToLists();

		if (s_usesVertexCache)
		{
			
			//static zCVertex s_compareVert;
			//s_compareVert.position = v;
			//int vertIndex = s_vertexCache.Search(&s_compareVert);
			//if (vertIndex == -1) return FALSE;
			//return s_vertexCache[vertIndex];
			

			auto pVec = v;

			zSTRING str = pVec.ToString();
			Stack_PushString(str);
			theApp.exports.getShaString();
			std::string hash = Stack_PeekString();

			//auto& foundPair = fastMap[hash];


			if (fastMap.find(hash) == fastMap.end())
			{
				return FALSE;
			}	
			else
			{
				return fastMap[hash];
			}

		}

		int i;
		for (i = 0; i<numVert; i++)
		{
			const zVALUE ALMOST_ZERO = 0.01F;		// = cm
			const zPOINT3& p1 = vertList[i]->position;
			if ((zAbs(p1.n[VX] - v.n[VX]) < ALMOST_ZERO) &&
				(zAbs(p1.n[VY] - v.n[VY]) < ALMOST_ZERO) &&
				(zAbs(p1.n[VZ] - v.n[VZ]) < ALMOST_ZERO)) return vertList[i];

		};
		return FALSE;
	}

	//0x005688F0 public: class zCVertex * __thiscall zCMesh::AddVertex(class zVEC3 &)
	HOOK ivk_AddVertex AS(&zCMesh::AddVertex, &zCMesh::AddVertex_Union);
	zCVertex* zCMesh::AddVertex_Union(zVEC3 & a)
	{

		ArraysToLists();

		if (numVertAlloc<numVert + 1) AllocVerts(zMax(1, numVert / 2));

		vertList[numVert] = zNEW(zCVertex);
		vertList[numVert]->position = a;

		if (s_usesVertexCache)
		{
			zSTRING str = vertList[numVert]->position.ToString();
			Stack_PushString(str);
			theApp.exports.getShaString();
			std::string hash = Stack_PeekString();


			if (fastMap.find(hash) == fastMap.end())
			{
				fastMap[hash] = vertList[numVert];
			}
			else
			{
				
				//cmd << "already in list hash!" << endl;
			}

			//fastMap.Insert(hash, vertList[numVert]);
			
			//s_vertexCache.InsertSort(vertList[numVert]);
		}

		numVert++;
		return vertList[numVert - 1];
	}
	*/

}