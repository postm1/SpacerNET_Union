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
		if (s_bAddVobsToMesh || isNewMapActiveNow || trafo.GetTranslation().Length() > 0)
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

		if (!s_bAddVobsToMesh) S_InitVertexMergeCache(this);

		zCVertex** newVert = zNEW(zCVertex*)[mesh->numVert];

		cmd << "MergeMesh_Union #2. Numvert: " << mesh->numVert 
			<< " NumPoly: " << numPoly
			<< " GetTranslation: " << trafo.GetTranslation().ToString()
			<< " GetRightVector: " << trafo.GetRightVector().ToString()
			<< " GetAtVector: " << trafo.GetAtVector().ToString()
			<< " GetUpVector: " << trafo.GetUpVector().ToString()

			<< endl;

		

		RX_Begin(41);

		
		//mesh->BuildVertexMap();
		//cmd << "MergeMesh_Union #3" << endl;

		for (i = 0; i < mesh->numVert; i++) {
			zCVertex* vert = zNEW(zCVertex);
			// Vert transformieren & kopieren 
			*vert = (*(mesh->vertList[i]));
			vert->position = mesh->vertList[i]->position;
			newVert[i] = this->AddVertex(vert);
		};
		

		/*
		// Verts kopieren & einfügen
		for (i = 0; i < mesh->numVert; i++) 
		{
			const zVEC3& newPos = trafo * mesh->vertList[i]->position;

			zCVertex* vert = this->VertexInMesh(newPos);
			zCVertex* vert2 = NULL;// FindVertexFast(newPos);




			if (vert != vert2)
			{
				//cmd << "BAD POINTER: " << (int)vert << " " << (int)vert2 << endl;

				//cmd << "BAD POINTER: " << vert->position.ToString() << " " << vert2->position.ToString() << endl;
			}
			else
			{
				//cmd << "GOOD POINTER: " << (int)vert << " " << (int)vert2 << endl;
			}

			if (vert) newVert[i] = vert;
			else {
				vert = zNEW(zCVertex);
				// Vert transformieren & kopieren 
				*vert = (*(mesh->vertList[i]));
				vert->position = newPos;
				newVert[i] = this->AddVertex(vert);
				AddVertexToMap(vert);
			};
		};
		*/
		

		RX_End(41);

		cmd << "MergeMesh_Union #4 newVertList: " << RX_PerfString(41) << endl;

		RX_Begin(41);
		//ClearVertexMap();

		std::unordered_map<zCVertex*, zCVertex*> vertMap;

		vertMap.reserve(mesh->numVert);

		for (int k = 0; k < mesh->numVert; k++)
		{
			vertMap[mesh->vertList[k]] = newVert[k];
		}


		// Polys kopieren & einfügen
		for (i = 0; i < mesh->numPoly; i++) {

			zCPolygon* poly = AddPoly();
			(*poly) = *(mesh->polyList[i]);

			poly->vertex = 0;
			poly->polyNumVert = 0;

			poly->AllocVerts(mesh->polyList[i]->polyNumVert);
			mesh->polyList[i]->CopyValuesInto(poly);				// ACHTUNG, Material.AddRef impl. 

			
			// Alle Poly-Verts abgehen
			for (int j = 0; j < poly->polyNumVert; j++) {
				zCVertex* vert = mesh->polyList[i]->vertex[j];
				poly->vertex[j] = vert;

				
				auto it = vertMap.find(vert);
				if (it != vertMap.end())
				{
					poly->vertex[j] = it->second;
					break;
				}
			};

			// MUSS hier gemacht werden !!!!!!
			// Eine Normale ist definitiv eine *absolute* Angabe im jeweiligen Raum !
			poly->CalcNormal();
		};
		
		RX_End(41);
		cmd << "MergeMesh_Union #5 " << RX_PerfString(41) << endl;
		delete[] newVert;

		if (!s_bAddVobsToMesh) S_DeleteVertexMergeCache();
		zfpuControler->RestoreSavedControlWord();

		RX_End(40);

		cmd << "MergeMesh_Union: " << RX_PerfString(40) << endl;
	}
}