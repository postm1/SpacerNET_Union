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
			//cmd << "Called MergeMesh old function..." << endl;

			THISCALL(Ivk_zCMesh_MergeMesh)(mesh, trafo);
			return;
		}

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
	




	static int Compare_Occluder(const void* arg1, const void* arg2)
	{
		int poly1 = int(*((zCPolygon**)arg1));
		int poly2 = int(*((zCPolygon**)arg2));

		return (poly1 - poly2);
	};

	int staticTime = 0;
	//0x00534210 private: float __thiscall zCBspTree::GetOccluderAreaRec(class zCPolygon const *,class zCArray<class zCPolygon *> &)
	/*
	HOOK Ivk_zCCBspTree_GetOccluderAreaRec AS(&zCBspTree::GetOccluderAreaRec, &zCBspTree::GetOccluderAreaRec_Union);
	float zCBspTree::GetOccluderAreaRec_Union(const zCPolygon* rootPoly, zCArray<zCPolygon*>& nearOccluderPolys)
	{
		RX_Begin(5);
		auto result = THISCALL(Ivk_zCCBspTree_GetOccluderAreaRec)(rootPoly, nearOccluderPolys);
		RX_End(5);

		staticTime += perf[5];

		return result;
	}
	*/

	//HOOK Ivk_zCCBspTree_MarkOccluderPolys AS(&zCBspTree::MarkOccluderPolys, &zCBspTree::MarkOccluderPolys_Union);
	void zCBspTree::MarkOccluderPolys_Union()
	{
		//THISCALL(Ivk_zCCBspTree_MarkOccluderPolys)();

		/*cmd << (staticTime / 1000.0f)
			<< " 5#: "
			<< endl;*/

		return;

		zERR_MESSAGE(3, 0, "D: RBSP: Marking Occluder Polys...");
		RX_Begin(4);

		int numOccluder = 0;
		for (int i = 0; i < mesh->numPoly; i++) 
		{
			zCPolygon* poly = mesh->SharePoly(i);

			// Ghost-Occluder ?
			if (poly->GetGhostOccluder())
			{
				poly->SetOccluder(TRUE);
				continue;
			};

			// [EDENFELD] 1.09 Alle Polys, die ein Material mit noCollDet Flag 
			// auf TRUE gesetzt haben, sind _immer_ nicht-occluder
			// ausserdem: neue Material Eigenschaft "forceOccluder" eingefьhrt,
			// die alle Polys, die dieses Material benutzen automatisch zum Occluder 
			// werden lдsst
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
					//continue;
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

			// Alle Polys aus der nahen Umgebung einsammeln
			zTBBox3D searchBox = polyBBox;
			const zREAL INC = zREAL(1.0F);
			if (searchBox.mins[VX] == searchBox.maxs[VX]) { searchBox.mins[VX] -= INC; searchBox.maxs[VX] += INC; };
			if (searchBox.mins[VZ] == searchBox.maxs[VZ]) { searchBox.mins[VZ] -= INC; searchBox.maxs[VZ] += INC; };
			searchBox.Scale(zREAL(1.2F));		// scale x/z
			searchBox.mins[VY] = -999999;
			searchBox.maxs[VY] = +999999;

			zBOOL		occluder = TRUE;
			{
				bspRoot->CollectPolysInBBox3D(searchBox, foundPolyList, foundPolyNum);
				for (int j = 0; j < foundPolyNum; j++) {
					zCPolygon* poly2 = foundPolyList[j];
					if (poly == poly2) continue;

					// Sector-Poly Handling

					if (poly2->GetSectorFlag()) continue;

					if (poly2->polyPlane.normal[VY] < zREAL(0.0001F)) continue;

					// Wasser ?
					if (poly2->GetMaterial()->GetMatGroup() == zMAT_GROUP_WATER) continue;

					// haben beide Polys fast identische Normalen ?
					if (poly->polyPlane.normal.IsEqualEps(poly2->polyPlane.normal)) continue;

					// Das zweite Poly darf nicht ueber dem ersten liegen
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
			<< " 1#: "
			<< endl;

		RX_Begin(4);
		// zweiter pass: alle occluder rausschmeissen, die ineffizient sind.
		// dazu gehцren: 
		// a) occluder ohne occluder nachbarn die sehr klein sind
		// b) occluder polys mit occluder nachbarn, dessen flдchen-summe zu klein ist
		zCArray<zCPolygon*>		occluderNeighbours;
		zCArraySort<zCPolygon*> occluderTested;
		occluderTested.SetCompare(Compare_Occluder);

		//std::unordered_set<zCPolygon*> occluderTested;

		int maxNeighbourOccluders = 0;
		int maxArea = 0;
		int staticTime = 0;

		//zCMaterial *mat = zNEW(zCMaterial("Occluder_Poly_Mat"));  
		//mat->SetColor (0,255,0);

		for (int i = 0; i < mesh->numPoly; i++)
		{
			zCPolygon* poly = mesh->SharePoly(i);

			static int count = 0;
			if ((count++ == 10000))
			{
				count = 0;
				zERR_MESSAGE(5, 0, "C: unmarking inefficient occluders, still working: " + zSTRING((float(i) / float(mesh->numPoly)) * 100));
				//cmd << occluderTested.size() << "|" << occluderNeighbours.GetNumInList() << endl;
			}

			if (!poly->IsOccluder())				 continue;
			if (poly->GetGhostOccluder())			 continue;
			if (occluderTested.IsInList(poly))		 continue;
			//if (occluderTested.find(poly) != occluderTested.end()) continue;

			occluderTested.InsertSort(poly);
			//occluderTested.insert(poly);
			occluderNeighbours.InsertEnd(poly);

			//0x00534210 private: float __thiscall zCBspTree::GetOccluderAreaRec(class zCPolygon const *,class zCArray<class zCPolygon *> &)
			
			RX_Begin(5);
			zREAL sumOccluderArea = GetOccluderAreaRec(poly, occluderNeighbours);
			RX_End(5);

			staticTime += perf[5];
			
			//cmd << "occluderNeighbours: " << occluderNeighbours.GetNumInList() << endl;

			
			if (sumOccluderArea > maxArea)
			{
				maxArea = sumOccluderArea;
			}
			if (occluderNeighbours.GetNum() > maxNeighbourOccluders)
			{
				maxNeighbourOccluders = occluderNeighbours.GetNum();
			}

			const zBOOL big = (sumOccluderArea > (300 * 300));

			if (!big)
			{
				for (int j = 0; j < occluderNeighbours.GetNum(); j++)
				{
					occluderNeighbours[j]->SetOccluder(FALSE);
					//occluderNeighbours[j]->SetMaterial(mat);
					numOccluder--;
				}
			}
			else
			{
			
				for (int j = 0; j < occluderNeighbours.GetNum(); j++)
				{
					/*if (occluderTested.find(occluderNeighbours[j]) == occluderTested.end())
					{
						occluderTested.insert(occluderNeighbours[j]);
					}*/

					if (!occluderTested.IsInList(occluderNeighbours[j]))
						occluderTested.InsertSort(occluderNeighbours[j]);
				}

				

			}

			

			occluderNeighbours.DeleteList();

		}
		occluderTested.DeleteList();

		RX_End(4);

		cmd << RX_PerfString(4)
			<< " 2#: "
			<< endl;


		cmd << (staticTime / 1000.0f)
			<< " 5#: "
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