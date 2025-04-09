// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
#if ENGINE == Engine_G2A
	//void zCMesh::CalcVertexNormals (zTCalcVertNormalMode mode, zCBspTree* bspTree)

	HOOK Ivk_zCMesh_CalcVertexNormals AS(&zCMesh::CalcVertexNormals, &zCMesh::CalcVertexNormals_Union);
	void zCMesh::CalcVertexNormals_Union(zTCalcVertNormalMode mode, zCBspTree* bspTree)
	{
		if (!bspTree)
		{
			cmd << "CalcVertexNormals: original" << endl;

			RX_Begin(10);
			THISCALL(Ivk_zCMesh_CalcVertexNormals)(mode, bspTree);

			RX_End(10);


			cmd << "CalcVertexNormals: " << (int)mode << " " << RX_PerfString(10) << endl;

			return;
		}

		zTBBox3D	polyBBox;
		zCPolygon** foundPolyList;
		int			foundPolyNum;


		cmd << "CalcVertexNormalsNew: Mode: " << (int)mode << " BSP: " << (int)bspTree << " started..." << endl;

		ArraysToLists();
		UnshareFeatures();

		RX_Begin(10);
		//

		if (mode == zMSH_VERTNORMAL_MAT) //LightWorldStaticCompiled
		{

			std::unordered_map<zCVertex*, std::vector<zCPolygon*>> vertexToPolygonsMap;

			for (int i = 0; i < numPoly; i++)
			{
				zCPolygon* poly = polyList[i];

				for (int j = 0; j < poly->polyNumVert; j++)
				{
					vertexToPolygonsMap[poly->vertex[j]].push_back(poly);
				}
			}

			for (int i = 0; i < numPoly; i++) {
				zCPolygon* poly = polyList[i];
				if (bspTree) {
					polyBBox = poly->GetBBox3D();
					polyBBox.Scale(1.5F);
					const zREAL INC = zREAL(1.0F);
					if (polyBBox.mins[VX] == polyBBox.maxs[VX]) { polyBBox.mins[VX] -= INC; polyBBox.maxs[VX] += INC; };
					if (polyBBox.mins[VZ] == polyBBox.maxs[VZ]) { polyBBox.mins[VZ] -= INC; polyBBox.maxs[VZ] += INC; };

					bspTree->bspRoot->CollectPolysInBBox3D(polyBBox, foundPolyList, foundPolyNum);
				};
				
				for (int j = 0; j < poly->polyNumVert; j++) {
					zCVertex* vert = poly->vertex[j];
					zCVertFeature* feat = poly->feature[j];

					feat->vertNormal = poly->GetNormal();

					if (poly->material && (poly->material->smooth || poly->GetSectorFlag())) {
						// Используем карту для быстрого поиска полигонов, содержащих эту вершину
						auto it = vertexToPolygonsMap.find(vert);
						if (it != vertexToPolygonsMap.end()) {
							const auto& sharingPolys = it->second;

							for (zCPolygon* poly2 : sharingPolys) {
								if (poly2 == poly) continue;

								if (poly2->material && (poly2->material->smooth || poly2->GetSectorFlag())) {
									zREAL angle1 = poly->GetSectorFlag() ? 60 : poly->GetMaterial()->smoothAngle;
									zREAL angle2 = poly2->GetSectorFlag() ? 60 : poly2->GetMaterial()->smoothAngle;
									zREAL smoothRel = zMax(angle1, angle2);
									zREAL polyRel = Alg_Rad2Deg(Alg_AngleUnitRad(poly->GetNormal(), poly2->GetNormal()));

									if (smoothRel > polyRel) {
										feat->vertNormal += poly2->GetNormal();
									}
								}
							}
						}

						zREAL normalLen = feat->vertNormal.Length();
						if (normalLen == 0) {
							feat->vertNormal = poly->GetNormal();
						}
						else {
							feat->vertNormal /= normalLen;
						}
					}
				}
			};
		}
		else if (mode == zMSH_VERTNORMAL_SMOOTH) // light
		{
			// Vertex map
			std::unordered_map<zCVertex*, std::vector<zCPolygon*>> vertexToPolygonsMap;

			for (int i = 0; i < numPoly; i++) 
			{
				zCPolygon* poly = polyList[i];

				for (int j = 0; j < poly->polyNumVert; j++) 
				{
					vertexToPolygonsMap[poly->vertex[j]].push_back(poly);
				}
			}

			for (int i = 0; i < numPoly; i++) 
			{
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

				bspTree->bspRoot->CollectPolysInBBox3D(polyBBox, foundPolyList, foundPolyNum);

				int saveNum = foundPolyNum;
				//cmd << "CommonResult: " << polyBBox.mins.ToString() + "/" + polyBBox.maxs.ToString() << " foundPolyNum: " << foundPolyNum << endl;

				for (int j = 0; j < poly->polyNumVert; j++)
				{
					zCVertex* vert = poly->vertex[j];
					zCVertFeature* feat = poly->feature[j];

					feat->vertNormal = poly->GetNormal();
					if (bspTree)
					{
						
						//bspTree->bspRoot->CollectPolysInBBox3D(polyBBox, foundPolyList, foundPolyNum);

						//cmd << "\tPartResult: [" << j << "] " << " foundPolyNum: " << foundPolyNum << endl;

						/*if (foundPolyNum != saveNum)
						{
							cmd << "!!!!!!!!!!!!! " << foundPolyNum << " / " << saveNum << endl;
						}*/
						

						// welches Poly teilt noch dieses aktuelle Vert ?
						for (int k = 0; k < foundPolyNum; k++)
						{
							zCPolygon* poly2 = foundPolyList[k];

							//some debug test code
							//bool flagFound = false;
							//bool flagFound2 = false;

							/*
							if (poly2->VertPartOfPoly(vert))
							{
								if (poly2 != poly)
								{
									//feat->vertNormal += poly2->GetNormal();
									flagFound = true;
								}
							};
							*/
							

							if (poly2 != poly && std::find(poly2->vertex, poly2->vertex + poly2->polyNumVert, vert) != poly2->vertex + poly2->polyNumVert)
							{
								feat->vertNormal += poly2->GetNormal();
								//flagFound2 = true;
							}

							/*if (flagFound != flagFound2)
							{
								cmd << "flagFound fail! " << flagFound << "/" << flagFound2 << endl;
							}*/
						};
					}

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
			
		};
		
		
		RX_End(10);


		cmd << "CalcVertexNormalsNew: " << (int)mode << " " << RX_PerfString(10) << " finished" << endl;

		
	}

	static zCArray<zCPatchMap*>& patchMapList = *reinterpret_cast<zCArray<zCPatchMap*>*>(0x009A442C);
	//zCList<zCVob>** lightVobList = reinterpret_cast<zCList<zCVob>**>(0x009A4440);


	inline void zClamp01(float& f) {
		if ((*(zDWORD*)&f) >= (zDWORD)0x80000000)	f = 0.f; else
			if ((*(zDWORD*)&f) > (zDWORD)0x3f800000)	f = 1.0F;
	}

	static zCList<zCVob>*& GetLightVobList() {
		return *reinterpret_cast<zCList<zCVob>**>(0x009A4440);
	}

	//void zCWorld::LightPatchMap (zCPatchMap *patchMap) 
	//HOOK Ivk_zCWorld_LightPatchMap AS(&zCWorld::LightPatchMap, &zCWorld::LightPatchMap_Union);
	void zCWorld::LightPatchMap_Union(zCPatchMap* patchMap)
	{
		zCList<zCVob>*& lightVobList = GetLightVobList();

		zCList<zCVob>* item = lightVobList->GetNextInList();
		zCPolygon* poly = patchMap->surface[0];

		cmd << "LightPatchMap_Union: " << lightVobList->GetNumInList() << endl;

		while (item)
		{
			// aktuelles Mesh mit jedem Licht aus Liste beleuchten
			zCVobLight* light = ((zCVobLight*)item->GetData());

			// Trivial rejection:
			// Nur Lichter beruecksichtigen, die die minSurfacPlane schneiden und die BBox der Surface beruehren
			zPOINT3 lightPos = light->GetPositionWorld();
			//		zREAL	dist0		= zAbsApprox(patchMap->lightRejectPlane0.GetDistanceToPlane (lightPos));
			//		zREAL	dist1		= zAbsApprox(patchMap->lightRejectPlane1.GetDistanceToPlane (lightPos));
			zTBSphere3D lightSphere;
			lightSphere.center = lightPos;
			lightSphere.radius = light->GetRange();
			if (
				//			((dist0<=light->GetRange()) ||
				//			 (dist1<=light->GetRange())) && 
				(patchMap->bbox3D.IsIntersecting(lightSphere))
				)
			{
				//
				zCOLOR	col = light->GetColor();
				zVEC3	lightColor = col.GetVEC3();
				zREAL	range2 = light->GetRange() * light->GetRange();
				zVALUE	rangeInv = light->GetRangeInv();
				zREAL	spotDotMax = cosf((light->lightData.GetSpotConeAngle() / 180.0F) * 3.14159F);

				for (int i = 0; i < patchMap->patchList.GetNumInList(); i++)
				{
					zCPatch* patch = patchMap->patchList[i];
					zVEC3	view = (patch->centerLight - lightPos);
					zREAL	dist = (view).Length_Sqr();
					if (dist <= range2)
					{
						zREAL dot, i = 0;
						dist = sqrt(dist);
						view /= dist;
						dot = -(view.Dot(patch->normal));

						if (dot < 0) continue;

						if (light->GetLightType() == zVOBLIGHT_TYPE_SPOT)
						{
							zREAL spotDot = view.Dot(light->GetAtVectorWorld());
							if (spotDot <= spotDotMax) continue;
						};

						/*					// linear falloff
											dist  = 1-(dist * rangeInv);
											zClamp01 (dist);
											i	  = dot;
											i	 *= dist;
						*/
						// quadratic falloff
						// (fuer einen helleren Gesamteindruck)
						dist = (dist * rangeInv);
						dist *= dist;
						dist = 1 - dist;
						zClamp01(dist);
						i = dot;
						i *= dist;

						// Raytrace Occlusion Test
						zVEC3		patchColor = lightColor;
						zVEC3		inters;
						zCPolygon* hitPoly = 0;
						//					bspTree.TraceRay (lightPos, patch->center-patchMap->poly->polyPlane.normal, TRUE, FALSE, TRUE, inters, hitPoly, 0); 
											// ignoreTranspPoly ? Farbaenderung bei Wasserdurchdringung ??
						//					bspTree.TraceRay	(lightPos, patch->center, TRUE, TRUE, TRUE, inters, hitPoly, 0); 
						/*					if ((bspTree.TraceRay (lightPos, patch->center, TRUE, FALSE, TRUE, inters, hitPoly, 0) &&
												(hitPoly) &&
												(hitPoly->GetMaterial()->GetAlpha()!=255)))
											{
												patchColor	= 0.5F*patchColor + 0.5F*zVEC3(0,0,GetColorIntensity(patchColor));
												bspTree.TraceRay (lightPos, patch->center, TRUE, TRUE, TRUE, inters, hitPoly, 0);
											};*/
											//					if (LightingTestRay (lightPos, patch->center, inters, hitPoly)) 
																// FIXME !!!   SURFACE !!!
																// nichts getroffen => ok, der Licht Strahl kommt durch !
																// etwas  getroffen => Strahl kommt nur durch, wenn das getroffene Poly eines der Surfae ist !
											/*
																zVEC3 ray = (patch->center-lightPos)*0.5F;
																zBOOL res = LightingTestRay (lightPos, patch->center + ray, inters, hitPoly);
											//					zBOOL res = LightingTestRay (lightPos, patch->center, inters, hitPoly);
																if (res) {
																	for (int i=0; i<patchMap->surface.GetNumInList(); i++)
																		if (patchMap->surface[i]==hitPoly) { res = FALSE; break; };
																	if ((inters-patch->center).Length2()<80.0F)	res = FALSE;
																};
																if (!res)
											*/
											// Vobs werfen Schatten
						const zVEC3& rayLightToPatch = patch->centerLight - lightPos;
						if (TraceRayNearestHit(lightPos, rayLightToPatch, light, zTRACERAY_STAT_IGNORE)) {
							if (traceRayReport.foundVob)
								if (traceRayReport.foundVob->staticVob)
									goto leaveThis;
						};

						{

							// Der ZielOrt ist ein wenig von dem Poly weggerueckt.
							// Beachte: Die BSP-TraceRay Routine verlaengt den Ray minimal (ca. 1.001), wodurch ein hier
							// weggerueckter EndPunkt trotzdem 'merkwuerdigerweise' das Ziel treffen wuerde.
							// (=> zCBspTree::CheckRayAgainstPolys..())
							zBOOL rayOccluded = bspTree.TraceRay(lightPos,
								patch->center + poly->GetNormal() * 11,	// war: 9
								zTRACERAY_STAT_POLY | zTRACERAY_POLY_IGNORE_TRANSP,
								inters,
								hitPoly,
								0);

							if (!rayOccluded)
							{
								//						if ((hitPoly==poly) || ((inters-patch->center).Length2()<80.0F))	// 9cm
								{
#ifdef zLIGHTMAP_COLOR_MARK
									patch->radiosity = zVEC3(0, 255, 0);
#else
									patch->radiosity += patchColor * i;
									//								patch->radiosity= zVEC3(255,0,0);
#endif
									patch->radToShoot += patchColor * i;
									patchMap->hit = TRUE;
								};
							};
						};

						goto leaveThis;
					leaveThis:;
					};
				};
			};
			item = item->GetNextInList();
		};

		//THISCALL(Ivk_zCWorld_LightPatchMap)(patchMap);
	}


	//void zCWorld::GenerateSurfaces (const zBOOL doRaytracing, zTBBox3D *updateBBox3D)
	//HOOK Ivk_zCWorld_GenerateSurfaces AS(&zCWorld::GenerateSurfaces, &zCWorld::GenerateSurfaces_Union);
	void zCWorld::GenerateSurfaces_Union(const zBOOL doRaytracing, zTBBox3D* updateBBox3D)
	{
		int						numSurfaces = 0;
		int						i;
		zCMesh* mesh = bspTree.mesh;
		zCArray<zCPolygon*>		polyList(mesh->numPoly);
		zCArray<zCPolygon*>		surface;

	
		for (i = 0; i < mesh->numPoly; i++) {
			zCPolygon* poly = mesh->Poly(i);
			poly->flags.mustRelight = TRUE;	
		
			if (poly->GetMaterial()->dontUseLightmaps) continue;
			if (poly->GetMaterial()->GetTexture())
			{
				poly->GetMaterial()->GetTexture()->CacheIn(-1);		
				if (poly->GetMaterial()->GetTexture()->HasAlpha())	
					continue;
			}
			if (GetBspTree()->bspTreeMode == zBSP_MODE_OUTDOOR) {
				if (!poly->GetSectorFlag()) continue;
				if (poly->IsPortal())		continue;
			};

			if (updateBBox3D) {
				if (!poly->GetBBox3D().IsIntersecting(*updateBBox3D)) continue;
			};
			poly->flags.mustRelight = FALSE;					
			polyList.Insert(poly);
		};


		

		zREAL numPolyTotal = polyList.GetNum();
		const zREAL		EPSILON_PLANE_NORMAL = 0.70F;

		cmd << "GenerateSurfaces_Union: " << numPolyTotal << endl;

		

		while (polyList.GetNum() > 0)
		{
			surface.EmptyList();
			surface.Insert(polyList[0]);
			polyList.RemoveIndex(0);

			RX_Begin(11);

			{
				for (int l = 0; l < surface.GetNumInList(); l++) {
					zCPolygon* poly = surface[l];
					zCPolygon** foundPolyList = 0;
					int			foundPolyNum = 0;
					zTBBox3D	bbox3D;
					bbox3D = poly->GetBBox3D();
					bbox3D.Scale(zREAL(1.01F));
					GetBspTree()->CollectPolysInBBox3D(bbox3D, foundPolyList, foundPolyNum);

					for (int j = 0; j < foundPolyNum; j++) {
						zCPolygon* poly2 = foundPolyList[j];

						if (poly2->flags.mustRelight)	continue;
						if (poly == poly2) 			continue;

						
						const zTPlane& p1 = surface[0]->GetPlane();
						const zTPlane& p2 = poly2->GetPlane();
						if (p1.normal.Dot(p2.normal) >= EPSILON_PLANE_NORMAL)
						{
							for (int k = 0; k < poly->polyNumVert; k++)
							{
								if (poly2->VertPartOfPoly(poly->GetVertexPtr(k)))
								{
									int polyListIndex = polyList.Search(poly2);
									if (polyListIndex >= 0)
									{
										zBOOL intersectingProjection = FALSE;
										for (int m = 0; m < surface.GetNum(); m++)
										{
											if (poly2->IsIntersectingProjection(surface[m], surface[0]->GetNormal()))
											{
												intersectingProjection = TRUE;
												break;
											};
										};
										if (!intersectingProjection)
										{
											zTBBox2D	lmBox;
											int			realDim[2];
											surface.InsertEnd(poly2);
											if (!GetSurfaceLightmapBBox2D(surface, lmBox, realDim)) {
												surface.RemoveIndex(surface.GetNumInList() - 1);
											}
											else {
												poly2->flags.mustRelight = TRUE;
												polyList.RemoveIndex(polyListIndex);
												foundPolyList[j] = foundPolyList[foundPolyNum - 1];
												foundPolyNum--;
												j--;
												break;
											};
										};
									};
								};
							};
						};
					};
				};
			};
			RX_End(11);

			//zERR_MESSAGE(3, 0, "D: ... Light: " + RX_PerfString(11));


			RX_Begin(12);


			
			zCPatchMap* patchMap = 0;
			int currentPatchDim[2];
			if (doRaytracing)
			{
				RX_Begin(13);
				patchMap = GeneratePatchMapFromSurface(surface);
				RX_End(13);
				RX_Begin(14);
				LightPatchMap(patchMap);
				RX_End(14);
				RX_Begin(15);
				GenerateLightmapFromPatchMap(patchMap);
				RX_End(15);
				currentPatchDim[0] = patchMap->xdim;
				currentPatchDim[1] = patchMap->ydim;
				delete patchMap;

				
				
			}
			else
			{
				cmd << "!!!!!!!" << endl;
				patchMap = GeneratePatchMapFromSurface(surface);
				patchMapList.Insert(patchMap);
				LightPatchMap(patchMap);
			};

			RX_End(12);

			const int MIN_TIME_CHECK = 20;

			if ((perf[13] / 1000.0f) >= MIN_TIME_CHECK || (perf[14] / 1000.0f) >= MIN_TIME_CHECK || (perf[15] / 1000.0f) >= MIN_TIME_CHECK || (perf[11] / 1000.0f) >= MIN_TIME_CHECK)
			{
				cmd << RX_PerfString(13) << " | " << RX_PerfString(14) << " | " << RX_PerfString(15) << " | -> " << RX_PerfString(12) << " | Prepare: " << RX_PerfString(11) << endl;
			}

			zERR_MESSAGE(3, 0, "D: ... Generate: " + RX_PerfString(12));

			numSurfaces++;
			if ((numSurfaces & 7) == 0)
			{
				zREAL perc = (zREAL(1) - (zREAL(polyList.GetNum()) / numPolyTotal)) * zREAL(100);
				zERR_MESSAGE(3, 0, "D: ... working, numSurfaces: " + zSTRING(numSurfaces) + ", numPolys: " + zSTRING(surface.GetNum()) + ", dim: " + zSTRING(currentPatchDim[0]) + "x" + zSTRING(currentPatchDim[1]) + " (" + zSTRING(perc, 3) + "%) ...");
			};
		};

		mesh->CombineLightmaps();

		zerr->Message("D: WORLD: LM: numPolys: " + zSTRING(mesh->numPoly) + ", numSurfaces: " + zSTRING(numSurfaces));
	}


#endif
}