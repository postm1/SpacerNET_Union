// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

#if ENGINE == Engine_G2A
	int& s_bAddVobsToMesh = *(int*)0x008D8870;

	HOOK Ivk_zCMesh_MergeMesh AS(&zCMesh::MergeMesh, &zCMesh::MergeMesh_Union);
	void zCMesh::MergeMesh_Union(zCMesh* mesh, const zMAT4& trafo)
	{
		// in some special cases we use old funciton
		if (s_bAddVobsToMesh || isNewMapActiveNow || trafo.GetTranslation().Length() > 0 || (mesh && mesh->numVert == 0))
		{
			cmd << "Called MergeMesh old function..." << endl;

			THISCALL(Ivk_zCMesh_MergeMesh)(mesh, trafo);
			return;
		}

		RX_Begin(40);

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

		RX_End(40);

		cmd << "MergeMesh_Union: " << RX_PerfString(40) << endl;
	}
#endif
}