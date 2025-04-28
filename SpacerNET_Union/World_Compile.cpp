// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	static float GetPolyNeighbourPerfCounter = 0;

//#define GETPOLYS_TEST

#if ENGINE == Engine_G2A
	int& s_bAddVobsToMesh = *(int*)0x008D8870;

	HOOK Ivk_zCMesh_MergeMesh AS(&zCMesh::MergeMesh, &zCMesh::MergeMesh_Union);
	void zCMesh::MergeMesh_Union(zCMesh* mesh, const zMAT4& trafo)
	{
		

		// in some special cases we use old funciton
		if (s_bAddVobsToMesh || isNewMapActiveNow || trafo.GetTranslation().Length() > 0 || (mesh && mesh->numVert == 0))
		{
			zERR_MESSAGE(3, zERR_BEGIN, "D: MergeMesh_Union OLD CALLED ");
			RX_Begin(2);
			THISCALL(Ivk_zCMesh_MergeMesh)(mesh, trafo);
			RX_End(2);
			zERR_MESSAGE(3, zERR_END, "");
			return;
		}

		zERR_MESSAGE(3, zERR_BEGIN, "D: MergeMesh_Union NEW CALLED ");
		RX_Begin(2);

		ArraysToLists();
		UnshareFeatures();
 
		int i = 0;
		AllocVerts(mesh->numVert);
		AllocPolys(mesh->numPoly);

		zfpuControler->SaveCurrentControlWord();
		zfpuControler->SetPrecision_64();

		
		/*
		cmd << "MergeMesh_Union #2. Numvert: " << mesh->numVert 
			<< " NumPoly: " << numPoly
			<< " GetTranslation: " << trafo.GetTranslation().ToString()
			<< " GetRightVector: " << trafo.GetRightVector().ToString()
			<< " GetAtVector: " << trafo.GetAtVector().ToString()
			<< " GetUpVector: " << trafo.GetUpVector().ToString()

			<< endl;
		*/

		


		zCVertex** newVert = zNEW(zCVertex*)[mesh->numVert];

		std::unordered_map<zCVertex*, zCVertex*> vertexMap;

		vertexMap.reserve(mesh->numVert);

		for (i = 0; i < mesh->numVert; i++) 
		{
			zCVertex* vert = zNEW(zCVertex);
			// Vert transformieren & kopieren 
			*vert = (*(mesh->vertList[i]));
			vert->position = mesh->vertList[i]->position;
			newVert[i] = this->AddVertex(vert);

			if (mesh->vertList[i] != NULL)
			{
				vertexMap[mesh->vertList[i]] = newVert[i];
			}
			else
			{
				//cmd << "NewVert " << i << " is NULL" << endl;
			}
		}



		//cmd << "MergeMesh_Union #4 newVertList: " << RX_PerfString(41) << endl;



		for (i = 0; i < mesh->numPoly; i++) {

			zCPolygon* poly = AddPoly();
			(*poly) = *(mesh->polyList[i]);

			poly->vertex = 0;
			poly->polyNumVert = 0;

			poly->AllocVerts(mesh->polyList[i]->polyNumVert);
			mesh->polyList[i]->CopyValuesInto(poly);

			for (int j = 0; j < poly->polyNumVert; j++) 
			{
				zCVertex* vert = mesh->polyList[i]->vertex[j];
				poly->vertex[j] = vert;


				auto it = vertexMap.find(vert);

				if (it != vertexMap.end()) {

					//cmd << (int)poly->vertex[j] << " " << (int)it->second << " " << " " << (int)it->first << endl;
					poly->vertex[j] = it->second;
					//break;
				}
				else
				{
					//if no found in map, not real case, it must be in the list
					for (int k = 0; k < mesh->numVert; k++) {
						if (vert == mesh->vertList[k]) {
							//cmd << (int)vert << " " << (int)mesh->vertList[k] << endl;
							poly->vertex[j] = newVert[k];
							break;
						};
					};
				}

			};

			
			poly->CalcNormal();
		};

		delete[] newVert;
		

		zfpuControler->RestoreSavedControlWord();

		RX_End(2);

		cmd << "MergeMesh_Union: " << RX_PerfString(2) << endl;
		zERR_MESSAGE(3, zERR_END, "");
		GetPolyNeighbourPerfCounter = 0;
	}


	//void zCCBspTree::GetPolyNeighbours(zCPolygon* sourcePoly, zCPolygon**& foundPolyList, int& foundPolyNum) {
	HOOK Ivk_zCCBspTree_GetPolyNeighbours AS(&zCCBspTree::GetPolyNeighbours, &zCCBspTree::GetPolyNeighbours_Union);

	void zCCBspTree::GetPolyNeighbours_Union(zCPolygon* sourcePoly, zCPolygon**& foundPolyList, int& foundPolyNum)
	{

#ifdef GETPOLYS_TEST
		RX_Begin(3);

		THISCALL(Ivk_zCCBspTree_GetPolyNeighbours)(sourcePoly, foundPolyList, foundPolyNum);

		int savefoundPolyNum = foundPolyNum;
#endif // GETPOLYS_TEST


		zTBBox3D searchBox = sourcePoly->GetBBox3D();
		searchBox.Scale(1.1F);
		bspRoot.CollectPolysInBBox3D(searchBox, foundPolyList, foundPolyNum);

		std::sort(foundPolyList, foundPolyList + foundPolyNum);

		int writeIndex = 0;
		zCPolygon* lastPoly = nullptr;

		for (int i = 0; i < foundPolyNum; ++i) {
			zCPolygon* poly = foundPolyList[i];

			if (poly == sourcePoly || poly == lastPoly ||
				(poly->GetSectorFlag() && !poly->IsPortal())
				) {
				continue;
			}

			foundPolyList[writeIndex++] = poly;
			lastPoly = poly;
		}

		foundPolyNum = writeIndex;

#ifdef GETPOLYS_TEST

		if (savefoundPolyNum != foundPolyNum)
		{
			cmd << "WRONG!!!: " << savefoundPolyNum << " | " << foundPolyNum << endl;
		}

		RX_End(3);

		GetPolyNeighbourPerfCounter += perf[3];
#endif // GETPOLYS_TEST		


	}
	
	// unordered_set instead of old zCArray, much faster
	float zCBspTree::GetOccluderAreaRec_UnionMyNew(const zCPolygon* rootPoly, std::unordered_set<zCPolygon*>& nearOccluderPolys)
	{
		zCArray<zCPolygon*>	neighbourOccluder;
		zCPolygon** foundPolyList;
		int					foundPolyNum;
		zREAL				localArea = rootPoly->GetArea();
		zTBBox3D			searchBox = rootPoly->GetBBox3D();
		const zREAL			INC = zREAL(1.0F);

		if (searchBox.mins[VX] == searchBox.maxs[VX]) { searchBox.mins[VX] -= INC; searchBox.maxs[VX] += INC; };
		if (searchBox.mins[VY] == searchBox.maxs[VY]) { searchBox.mins[VY] -= INC; searchBox.maxs[VY] += INC; };
		if (searchBox.mins[VZ] == searchBox.maxs[VZ]) { searchBox.mins[VZ] -= INC; searchBox.maxs[VZ] += INC; };
		searchBox.Scale(zREAL(1.2F));		// scale x/z

		bspRoot->CollectPolysInBBox3D(searchBox, foundPolyList, foundPolyNum);

		for (int j = 0; j < foundPolyNum; j++)
		{
			zCPolygon* poly2 = foundPolyList[j];
			if (rootPoly == poly2)	 continue;

			if (poly2->IsOccluder() && !poly2->GetGhostOccluder() && (/*!nearOccluderPolys.IsInList(poly2)*/nearOccluderPolys.find(poly2) == nearOccluderPolys.end()))
			{
				if (poly2->IsNeighbourOf(rootPoly))
				{
					neighbourOccluder.InsertEnd(poly2);
					nearOccluderPolys.insert(poly2);
				}
			}
		}

		if (neighbourOccluder.GetNum() > 0)
		{
			for (int n = 0; n < neighbourOccluder.GetNum(); n++)
			{
				localArea += GetOccluderAreaRec_UnionMyNew(neighbourOccluder[n], nearOccluderPolys);
			}
			neighbourOccluder.DeleteList();
		}

		return localArea;
	}

	// new functions, but sometimes it has wrong results, so just skip this optimization...
	zBOOL __fastcall zCPolygon::IsIntersectingProjectionNew(zCPolygon* poly2, const zVEC3& projNormal)
	{
		zCPolygon* polyA = this;
		zCPolygon* polyB = poly2;

		// Determine projection axes using SSE for faster comparisons
		__m128 absNormal = _mm_set_ps(0.0f,
			zAbsApprox(projNormal[2]),
			zAbsApprox(projNormal[1]),
			zAbsApprox(projNormal[0]));

		int vz;
		if (zIsSmallerPositive(absNormal.m128_f32[0], absNormal.m128_f32[1])) {
			vz = zIsSmallerPositive(absNormal.m128_f32[1], absNormal.m128_f32[2]) ? 2 : 1;
		}
		else {
			vz = zIsSmallerPositive(absNormal.m128_f32[0], absNormal.m128_f32[2]) ? 2 : 0;
		}

		int vx = vz + 1; if (vx > 2) vx = 0;
		int vy = vx + 1; if (vy > 2) vy = 0;

		// Point-in-poly test
		for (int k = 0; k < 1; k++) {
			zPOINT3 polyACenter = polyA->GetCenter();
			__m128 center = _mm_loadu_ps(&polyACenter[0]);
			const float scale = 0.2f;
			__m128 scaleVec = _mm_set1_ps(scale);
			__m128 normalVec = _mm_loadu_ps(&polyA->polyPlane.normal[0]);

			for (int l = 0; l < polyA->polyNumVert; l++) {
				__m128 vertex = _mm_loadu_ps(&polyA->vertex[l]->position[0]);
				__m128 diff = _mm_sub_ps(center, vertex);
				__m128 scaled = _mm_mul_ps(diff, scaleVec);
				__m128 inters = _mm_add_ps(vertex, scaled);
				inters = _mm_sub_ps(inters, normalVec);

				float intersection[3];
				_mm_storeu_ps(intersection, inters);

				zBOOL inside = FALSE;
				for (int i = 0, j = polyB->polyNumVert - 1; i < polyB->polyNumVert; j = i++) {
					const zPOINT3& u = polyB->vertex[i]->position;
					const zPOINT3& v = polyB->vertex[j]->position;

					if ((u[vy] >= intersection[vy]) != (v[vy] >= intersection[vy])) {
						zBOOL right = u[vx] >= intersection[vx];
						if (right != (v[vx] >= intersection[vx])) {
							zREAL t = (u[vy] - intersection[vy]) / (u[vy] - v[vy]);
							if (u[vx] + (v[vx] - u[vx]) * t >= intersection[vx]) inside = !inside;
						}
						else if (right) inside = !inside;
					}
				}
				if (inside) return TRUE;
			}
		}

		// Edge-edge intersection test
		for (int k = 0; k < 1; k++) {
			zPOINT3 polyACenter = polyA->GetCenter();
			__m128 center = _mm_loadu_ps(&polyACenter[0]);
			const float scale = 0.1f;
			__m128 scaleVec = _mm_set1_ps(scale);
			__m128 normalVec = _mm_loadu_ps(&polyA->polyPlane.normal[0]);
			__m128 normalScale = _mm_set1_ps(2.0f);

			for (int l = 0; l < polyA->polyNumVert; l++) {
				int lnext = l + 1;
				if (lnext >= polyA->polyNumVert) lnext = 0;

				__m128 vertex1 = _mm_loadu_ps(&polyA->vertex[l]->position[0]);
				__m128 diff1 = _mm_sub_ps(center, vertex1);
				__m128 scaled1 = _mm_mul_ps(diff1, scaleVec);
				__m128 p1 = _mm_add_ps(vertex1, scaled1);
				p1 = _mm_sub_ps(p1, _mm_mul_ps(normalVec, normalScale));

				__m128 vertex2 = _mm_loadu_ps(&polyA->vertex[lnext]->position[0]);
				__m128 diff2 = _mm_sub_ps(center, vertex2);
				__m128 scaled2 = _mm_mul_ps(diff2, scaleVec);
				__m128 p2 = _mm_add_ps(vertex2, scaled2);
				p2 = _mm_sub_ps(p2, _mm_mul_ps(normalVec, normalScale));

				float p1_arr[3], p2_arr[3];
				_mm_storeu_ps(p1_arr, p1);
				_mm_storeu_ps(p2_arr, p2);

				for (int i = 0; i < polyB->polyNumVert; i++) {
					int inext = i + 1;
					if (inext >= polyB->polyNumVert) inext = 0;

					const zPOINT3& p3 = polyB->vertex[i]->position;
					const zPOINT3& p4 = polyB->vertex[inext]->position;

					zREAL b = (p4[vy] - p3[vy]) * (p2_arr[vx] - p1_arr[vx]) -
						(p4[vx] - p3[vx]) * (p2_arr[vy] - p1_arr[vy]);
					if (b == 0) continue;

					zREAL a = (p4[vx] - p3[vx]) * (p1_arr[vy] - p3[vy]) -
						(p4[vy] - p3[vy]) * (p1_arr[vx] - p3[vx]);
					zREAL c = (p2_arr[vx] - p1_arr[vx]) * (p1_arr[vy] - p3[vy]) -
						(p2_arr[vy] - p1_arr[vy]) * (p1_arr[vx] - p3[vx]);

					zREAL ua = a / b;
					if ((ua < 0.0f) || (ua > 1.0f)) continue;
					zREAL ub = c / b;
					if ((ub < 0.0f) || (ub > 1.0f)) continue;
					return TRUE;
				}
			}
		}

		return FALSE;
	}
	
	//zBOOL zCCFASTCALL	zCPolygon::IsIntersectingProjection (zCPolygon *poly2, const zVEC3& projNormal) 
	//HOOK Ivk_zCPolygon_IsIntersectingProjection AS(&zCPolygon::IsIntersectingProjection, &zCPolygon::IsIntersectingProjection_Union);
	zBOOL __fastcall zCPolygon::IsIntersectingProjection_Union(zCPolygon* poly2, const zVEC3& projNormal)
	{
		return IsIntersectingProjectionNew(poly2, projNormal);
	}



	// unordered_set instead of old zCArray/zCArraySort, much faster
	HOOK Ivk_zCCBspTree_MarkOccluderPolys AS(&zCBspTree::MarkOccluderPolys, &zCBspTree::MarkOccluderPolys_Union);
	void zCBspTree::MarkOccluderPolys_Union()
	{
		//THISCALL(Ivk_zCCBspTree_MarkOccluderPolys)();

		zERR_MESSAGE(3, 0, "D: RBSP: Marking Occluder Polys... PolysCount: " + Z mesh->numPoly);
		RX_Begin(4);

		int numOccluder = 0;
		for (int i = 0; i < mesh->numPoly; i++) 
		{
			zCPolygon* poly = mesh->SharePoly(i);

			if (poly->GetGhostOccluder())
			{
				poly->SetOccluder(TRUE);
				continue;
			};

			if (poly->GetMaterial())
			{
				if (poly->GetMaterial()->GetNoCollDet(TRUE))
				{
					poly->SetOccluder(FALSE);
					continue;
				}
				else if (poly->GetMaterial()->GetOccluder())
				{
					poly->SetOccluder(TRUE);
				};
			};


			if ((poly->polyPlane.normal[VY] < -0.01F) || (poly->polyPlane.normal[VY] > 0.8F))
			{
				poly->SetOccluder(FALSE);
				continue;
			};

			if (poly->GetMaterial()->GetTexture())
				poly->GetMaterial()->GetTexture()->CacheIn(-1);

			if (poly->GetMaterial()->GetTexture() && poly->GetMaterial()->GetTexture()->HasAlpha())
			{
				poly->SetOccluder(FALSE);
				continue;
			}

			if (poly->GetMaterial()->GetAlpha() < 255)
			{
				poly->SetOccluder(FALSE);
				continue;
			}

			if (poly->GetMaterial()->GetAlphaBlendFunc() > zRND_ALPHA_FUNC_NONE)
			{
				poly->SetOccluder(FALSE);
				continue;
			}

			if (poly->GetMaterial()->GetMatGroup() == zMAT_GROUP_WATER)
			{
				poly->SetOccluder(FALSE);
				continue;
			}

			zCPolygon** foundPolyList;
			int			foundPolyNum;
			zTBBox3D	polyBBox = poly->GetBBox3D();

			zTBBox3D searchBox = polyBBox;
			const zREAL INC = zREAL(1.0F);
			if (searchBox.mins[VX] == searchBox.maxs[VX]) { searchBox.mins[VX] -= INC; searchBox.maxs[VX] += INC; };
			if (searchBox.mins[VZ] == searchBox.maxs[VZ]) { searchBox.mins[VZ] -= INC; searchBox.maxs[VZ] += INC; };
			searchBox.Scale(zREAL(1.2F));
			searchBox.mins[VY] = -999999;
			searchBox.maxs[VY] = +999999;

			zBOOL		occluder = TRUE;
			{
				bspRoot->CollectPolysInBBox3D(searchBox, foundPolyList, foundPolyNum);
				for (int j = 0; j < foundPolyNum; j++) {
					zCPolygon* poly2 = foundPolyList[j];
					if (poly == poly2) continue;


					if (poly2->GetSectorFlag()) continue;

					if (poly2->polyPlane.normal[VY] < zREAL(0.0001F)) continue;

					if (poly2->GetMaterial()->GetMatGroup() == zMAT_GROUP_WATER) continue;

					if (poly->polyPlane.normal.IsEqualEps(poly2->polyPlane.normal)) continue;

					zTBBox3D poly2BBox = poly2->GetBBox3D();
					if (poly2BBox.maxs[VY] > polyBBox.mins[VY]) continue;

					auto resultOriginal = poly->IsIntersectingProjection(poly2, zVEC3(0, 1, 0));
					//auto result2 = poly->IsIntersectingProjectionNew(poly2, zVEC3(0, 1, 0));

					/*if (resultOriginal != result2)
					{
						cmd << "BAD RESULT -> ";

						if (resultOriginal)
						{
							cmd << "Old function result: TRUE";
						}
						else
						{
							cmd << "Old function result: FALSE";
						}

						for (int i = 0; i < poly2->polyNumVert; i++)
						{
							cmd << "\t" << i << ": " << poly2->vertex[i]->position.ToString() << endl;
						}
					}*/
					if (resultOriginal) { occluder = FALSE; break; };

				};
			};

			poly->SetOccluder(occluder);
			numOccluder += int(occluder);
		};

		RX_End(4);

		cmd << "MarkOccluderPolys #1: " << RX_PerfString(4) << endl;

		RX_Begin(4);


		//zCArray<zCPolygon*>		occluderNeighbours;
		std::unordered_set<zCPolygon*> occluderNeighbours;
		std::unordered_set<zCPolygon*> occluderTested;
		//zCArraySort<zCPolygon*> occluderTested;
		//occluderTested.SetCompare(Compare_Occluder);

		int maxNeighbourOccluders = 0;
		int maxArea = 0;

		zERR_MESSAGE(3, 0, "D: RBSP: Marking Occluder Polys = START CYCLE");

		occluderNeighbours.reserve(10000);
		occluderTested.reserve(10000);

		for (int i = 0; i < mesh->numPoly; i++)
		{
			zCPolygon* poly = mesh->SharePoly(i);

			static int count = 0;
			if ((count++ == 10000))
			{
				count = 0;
				zERR_MESSAGE(5, 0, "C: unmarking inefficient occluders, still working: " + zSTRING((float(i) / float(mesh->numPoly)) * 100));
			}

			if (!poly->IsOccluder())				 continue;
			if (poly->GetGhostOccluder())			 continue;
			//if (occluderTested.IsInList(poly))		 continue;
			if (occluderTested.find(poly) != occluderTested.end()) continue;

			//occluderTested.InsertSort(poly);
			occluderTested.insert(poly);

			//occluderNeighbours.InsertEnd(poly);
			occluderNeighbours.insert(poly);

			
			zREAL sumOccluderArea = GetOccluderAreaRec_UnionMyNew(poly, occluderNeighbours);
			
			if (sumOccluderArea > maxArea)
			{
				maxArea = sumOccluderArea;
			}
			if (occluderNeighbours.size() > maxNeighbourOccluders)
			{
				maxNeighbourOccluders = occluderNeighbours.size();
			}

			const zBOOL big = (sumOccluderArea > (300 * 300));

			if (!big)
			{
				/*
				for (int j = 0; j < occluderNeighbours.size(); j++)
				{
					occluderNeighbours[j]->SetOccluder(FALSE);
					//occluderNeighbours[j]->SetMaterial(mat);
					numOccluder--;
				}
				*/

				for (auto& entry : occluderNeighbours)
				{
					entry->SetOccluder(FALSE);
					numOccluder--;
				}
			}
			else
			{
			
				for (auto& entry : occluderNeighbours)
				{
					/*if (!occluderTested.IsInList(entry))
						occluderTested.InsertSort(entry);*/
					if (occluderTested.find(entry) == occluderTested.end())
					{
						occluderTested.insert(entry);
					}

				}
				/*
				for (int j = 0; j < occluderNeighbours.size(); j++)
				{

					if (!occluderTested.IsInList(occluderNeighbours[j]))
						occluderTested.InsertSort(occluderNeighbours[j]);
				}
				*/
				

			}

			

			//occluderNeighbours.DeleteList();
			occluderNeighbours.clear();
		}
		//occluderTested.DeleteList();
		occluderTested.clear();
		RX_End(4);

		cmd << "MarkOccluderPolys #2: " << RX_PerfString(4) << endl;


		zERR_MESSAGE(3, 0, "D: RBSP: ... numOccluder: " + zSTRING(numOccluder) + " of " + zSTRING(mesh->numPoly));
		zERR_MESSAGE(3, 0, "D: RBSP: ... maxOccluderSize: " + zSTRING(maxArea));
		zERR_MESSAGE(3, 0, "D: RBSP: ... maxOccluderNeighbours: " + zSTRING(maxNeighbourOccluders));
	}

#endif












	//int zCCBspNode::OutdoorKillRedundantLeafs () 
	//0x0053FF80 public: int __thiscall zCCBspNode::OutdoorKillRedundantLeafs(void)

	HOOK Ivk_zCCBspNode_OutdoorKillRedundantLeafs AS(&zCCBspNode::OutdoorKillRedundantLeafs, &zCCBspNode::OutdoorKillRedundantLeafs_Union);
	int zCCBspNode::OutdoorKillRedundantLeafs_Union()
	{
#ifdef GETPOLYS_TEST
		cmd << "GetPolyNeighbours: " << (GetPolyNeighbourPerfCounter / 1000.0f) << " ms" << endl;
#endif
		if (theApp.options.GetIntVal("bSkipPolysCut"))
		{
			return 0;
		}

		return THISCALL(Ivk_zCCBspNode_OutdoorKillRedundantLeafs)();
	}

}