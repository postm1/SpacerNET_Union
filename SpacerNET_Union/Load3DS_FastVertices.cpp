// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	// global engine vars

	int& numPoly = *(int*)0x8D85DC;
	int& numVert = *(int*)0x8D85C8;
	static zCMesh*& targetMesh = *(zCMesh**)0x8D85D8;
	static TVertex3ds*& vertList = *(TVertex3ds**)0x8D85C0;
	static TPoly3ds*& polyList = *(TPoly3ds**)0x8D85CC;

	// HASH FUNCTION
	struct Point3Hasher {
		size_t operator()(const zPOINT3& p) const {
			const float epsilon = 0.01f;
			size_t h1 = std::hash<int>()(static_cast<int>(p.n[VX] / epsilon));
			size_t h2 = std::hash<int>()(static_cast<int>(p.n[VY] / epsilon));
			size_t h3 = std::hash<int>()(static_cast<int>(p.n[VZ] / epsilon));
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};

	// Comparator
	struct Point3Equal {
		bool operator()(const zPOINT3& a, const zPOINT3& b) const {
			const zVALUE ALMOST_ZERO = 0.01f;
			return (fabs(a.n[VX] - b.n[VX]) < ALMOST_ZERO &&
				fabs(a.n[VY] - b.n[VY]) < ALMOST_ZERO &&
				fabs(a.n[VZ] - b.n[VZ]) < ALMOST_ZERO);
		}
	};

	std::unordered_map<zPOINT3, zCVertex*, Point3Hasher, Point3Equal> vertexMap;


	zCVertex* FindVertexFast(const zPOINT3& v)
	{
		auto it = vertexMap.find(v);
		return (it != vertexMap.end()) ? it->second : nullptr;
	}

	void AddVertexToMap(zCVertex* vert) {
		if (!vert) return;
		vertexMap[vert->position] = vert;
	}

	void zCMesh::BuildVertexMap()
	{
		vertexMap.clear();
		vertexMap.reserve(numVert);

		for (int i = 0; i < numVert; i++)
		{
			AddVertexToMap(vertList[i]);
		}
	}


	//0x00568B80 public: class zCVertex * __thiscall zCMesh::AddVertexSmart(class zVEC3 &)
	HOOK Ivk_zCMesh_AddVertexSmart AS(&zCMesh::AddVertexSmart, &zCMesh::AddVertexSmart_Union);
	zCVertex* zCMesh::AddVertexSmart_Union(zVEC3& a)
	{
		this->ArraysToLists();

		//zCVertex* vert = VertexInMesh(a); // original function
		zCVertex* vert = FindVertexFast(a); // new system, find key in hashmap

		if (vert)
		{
			return vert;
		}

		auto result = AddVertex(a);

		if (result)
		{
			AddVertexToMap(result);
		}

		return result;
	}

	//void sub_55D090() = FlushMeshBuffer
	void FlushMeshBuffer_Union();
	CInvoke <void(__cdecl*) (void)> pFlushMeshBufferInv(0x55D090, FlushMeshBuffer_Union, IVK_AUTO);
	void FlushMeshBuffer_Union()
	{
		//pFlushMeshBufferInv(); // original func

		if (numPoly == 0) return;

		int i = 0;

		// Vertices
		targetMesh->AllocVerts(numVert);
		zCVertex** newVertList = zNEW(zCVertex*)[numVert];

		zERR_MESSAGE(3, zERR_BEGIN, "D: FlushMeshBuffer (" + zSTRING(numVert) + " verts)");

		targetMesh->BuildVertexMap(); // build new hash map from existring vertices

		//RX_Begin(32);

		for (i = 0; i < numVert; i++)
		{

			newVertList[i] = targetMesh->AddVertexSmart(vertList[i].position);
			if ((i != 0) && ((i & 4095) == 0)) zERR_MESSAGE(3, 0, "D: ... still working (" + zSTRING(i) + " verts)");
		};

		//RX_End(32);

		zERR_MESSAGE(3, zERR_END, "");


		// Polys 
		targetMesh->AllocPolys(numPoly);


		zCPolygon* poly;
		for (i = 0; i < numPoly; i++)
		{
			//polyList[i].CheckForWrap(); // not used by engine
			poly =
				targetMesh->AddPoly(newVertList[polyList[i].vertIndex[0]],
					newVertList[polyList[i].vertIndex[1]],
					newVertList[polyList[i].vertIndex[2]]);

			poly->SetMapping(0, vertList[polyList[i].vertIndex[0]].texCoord);
			poly->SetMapping(1, vertList[polyList[i].vertIndex[1]].texCoord);
			poly->SetMapping(2, vertList[polyList[i].vertIndex[2]].texCoord);

			poly->material = polyList[i].material;
			if (poly->material == 0) {
				zERR_FATAL("D: zFile3d(): Mesh has polys without material !");
			};


			if (poly->material) {
				poly->material->SetUsageFromDefault();
			};
		};



		delete[] newVertList;
		delete[] polyList;
		delete[] vertList;
		polyList = 0;
		vertList = 0;
		numVert = 0;
		numPoly = 0;


		/*
		cmd << "All: " << RX_PerfString(30) << endl;
		cmd << "#1: " << RX_PerfString(31) << endl;
		cmd << "#2: " << RX_PerfString(32) << endl;
		cmd << "#3: " << RX_PerfString(33) << endl;
		cmd << "#4: " << RX_PerfString(34) << endl;
		*/
	}


	//test function, not used
	//0x00568350 private: class zCVertex * __thiscall zCMesh::VertexInMesh(class zVEC3 const &)
	//HOOK Ivk_zCMesh_VertexInMesh AS(&zCMesh::VertexInMesh, &zCMesh::VertexInMesh_Union);
	zCVertex* zCMesh::VertexInMesh_Union(const zVEC3& v)
	{
		ArraysToLists();

		if (s_usesVertexCache)
		{
			//cmd << "Cache ok" << endl;

			static zCVertex s_compareVert;
			s_compareVert.position = v;
			int vertIndex = s_vertexCache.Search(&s_compareVert);
			if (vertIndex == -1) return FALSE;
			return s_vertexCache[vertIndex];
		}

		int i;
		for (i = 0; i < numVert; i++)
		{
			const zVALUE ALMOST_ZERO = 0.01F;		// = cm
			const zPOINT3& p1 = vertList[i]->position;
			if ((fabs(p1.n[VX] - v.n[VX]) < ALMOST_ZERO) &&
				(fabs(p1.n[VY] - v.n[VY]) < ALMOST_ZERO) &&
				(fabs(p1.n[VZ] - v.n[VZ]) < ALMOST_ZERO)) return vertList[i];

		};
		return FALSE;
	}
}