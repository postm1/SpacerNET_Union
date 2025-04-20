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

		RX_Begin(3);

#ifdef GETPOLYS_TEST
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