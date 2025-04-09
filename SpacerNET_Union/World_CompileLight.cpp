// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	//void zCMesh::CalcVertexNormals (zTCalcVertNormalMode mode, zCBspTree* bspTree)

	//HOOK Ivk_zCMesh_CalcVertexNormals AS(&zCMesh::CalcVertexNormals, &zCMesh::CalcVertexNormals_Union);
	void zCMesh::CalcVertexNormals_Union(zTCalcVertNormalMode mode, zCBspTree* bspTree)
	{
		zTBBox3D	polyBBox;
		zCPolygon** foundPolyList;
		int			foundPolyNum;

		RX_Begin(10);

		if (mode == zMSH_VERTNORMAL_MAT) //LightWorldStaticCompiled
		{
			cmd << "CalcVertexNormals Usual zMSH_VERTNORMAL_MAT" << endl;
			//THISCALL(Ivk_zCMesh_CalcVertexNormals)(mode, bspTree);
		}
		else
		{

			cmd << "CalcVertexNormals Start: " << (int)mode << endl;
			RX_Begin(11);
			ArraysToLists();
			UnshareFeatures();
			RX_End(11);

			cmd << "ArraysToList: " << RX_PerfString(11) << endl;

			if (mode == zMSH_VERTNORMAL_SMOOTH) // light
			{
				for (int i = 0; i < numPoly; i++) {
					zCPolygon* poly = polyList[i];

					if (bspTree)
					{
						polyBBox = poly->GetBBox3D();
						polyBBox.Scale(1.5F);
						const zREAL INC = zREAL(1.0F);
						if (polyBBox.mins[VX] == polyBBox.maxs[VX]) { polyBBox.mins[VX] -= INC; polyBBox.maxs[VX] += INC; };
						if (polyBBox.mins[VY] == polyBBox.maxs[VY]) { polyBBox.mins[VY] -= INC; polyBBox.maxs[VY] += INC; };
						if (polyBBox.mins[VZ] == polyBBox.maxs[VZ]) { polyBBox.mins[VZ] -= INC; polyBBox.maxs[VZ] += INC; };
					};
					for (int j = 0; j < poly->polyNumVert; j++)
					{
						zCVertex* vert = poly->vertex[j];
						zCVertFeature* feat = poly->feature[j];

						feat->vertNormal = poly->GetNormal();
						if (bspTree)
						{
							bspTree->bspRoot->CollectPolysInBBox3D(polyBBox, foundPolyList, foundPolyNum);
							// welches Poly teilt noch dieses aktuelle Vert ?
							for (int k = 0; k < foundPolyNum; k++)
							{
								zCPolygon* poly2 = foundPolyList[k];
								if (poly2->VertPartOfPoly(vert))
								{
									if (poly2 != poly)
										feat->vertNormal += poly2->GetNormal();
								};
							};
						}
						else
						{
							// welches Poly teilt noch dieses aktuelle Vert ?
							for (int k = 0; k < numPoly; k++)
							{
								zCPolygon* poly2 = polyList[k];
								if ((poly2->VertPartOfPoly(vert) && (i != k))) {
									feat->vertNormal += poly2->GetNormal();
								};
							};
						};
						feat->vertNormal.NormalizeSafe();
					};
				};
			}
			else if (mode == zMSH_VERTNORMAL_FACET) //3DS
			{
				cmd << "NumPoly: " << numPoly << endl;

				RX_Begin(12);

				/*
				for (int i = 0; i < numPoly; i++)
				{
					zCPolygon* poly = polyList[i];
					for (int j = 0; j < poly->polyNumVert; j++)
						poly->feature[j]->vertNormal = poly->GetNormal();
				};
				*/

				RX_End(12);

				cmd << "GetNormal: " << RX_PerfString(12) << endl;

			}; /* END FACET */
		}
		
		RX_End(10);


		cmd << "CalcVertexNormals: " << (int)mode << " " << RX_PerfString(10) << endl;

		
	}
}