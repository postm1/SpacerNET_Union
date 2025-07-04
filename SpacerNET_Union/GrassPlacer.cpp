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


			int grassToolClearMouse = theApp.options.GetIntVal("grassToolClearMouse");

			if (mainTimer[TIMER_ID_OBJECTS_SEWER].Await(150) || grassToolClearMouse)
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



				if (ogame->GetWorld()->TraceRayNearestHit(cam->GetVob()->GetPositionWorld(), ray * 30000, (zCVob*)NULL, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_POLY_IGNORE_TRANSP)) {

					if (ogame->GetWorld()->traceRayReport.foundPoly) {

						auto poly = ogame->GetWorld()->traceRayReport.foundPoly;
						auto posToPlace = ogame->GetWorld()->traceRayReport.foundIntersection;

						//cmd << posToPlace.ToString() << endl;

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

							
							int grassToolDynColl = theApp.options.GetIntVal("grassToolDynColl");
							int grassToolRotRandAngle = theApp.options.GetIntVal("grassToolRotRandAngle");
							int grassToolSetNormal = theApp.options.GetIntVal("grassToolSetNormal");
							int grassToolSetGlobalParent = theApp.options.GetIntVal("grassToolGlobalParent");

							int grassToolcomboBoxVisualCamAlignValue = theApp.options.GetIntVal("grassToolcomboBoxVisualCamAlignValue");
							int grassToolcomboBoxVisualAniModeValue = theApp.options.GetIntVal("grassToolcomboBoxVisualAniModeValue");


							float grassToolvisualAniModeStrengthValue = theApp.options.GetFloatVal("grassToolvisualAniModeStrengthValue");
							float grassToolVobFarClipZScaleValue = theApp.options.GetFloatVal("grassToolVobFarClipZScaleValue");

							int grassToolcdStaticValue = theApp.options.GetIntVal("grassToolcdStaticValue");
							int grassToolstaticVobValue = theApp.options.GetIntVal("grassToolStaticVobValue");



							if (modelName.Length() == 0) {
								return;
							}

							if (grassToolClearMouse)
							{
								zinput->ClearLeftMouse();
								zinput->ClearKeyBuffer();
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
									&& vob->GetVisual()->GetVisualName() == modelName
									&& vob != theApp.currentVobRender
									&& vob != pfxManager.testVob
									&& vob != pfxEditorVob
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
									&& vob != pfxEditorVob
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
									print.PrintRed(GetLang("SEWER_NO_PLACE_TO_PUT_VOB"));
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

							bool collDetDyn = newVob->GetCollDetDyn();
							bool collDetStat = newVob->GetCollDetStat();

							newVob->SetCollDet(FALSE);

							newVob->SetPositionWorld(posToPlace + zVEC3(0, offsetVert, 0) + point);

							newVob->SetCollDetDyn(collDetDyn);
							newVob->SetCollDetStat(collDetStat);
							

							if (grassToolSetGlobalParent && theApp.globalParent && !isItem)
							{
								InsertIntoWorld(newVob, theApp.globalParent, false);
							}
							else if (!isItem)
							{
								
								InsertIntoWorld(newVob, NULL, false);
							}


							poly->CalcNormal();

							zVEC3 newDir = (poly->polyPlane.normal);


							if (grassToolSetNormal)
							{
								newVob->SetHeadingAtWorld(newDir);
								newVob->RotateLocalX(90);
							}

							//newVob->SetHeadingYLocal(newDir);


							//

							//newVob->ResetXZRotationsWorld();

							

							if (grassToolRotRandAngle)
							{
								newVob->RotateLocalY(GetRandVal(0, 360));
							}
							else if (!grassToolSetNormal)
							{

								newVob->SetHeadingWorld(camVob->GetPositionWorld());
								newVob->RotateLocalY(90);
								newVob->ResetXZRotationsWorld();
							}


							//apply some settings from sewer window


#if ENGINE >= Engine_G2
							newVob->m_fVobFarClipZScale = isItem ? 1 : grassToolVobFarClipZScaleValue;
#endif

							if (!isItem)
							{
								newVob->Release();



								/*cmd << " grassToolcomboBoxVisualCamAlignValue: " << grassToolcomboBoxVisualCamAlignValue << endl;
								cmd << " grassToolcomboBoxVisualAniModeValue: " << grassToolcomboBoxVisualAniModeValue << endl;
								cmd << " grassToolvisualAniModeStrengthValue: " << grassToolvisualAniModeStrengthValue << endl;
								cmd << " grassToolVobFarClipZScaleValue: " << grassToolVobFarClipZScaleValue << endl;
								cmd << " grassToolcdStaticValue: " << grassToolcdStaticValue << endl;
								cmd << " grassToolStaticVobValue: " << grassToolstaticVobValue << endl;*/


								newVob->SetCollDetStat(grassToolcdStaticValue);
								newVob->SetStaticVob((zBOOL)grassToolstaticVobValue);

								switch (grassToolcomboBoxVisualCamAlignValue)
								{
								case 0: newVob->SetVisualCamAlign((zTVisualCamAlign)zVISUAL_CAMALIGN_NONE); break;
								case 1: newVob->SetVisualCamAlign((zTVisualCamAlign)zVISUAL_CAMALIGN_YAW); break;
								case 2: newVob->SetVisualCamAlign((zTVisualCamAlign)zVISUAL_CAMALIGN_FULL); break;
								}
#if ENGINE >= Engine_G2
								newVob->m_AniMode = (zTAnimationMode)grassToolcomboBoxVisualAniModeValue;
								newVob->m_aniModeStrength = grassToolvisualAniModeStrengthValue;
#endif

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