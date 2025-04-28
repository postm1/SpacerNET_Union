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
	
	// unordered_set instead of old zCArray/zCArraySort, much faster
	HOOK Ivk_zCCBspTree_MarkOccluderPolys AS(&zCBspTree::MarkOccluderPolys, &zCBspTree::MarkOccluderPolys_Union);
	void zCBspTree::MarkOccluderPolys_Union()
	{
		//THISCALL(Ivk_zCCBspTree_MarkOccluderPolys)();

		zERR_MESSAGE(3, 0, "D: RBSP: Marking Occluder Polys...");
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

					if (polyBBox.IsIntersecting(poly2BBox) && poly->IsIntersectingProjection(poly2, zVEC3(0, 1, 0))) { occluder = FALSE; break; };

				};
			};

			poly->SetOccluder(occluder);
			numOccluder += int(occluder);
		};

		RX_End(4);

		cmd << RX_PerfString(4)
			<< " FirstPart: "
			<< endl;

		RX_Begin(4);


		//zCArray<zCPolygon*>		occluderNeighbours;
		std::unordered_set<zCPolygon*> occluderNeighbours;
		std::unordered_set<zCPolygon*> occluderTested;
		//zCArraySort<zCPolygon*> occluderTested;
		//occluderTested.SetCompare(Compare_Occluder);

		int maxNeighbourOccluders = 0;
		int maxArea = 0;

		cmd << "NumPolys: " << mesh->numPoly << endl;


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

		cmd << RX_PerfString(4)
			<< " SecondPart: "
			<< endl;


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