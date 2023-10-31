// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	

	void GrassPlacer::GeneratePointsCircle(zVEC3 startPos, int amount, int radius)
	{
		pointsVec.DeleteList();

		for (int i = 0; i <= amount; ++i)
		{
			zVEC3 newPoint;
			newPoint.n[0] = startPos.n[0] + GetRandValNegative(-radius, radius);
			newPoint.n[1] = startPos.n[1];
			newPoint.n[2] = startPos.n[2] + GetRandValNegative(-radius, radius);


			if (startPos.Distance(newPoint) <= radius)
			{
				pointsVec.Insert(newPoint);
			}
		}
	}

	void GrassPlacer::DoPlace()
	{
		if (zinput->GetMouseButtonPressedLeft())
		{

			if (!theApp.TryPickMouse())
			{
				return;
			}

			if (mainTimer[0u].Await(30))
			{
				zCCamera* cam = ogame->GetCamera();
				zCVob* camVob = ogame->GetCamera()->connectedVob;

				float ax = theApp.pickTryEntry.ax;
				float ay = theApp.pickTryEntry.ay;

				zVEC3 ray00, ray, p;
				cam->camMatrixInv.GetTranslation(ray00);
				p.n[VZ] = 1;
				cam->BackProject(ax, ay, p);
				p = cam->camMatrixInv * p;
				ray = p - ray00;

				ray = ray.Normalize();

				ogame->GetWorld()->PickScene(*ogame->GetCamera(), ax, ay, -1);



				if (ogame->GetWorld()->TraceRayFirstHit(cam->GetVob()->GetPositionWorld(), ray * 25000, (zCVob*)NULL, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_VOB_IGNORE)) {

					if (ogame->GetWorld()->traceRayReport.foundPoly) {

						auto poly = ogame->GetWorld()->traceRayReport.foundPoly;
						auto posToPlace = ogame->GetWorld()->traceRayReport.foundIntersection;


						if (poly)
						{
							//GeneratePointsCircle(posToPlace, 10, 2000);

							int bBoxSize = 5000;
							zCArray<zCVob*> baseVobList;
							zCArray<zCVob*> resVobList;
							zTBBox3D box;

							box.maxs = posToPlace + zVEC3(bBoxSize, bBoxSize, bBoxSize);
							box.mins = posToPlace - zVEC3(bBoxSize, bBoxSize, bBoxSize);
							ogame->GetWorld()->CollectVobsInBBox3D(baseVobList, box);


							int minDist = theApp.options.GetIntVal("grassMinDist");
							zSTRING modelName = theApp.options.GetVal("grassModelName");
							int offsetVert = theApp.options.GetIntVal("grassVertOffset");
							int grassToolRemove = theApp.options.GetIntVal("grassToolRemove");
							int isItem = theApp.options.GetIntVal("grassToolIsItem");

							int grassToolClearMouse = theApp.options.GetIntVal("grassToolClearMouse");
							int grassToolDynColl = theApp.options.GetIntVal("grassToolDynColl");
							int grassToolRotRandAngle = theApp.options.GetIntVal("grassToolRotRandAngle");
							int grassToolSetNormal = theApp.options.GetIntVal("grassToolSetNormal");


							if (modelName.Length() == 0) {
								return;
							}

							if (grassToolClearMouse) zinput->ClearLeftMouse();

							for (int i = 0; i < baseVobList.GetNumInList(); i++) {


								zCVob* vob = baseVobList[i];

								if (vob
									&& vob != camVob
									&& !dynamic_cast<zCVobWaypoint*>(vob)
									&& !dynamic_cast<zCVobSpot*>(vob)
									&& !dynamic_cast<zCVobLight*>(vob)
									&& !dynamic_cast<zCVobLevelCompo*>(vob)
									&& !dynamic_cast<zCZone*>(vob)
									&& vob->GetVisual()
									&& vob->GetVisual()->GetVisualName() == modelName
									&& vob != theApp.currentVobRender
									&& vob != pfxManager.testVob
									)
								{

									resVobList.InsertEnd(vob);
								}
							}


							for (int i = 0; i < baseVobList.GetNumInList(); i++) {


								zCVob* vob = baseVobList[i];

								if (vob
									&& vob != camVob
									&& !dynamic_cast<zCVobWaypoint*>(vob)
									&& !dynamic_cast<zCVobSpot*>(vob)
									&& !dynamic_cast<zCVobLight*>(vob)
									&& !dynamic_cast<zCVobLevelCompo*>(vob)
									&& !dynamic_cast<zCZone*>(vob)
									&& vob->GetVisual()
									&& vob != theApp.currentVobRender
									&& vob != pfxManager.testVob
									)
								{
									auto item = vob->CastTo<oCItem>();

									if (item && item->GetInstanceName() == modelName)
									{
										resVobList.InsertEnd(vob);
									}


								}
							}


							if (grassToolRemove)
							{

								for (int i = 0; i < resVobList.GetNumInList(); i++)
								{
									zCVob* vob = resVobList[i];

									auto dist = vob->GetPositionWorld().Distance(posToPlace);

									if (dist <= minDist)
									{
										theApp.RemoveVob(vob);
									}
								}

								resVobList.DeleteList();

								return;
							}

							for (int i = 0; i < resVobList.GetNumInList(); i++)
							{
								zCVob* vob = resVobList[i];

								auto dist = vob->GetPositionWorld().Distance(posToPlace);

								if (dist <= minDist)
								{
									print.PrintRed("No place to put vob");
									return;
								}
							}

							/*
							for (int i = 0; i < pointsVec.GetNumInList(); i++)
							{
								auto posCurrent = pointsVec.GetSafe(i);

								auto newVob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance("zCVob"));
	

								newVob->SetVobName("");


								newVob->SetCollDetStat(FALSE);
								newVob->SetCollDetDyn(FALSE);
								newVob->SetVisual(modelName);
								newVob->SetPositionWorld(posCurrent + zVEC3(0, offsetVert, 0));
								InsertIntoWorld(newVob, NULL, false);

								newVob->RotateLocalY(GetRandVal(0, 360));

							}
							*/

							zCVob* newVob = NULL;

							if (isItem)
							{
								newVob = theApp.CreateItem(modelName.Upper());
							}
							else
							{
								newVob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance("zCVob"));
								newVob->SetVobName("");


								newVob->SetCollDetStat(FALSE);
								newVob->SetCollDetDyn(FALSE);
								newVob->SetVisual(modelName);
							}


							if (!newVob) return;



							auto bboxCenter = newVob->bbox3D.GetCenter();
							auto point = newVob->trafoObjToWorld.GetTranslation() - newVob->bbox3D.GetCenterFloor();

							if (!isItem)
							{
								newVob->SetCollDetStat(FALSE);
								newVob->SetCollDetDyn(FALSE);
							}

							newVob->SetPositionWorld(posToPlace + zVEC3(0, offsetVert, 0) + point);

							if (!isItem)
							{
								InsertIntoWorld(newVob, NULL, false);
							}


							poly->CalcNormal();

							zVEC3 newDir = (poly->polyPlane.normal);


							if (grassToolSetNormal)
							{
								newVob->SetHeadingAtWorld(newDir);
							}

							//newVob->SetHeadingYLocal(newDir);


							newVob->RotateLocalX(90);

							//newVob->ResetXZRotationsWorld();

							if (grassToolRotRandAngle)
							{
								newVob->RotateLocalY(GetRandVal(0, 360));
							}
							else
							{
								newVob->SetHeadingWorld(camVob->GetPositionWorld());
								newVob->RotateLocalY(90);
								newVob->ResetXZRotationsWorld();
							}




							if (!isItem)
							{
								newVob->Release();

								if (grassToolDynColl)
								{
									newVob->SetCollDetDyn(TRUE);
								}
							}


							theApp.SetSelectedVob(NULL);
						}
					}
				}

			}




			//print.PrintRed(Z vobsNeed.GetNumInList());
		}

	}

}