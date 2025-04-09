// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
#if ENGINE == Engine_G2A
	//void zCMesh::CalcVertexNormals (zTCalcVertNormalMode mode, zCBspTree* bspTree)

	HOOK Ivk_zCMesh_CalcVertexNormals AS(&zCMesh::CalcVertexNormals, &zCMesh::CalcVertexNormals_Union);
	void zCMesh::CalcVertexNormals_Union(zTCalcVertNormalMode mode, zCBspTree* bspTree)
	{
		zTBBox3D	polyBBox;
		zCPolygon** foundPolyList;
		int			foundPolyNum;


		cmd << "CalcVertexNormals: " << (int)mode << " started..." << endl;

		ArraysToLists();
		UnshareFeatures();

		RX_Begin(10);
		//THISCALL(Ivk_zCMesh_CalcVertexNormals)(mode, bspTree);

		if (mode == zMSH_VERTNORMAL_MAT) //LightWorldStaticCompiled
		{
			for (int i = 0; i < numPoly; i++) {
				zCPolygon* poly = polyList[i];
				if (bspTree) {
					polyBBox = poly->GetBBox3D();
					polyBBox.Scale(1.5F);
					const zREAL INC = zREAL(1.0F);
					if (polyBBox.mins[VX] == polyBBox.maxs[VX]) { polyBBox.mins[VX] -= INC; polyBBox.maxs[VX] += INC; };
					if (polyBBox.mins[VZ] == polyBBox.maxs[VZ]) { polyBBox.mins[VZ] -= INC; polyBBox.maxs[VZ] += INC; };
				};
				for (int j = 0; j < poly->polyNumVert; j++)
				{
					zCVertex* vert = poly->vertex[j];
					zCVertFeature* feat = poly->feature[j];

					feat->vertNormal = poly->GetNormal();

					if (poly->material)
						if (poly->material->smooth || poly->GetSectorFlag())
						{
							if (bspTree)
							{
								bspTree->bspRoot->CollectPolysInBBox3D(polyBBox, foundPolyList, foundPolyNum);
								// welches Poly teilt noch dieses aktuelle Vert ?
								for (int k = 0; k < foundPolyNum; k++)
								{
									zCPolygon* poly2 = foundPolyList[k];

									if (poly2->material->smooth || poly2->GetSectorFlag())
									{
										if (poly2->VertPartOfPoly(vert))
										{
											if (poly2 == poly) continue;
											zREAL angle1, angle2;
											angle1 = poly->GetSectorFlag() ? 60 : poly->GetMaterial()->smoothAngle;
											angle2 = poly2->GetSectorFlag() ? 60 : poly2->GetMaterial()->smoothAngle;
											zREAL smoothRel = zMax(angle1, angle2);
											zREAL polyRel = Alg_Rad2Deg(Alg_AngleUnitRad(poly->GetNormal(), poly2->GetNormal()));
											if (smoothRel > polyRel)
												feat->vertNormal += poly2->GetNormal();
										};
									};
								};
							}
							else
							{
								// welches Poly teilt noch dieses aktuelle Vert ?
								for (int k = 0; k < numPoly; k++)
								{
									zCPolygon* poly2 = polyList[k];

									if (poly2->material == poly->material)
									{
										if (poly2->VertPartOfPoly(vert))
										{
											if (i == k) continue;
											// 2. Poly gefunden: ist dieses
											// a) auch ein Smooth-Material b) ist der Winkel < alpha° ?
											zREAL smoothRel = zMax(poly->GetMaterial()->smoothAngle, poly2->GetMaterial()->smoothAngle);
											zREAL polyRel = Alg_Rad2Deg(Alg_AngleUnitRad(poly->GetNormal(), poly2->GetNormal()));
											if (smoothRel > polyRel)
												feat->vertNormal += poly2->GetNormal();
										};
									};
								};
							};
							// hier kann theoret. eine Normale aus obiger Berechnung mit Laenge 0 ankommen
							// sollte man das anders behandeln .. ?
							zREAL normalLen = feat->vertNormal.Length();
							if (normalLen == 0)	feat->vertNormal = poly->GetNormal();
							else				feat->vertNormal /= normalLen;
						};
				};
			};
		}
		else if (mode == zMSH_VERTNORMAL_SMOOTH) // light
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
			
			for (int i = 0; i < numPoly; i++)
			{
				zCPolygon* poly = polyList[i];
				for (int j = 0; j < poly->polyNumVert; j++)
					poly->feature[j]->vertNormal = poly->GetNormal();
			};
			
		}; /* END FACET */
		
		
		RX_End(10);


		cmd << "CalcVertexNormals: " << (int)mode << " " << RX_PerfString(10) << endl;

		
	}

#endif
}