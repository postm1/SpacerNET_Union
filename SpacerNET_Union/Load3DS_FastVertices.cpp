// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

#if ENGINE == Engine_G2A
	// global engine vars

	static TVertex3ds*& vertList = *(TVertex3ds**)0x8D85C0;
	static TPoly3ds*& polyList = *(TPoly3ds**)0x8D85CC;
	

	constexpr float COMPARE_EPSILON = 0.01f; // Accuracy for comparison 1 cm
	constexpr float HASH_GRID_SIZE = 0.5f;   // Hash size 50 cm
	constexpr float inv_grid = 1.0f / HASH_GRID_SIZE;

	// Hash function
	struct Point3Hasher {
		size_t operator()(const zPOINT3& p) const {
			/*
			int x = static_cast<int>(std::roundf(p.n[VX] * inv_grid));
			int y = static_cast<int>(std::roundf(p.n[VY] * inv_grid));
			int z = static_cast<int>(std::roundf(p.n[VZ] * inv_grid));
			return ((x * 73856093) ^
				(y * 19349663) ^
				(z * 83492791));
			*/

			/*
			__m128 vec;
			float temp[4] = { p.n[VX], p.n[VY], p.n[VZ], 0.0f };
			vec = _mm_loadu_ps(temp); // _mm_loadu_ps = unaligned load

			const __m128 grid_inv = _mm_set1_ps(1.0f / HASH_GRID_SIZE);
			__m128 scaled = _mm_mul_ps(vec, grid_inv);
			__m128i rounded = _mm_cvtps_epi32(scaled); // float -> int32

			alignas(16) int32_t coords[4];
			_mm_storeu_si128((__m128i*)coords, rounded); // unaligned store


			return (coords[0] * 73856093) ^
				(coords[1] * 19349663) ^
				(coords[2] * 83492791);
				*/

			/*
				// «агружаем только x,y,z, игнориру€ w компонент
			__m128 vec = _mm_set_ps(0.0f, p.n[VZ], p.n[VY], p.n[VX]);

			const __m128 grid_inv = _mm_set1_ps(1.0f / HASH_GRID_SIZE);
			__m128 scaled = _mm_mul_ps(vec, grid_inv);

			// явное округление к ближайшему целому (как std::roundf)
			scaled = _mm_add_ps(scaled, _mm_set1_ps(0.5f));
			scaled = _mm_sub_ps(scaled, _mm_and_ps(_mm_cmplt_ps(scaled, _mm_setzero_ps()), _mm_set1_ps(1.0f)));

			__m128i rounded = _mm_cvtps_epi32(scaled);

			// —охран€ем результаты
			int32_t coords[4];  // alignas не нужен дл€ _mm_storeu_si128
			_mm_storeu_si128(reinterpret_cast<__m128i*>(coords), rounded);

			return (coords[0] * 73856093) ^
				(coords[1] * 19349663) ^
				(coords[2] * 83492791);

			*/

			// Load XYZ components directly (W=0)
			__m128 vec = _mm_set_ps(0.0f, p.n[VZ], p.n[VY], p.n[VX]);

			// Scale and round to nearest integer
			const __m128 grid_inv = _mm_set1_ps(inv_grid);
			__m128 scaled = _mm_mul_ps(vec, grid_inv);
			__m128i rounded = _mm_cvtps_epi32(_mm_round_ps(scaled, _MM_FROUND_TO_NEAREST_INT));

			// Store and hash coordinates
			alignas(16) int32_t coords[4];
			_mm_store_si128(reinterpret_cast<__m128i*>(coords), rounded);

			// Better hash mixing
			return (coords[0] * 73856093u) ^
				(coords[1] * 19349663u) ^
				(coords[2] * 83492791u);
		}
	};

	// Comparator
	struct Point3Equal {
		bool operator()(const zPOINT3& a, const zPOINT3& b) const 
		{
			/*
			const float dx = std::abs(a.n[VX] - b.n[VX]);
			const float dy = std::abs(a.n[VY] - b.n[VY]);
			const float dz = std::abs(a.n[VZ] - b.n[VZ]);

			const float eps = COMPARE_EPSILON + std::numeric_limits<float>::epsilon();
			return (dx < eps) && (dy < eps) && (dz < eps);
			*/
			
			__m128 vec_a = _mm_set_ps(0.0f, a.n[VZ], a.n[VY], a.n[VX]);
			__m128 vec_b = _mm_set_ps(0.0f, b.n[VZ], b.n[VY], b.n[VX]);

			// Calculate absolute differences
			__m128 diff = _mm_sub_ps(vec_a, vec_b);
			__m128 abs_diff = _mm_max_ps(_mm_sub_ps(_mm_setzero_ps(), diff), diff);

			// Compare with epsilon (masking out W component)
			const __m128 eps = _mm_set1_ps(COMPARE_EPSILON);
			__m128 cmp = _mm_cmplt_ps(abs_diff, eps);

			return (_mm_movemask_ps(cmp) & 0x7) == 0x7;
		}
	};

	std::unordered_map<zPOINT3, zCVertex*, Point3Hasher, Point3Equal> vertexMap;
	bool isNewMapActiveNow = false;
	
	//debug only
	int counter0 = 0;
	int counter1 = 0;

	zCVertex* FindVertexFast(const zPOINT3& v)
	{
		auto it = vertexMap.find(v);
		return (it != vertexMap.end()) ? it->second : nullptr;
	}

	void AddVertexToMap(zCVertex* vert) 
	{
		if (!vert) return;
		vertexMap[vert->position] = vert;
	}

	void ClearVertexMap() 
	{
		vertexMap.clear();
		isNewMapActiveNow = false;
	}

	void zCMesh::BuildVertexMap()
	{
		ClearVertexMap();

		vertexMap.reserve(numVert);

		for (int i = 0; i < numVert; i++)
		{
			AddVertexToMap(vertList[i]);
		}

		isNewMapActiveNow = true;
	}

	

	//0x00568B80 public: class zCVertex * __thiscall zCMesh::AddVertexSmart(class zVEC3 &)
	HOOK Ivk_zCMesh_AddVertexSmart AS(&zCMesh::AddVertexSmart, &zCMesh::AddVertexSmart_Union);
	zCVertex* zCMesh::AddVertexSmart_Union(zVEC3& a)
	{
		this->ArraysToLists();

		zCVertex* vert = NULL;

		if (isNewMapActiveNow)
		{
			vert = FindVertexFast(a); // new system, find key in hashmap

			// debug function which compares engine / hasmap result
			// seems good for now
			
			/*
			auto vert2 = VertexInMesh(a); // original function

			if (vert != vert2)
			{
				cmd << "\nBad vec: " << a.ToString() <<  endl;

				if (vert)
				{
					cmd << "New: " << (int)vert << " " << vert->position.ToString() << endl;
				}

				if (vert2)
				{
					cmd << "Old: " << (int)vert2 << " " << vert2->position.ToString() << endl;
				}
			}
			*/
			
			
		}
		else
		{
			vert = VertexInMesh(a); // original function
		}


		if (vert)
		{
			counter0++;
			return vert;
		}

		auto result = AddVertex(a);

		if (result && isNewMapActiveNow)
		{
			
			AddVertexToMap(result);
		}

		counter1++;

		return result;
	}

	//void sub_55D090() = FlushMeshBuffer
	void FlushMeshBuffer_Union();
	CInvoke <void(__cdecl*) (void)> pFlushMeshBufferInv(0x55D090, FlushMeshBuffer_Union, IVK_AUTO);
	void FlushMeshBuffer_Union()
	{
		//pFlushMeshBufferInv(); // original func

		zERR_MESSAGE(3, 0, "D: FlushMeshBuffer_Union: " + Z numPoly + " polys");

		if (numPoly == 0) return;

		int i = 0;

		// Vertices
		targetMesh->AllocVerts(numVert);
		zCVertex** newVertList = zNEW(zCVertex*)[numVert];

		zERR_MESSAGE(3, zERR_BEGIN, "D: FlushMeshBuffer (" + zSTRING(numVert) + " verts)");

		targetMesh->BuildVertexMap(); // build new hash map from existring vertices

		counter0 = 0;
		counter1 = 0;

		RX_Begin(5);


		for (i = 0; i < numVert; i++)
		{

			newVertList[i] = targetMesh->AddVertexSmart(vertList[i].position);
			if ((i != 0) && ((i & 4095) == 0)) zERR_MESSAGE(3, 0, "D: ... still working (" + zSTRING(i) + " verts)");
		};


		RX_End(5);

		

		ClearVertexMap();

		//cmd << numVert << " " << counter0 << "/" << counter1 << endl;

		

		zERR_MESSAGE(3, 0, "D: AddVertexSmartAll: " + RX_PerfString(5));

		RX_Begin(6);
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

		RX_End(6);

		zERR_MESSAGE(3, 0, "D: FlushMeshBuffer_Union: " + RX_PerfString(6));



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
#endif
}