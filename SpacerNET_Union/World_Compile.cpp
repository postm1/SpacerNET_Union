// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	//void zCCBspTree::AddMesh (zCMesh* inMesh, const zMATRIX4& trafoObjToWorld) 
	//HOOK Ivk_zCCBspTree_AddMesh AS(&zCCBspTree::AddMesh, &zCCBspTree::AddMesh_Union);
	void zCCBspTree::AddMesh_Union(zCMesh* inMesh, const zMAT4& trafoObjToWorld)
	{
		RX_Begin(40);

		//THISCALL(Ivk_zCCBspTree_AddMesh)(inMesh, trafoObjToWorld);

		RX_End(40);

		cmd << "AddMesh_Union: " << RX_PerfString(50) << endl;
	}

	int& s_bAddVobsToMesh = *(int*)0x008D8870;

	HOOK Ivk_zCMesh_MergeMesh AS(&zCMesh::MergeMesh, &zCMesh::MergeMesh_Union);
	void zCMesh::MergeMesh_Union(zCMesh* mesh, const zMAT4& trafo)
	{
		// in some special mod we use old funciton or if we use new hashlist already
		if (s_bAddVobsToMesh || isNewMapActiveNow || trafo.GetTranslation().Length() > 0 || (mesh && mesh->numVert == 0))
		{
			cmd << "Called MergeMesh old function..." << endl;

			THISCALL(Ivk_zCMesh_MergeMesh)(mesh, trafo);
			return;
		}

		RX_Begin(40);

		ArraysToLists();
		UnshareFeatures();

		cmd << "MergeMesh_Union #1" << endl;
 
		int i = 0;
		AllocVerts(mesh->numVert);		// Ueberschaetzung
		AllocPolys(mesh->numPoly);


		// da die camera nicht performance kritisch ist, und höchste präzision vonnöten ist, 
		// wird die fpu auf 64 bit precision gesetzt
		zfpuControler->SaveCurrentControlWord();
		zfpuControler->SetPrecision_64();

		//if (!s_bAddVobsToMesh) S_InitVertexMergeCache(this);

		

		cmd << "MergeMesh_Union #2. Numvert: " << mesh->numVert 
			<< " NumPoly: " << numPoly
			<< " GetTranslation: " << trafo.GetTranslation().ToString()
			<< " GetRightVector: " << trafo.GetRightVector().ToString()
			<< " GetAtVector: " << trafo.GetAtVector().ToString()
			<< " GetUpVector: " << trafo.GetUpVector().ToString()

			<< endl;

		

		RX_Begin(41);

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

		RX_End(41);

		cmd << "MergeMesh_Union #4 newVertList: " << RX_PerfString(41) << endl;

		RX_Begin(41);

		for (i = 0; i < mesh->numPoly; i++) {

			zCPolygon* poly = AddPoly();
			(*poly) = *(mesh->polyList[i]);

			poly->vertex = 0;
			poly->polyNumVert = 0;

			poly->AllocVerts(mesh->polyList[i]->polyNumVert);
			mesh->polyList[i]->CopyValuesInto(poly);				// ACHTUNG, Material.AddRef impl. 

			// Alle Poly-Verts abgehen
			for (int j = 0; j < poly->polyNumVert; j++) 
			{
				zCVertex* vert = mesh->polyList[i]->vertex[j];
				poly->vertex[j] = vert;
				//			poly->vertex[j] = mesh->polyList[i]->vertex[j];
				//			zCVertex* vert	= poly->vertex[j];

							// Für jedes Poly-Vert passendes Vert aus Mesh-Liste finden

				
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
							cmd << (int)vert << " " << (int)mesh->vertList[k] << endl;
							poly->vertex[j] = newVert[k];
							break;
						};
					};
				}

			};

			// MUSS hier gemacht werden !!!!!!
			// Eine Normale ist definitiv eine *absolute* Angabe im jeweiligen Raum !
			poly->CalcNormal();
		};
		delete[] newVert;
		
		RX_End(41);
		cmd << "MergeMesh_Union #5 " << RX_PerfString(41) << endl;

		//if (!s_bAddVobsToMesh) S_DeleteVertexMergeCache();
		zfpuControler->RestoreSavedControlWord();

		RX_End(40);

		cmd << "MergeMesh_Union: " << RX_PerfString(40) << endl;
	}
}