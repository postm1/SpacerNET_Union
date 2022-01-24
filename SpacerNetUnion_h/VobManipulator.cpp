// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	extern Timer mainTimer;
	extern void _GetCursorPos(POINT* cur);

	// Add your code here . . .
	// обновить сетку вейпоинтов при перемещении WP
	void WayMovePoint(zCVob* wpvob)
	{
		if (!dynamic_cast<zCVobWaypoint*>(wpvob)) return;
		zCWorld* world = ogame->GetWorld(); if (!world) return;
		zCWayNet* waynet = world->GetWayNet(); if (!waynet) return;

		zCWaypoint* wp = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(wpvob));
		// neue koordinaten des Wp
		if (wp) wp->UpdatePositionWorld();
	}


	bool GetFloorPosition(zCVob* vob, zVEC3& centerPos)
	{
		if (!vob->GetHomeWorld()) return FALSE;

		// Auf passende Hцhe setzen
		zREAL diff = vob->GetPositionWorld()[VY] - vob->GetBBox3DWorld().mins[VY];
		zCWorld* wld = vob->GetHomeWorld();

		if (wld->TraceRayNearestHit(centerPos, zVEC3(0, -5000, 0), vob, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN)) {
			// Poly gefunden
			if (wld->traceRayReport.foundPoly || wld->traceRayReport.foundVob) {
				// Schnittpunkt schnappen und Position neu setzen
				zVEC3 newpos = wld->traceRayReport.foundIntersection;
				// 4 cm bis zum Bodem, um Kollision zu vermeiden.
				newpos[VY] += diff + 2;
				centerPos = newpos;
				return TRUE;
			}
		}
		return FALSE;
	}


	void SetOnFloor(zCVob* vob)
	{

		zVEC3 newPos = vob->GetPositionWorld();

		if (GetFloorPosition(vob, newPos)) {
			HandleVobTranslation(vob, newPos);
		}

		if (dynamic_cast<zCVobWaypoint*>(vob))
		{
			WayMovePoint(vob);
		}


	}



	struct VobMovement
	{
		zCVob* vob;
		bool collDyn;
		bool collStat;
		zVEC3 pos;

	public:
		void RestoreColl()
		{
			vob->SetCollDetDyn(collDyn);
			vob->SetCollDetStat(collStat);
		}
	};

	zCList<VobMovement> vobsToMove;

	void GetChildren(zCTree<zCVob>* node, zCVob* pickedVob)
	{
		zCVob* vob = node->GetData();

		if (vob)
		{

			VobMovement* vobStr = new VobMovement();
			vobStr->vob = vob;
			vobStr->collDyn = vob->collDetectionDynamic;
			vobStr->collStat = vob->collDetectionStatic;
			vobStr->pos = vob->GetPositionWorld();

			vobsToMove.Insert(vobStr);

			vob->SetCollDetDyn(FALSE);
			vob->SetCollDetStat(FALSE);

		}

		node = node->GetFirstChild();

		while (node != NULL)
		{
			GetChildren(node, pickedVob);
			node = node->GetNextChild();
		}
	}


	void HandleVobRotation(zCVob* pickedVob, int type, float angle)
	{

		//bool collDyn = pickedVob->collDetectionDynamic;
		//bool collStat = pickedVob->collDetectionStatic;

		zVEC3 pos = pickedVob->GetPositionWorld();
		zVEC3 vobUnit = pickedVob->GetAtVectorWorld().Normalize();

		zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

		//pickedVob->SetCollDetDyn(FALSE);
		//pickedVob->SetCollDetStat(FALSE);


		if (pickedVob->globalVobTreeNode)
		{
			GetChildren(pickedVob->globalVobTreeNode, pickedVob);
		}


		if (type == 1)
		{
			pickedVob->RotateWorld(camUnit.Cross(zVEC3(0, 1, 0)).Normalize(), angle);
		}

		if (type == 2)
		{
			zVEC3 newVec = camUnit;
			newVec[1] = 0;

			pickedVob->RotateWorld(newVec, -angle);
		}

		if (type == 3)
		{
			pickedVob->RotateWorldY(angle);
		}

		if (type == 4)
		{
			print.PrintRed(GetLang("UNION_VOB_AXIS_RESET"));
			pickedVob->SetHeadingAtWorld(zVEC3(0, 1, 0));
		}

		if (type == 5)
		{
			print.PrintRed(GetLang("UNION_VOB_AXIS_RESET"));
			pickedVob->SetHeadingAtWorld(zVEC3(1, 0, 0));
		}

		if (type == 6)
		{
			print.PrintRed(GetLang("UNION_VOB_AXIS_RESET"));
			pickedVob->SetHeadingAtWorld(zVEC3(0, 0, 1));
		}


		if (dynamic_cast<zCVobWaypoint*>(pickedVob))
		{
			WayMovePoint(pickedVob);
		}

		//pickedVob->SetCollDetDyn(collDyn);
		//pickedVob->SetCollDetStat(collStat);



		for (int i = 0; i < vobsToMove.GetNumInList(); i++)
		{
			vobsToMove.Get(i)->RestoreColl();
		}


		vobsToMove.DeleteListDatas();

	}


	void HandleVobTranslation(zCVob* pickedVob, zVEC3 pos)
	{

		//bool parentAlone = theApp.options.GetIntVal("translateParentAlone");

		if (pickedVob->globalVobTreeNode)
		{
			GetChildren(pickedVob->globalVobTreeNode, pickedVob);

			
		}

		
		pickedVob->SetPositionWorld(pos);


		if (dynamic_cast<zCVobWaypoint*>(pickedVob))
		{
			WayMovePoint(pickedVob);
		}


		for (int i = 0; i < vobsToMove.GetNumInList(); i++)
		{
			vobsToMove.Get(i)->RestoreColl();
		}


		vobsToMove.DeleteListDatas();
	}




	zCVob* baseSelectVob = NULL;

	void GetNextChildCopy(zCTree<zCVob>* node, zCVob* parent, bool rootVob = false, bool useHierarchy = false)
	{
		zCVob* farVobCurrent = node->GetData();
		zCVob* newVob = NULL;
		zCVob* farParent = NULL;

		cmd << Col16(CMD_YELLOW) << "Copy vob: " << GetVobName(farVobCurrent) << " parent: " << GetVobName(parent) << endl;

		if (farVobCurrent)
		{
			newVob = dynamic_cast<zCVob*>(farVobCurrent->CreateCopy());

			if (newVob)
			{
				zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
				zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				farParent = farVobCurrent->GetParentVob();


				if (rootVob)
				{
					baseSelectVob = newVob;

					if (theApp.options.GetIntVal("vobInsertItemLevel") && theApp.vobToCopy)
					{
						zVEC3 levelVec = pos + dir * 250;
						levelVec.n[1] = theApp.vobToCopy->GetPositionWorld().n[1];

						if (theApp.isNextCopyVobInsertNear)
						{
							levelVec = theApp.vobToCopy->GetPositionWorld();
						}

						HandleVobTranslation(newVob, levelVec);
					}
					else
					{
						if (theApp.isNextCopyVobInsertNear && theApp.vobToCopy)
						{
							HandleVobTranslation(newVob, theApp.vobToCopy->GetPositionWorld());
						}
						else
						{
							HandleVobTranslation(newVob, pos + dir * 250);
						}
						
					}
				}
				else if (farParent)
				{
					HandleVobTranslation(newVob, parent->GetPositionWorld() + (farVobCurrent->GetPositionWorld() - farParent->GetPositionWorld()));
				}




				zCVobSpot* freePoint = dynamic_cast<zCVobSpot*>(newVob);
				zCVobWaypoint* waypoint = dynamic_cast<zCVobWaypoint*>(newVob);


				if (freePoint || waypoint)
				{
					//cmd << "========================RESET====================" << endl;
					

					if (theApp.turnWpMode == TurnWpMode::AGAINST_CAMERA)
					{
						newVob->SetHeadingWorld(pos);
						newVob->RotateLocalY(180);
					}

					if (theApp.turnWpMode == TurnWpMode::ON_CAMERA)
					{
						newVob->SetHeadingWorld(pos);
					}

					newVob->ResetXZRotationsWorld();
				}
				else if (theApp.options.GetIntVal("vobInsertVobRotRand"))
				{
					newVob->RotateLocalY(GetRandVal(0, 360));
				}
			
				//newVob->ResetXZRotationsWorld();

				if (waypoint)
				{
					newVob->SetVobName(GetNextWayPointName(newVob->GetVobName()));
					RecalcWPBBox(newVob);
				}


				if (freePoint)
				{
					newVob->SetVobName(GetNextFreePointName(newVob->GetVobName()));
				}



				InsertIntoWorld(newVob, parent, false);

				newVob->Release();


				if (freePoint)
				{
					SetOnFloor(newVob);
				}

				if (waypoint)
				{
					if (newVob)
					{
						zCWayNet* waynet = ogame->GetWorld()->GetWayNet();
						if (waynet)
						{
							zCWaypoint* wp1 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(theApp.GetSelectedVob()));
							zCWaypoint* wp2 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(newVob));

							if (wp1 && wp2 && wp1 != wp2)
							{
								if (!waynet->HasWay(wp1, wp2))
								{
									waynet->CreateWay(wp1, wp2);
								}
							}

						}
					}
				}




			}

		}

		if (!useHierarchy)
		{
			return;
		}

		node = node->GetFirstChild();

		while (node != NULL)
		{
			GetNextChildCopy(node, newVob, false, useHierarchy);
			node = node->GetNextChild();
		}
	}

	bool checkCopyVobSelf = false;

	void CheckVobs(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		if (vob)
		{
			if (vob == theApp.pickedVob)
			{
				checkCopyVobSelf = true;
				return;
			}
		}

		node = node->GetFirstChild();

		while (node != NULL)
		{
			CheckVobs(node);
			node = node->GetNextChild();
		}
	}

	

	void HandleInsertVobCopy(zCVob* pickedVob)
	{
		if (theApp.vobToCopy)
		{
			zCWorld* pWorld = ogame->GetWorld();

			auto parent = theApp.GetSelectedVob();

			if (theApp.globalParent) parent = theApp.globalParent;

			if (parent && parent->IsPFX())
			{
				print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

				return;
			};

			



			bool useHierarchy = theApp.options.GetIntVal("vobInsertHierarchy");

			zCVobWaypoint* pickedVobWaypont = dynamic_cast<zCVobWaypoint*>(theApp.vobToCopy);
			zCVobSpot* pickedFreePoint = dynamic_cast<zCVobSpot*>(theApp.vobToCopy);
			oCItem* pItem = dynamic_cast<oCItem*>(theApp.vobToCopy);

			checkCopyVobSelf = false;

			CheckVobs(theApp.vobToCopy->globalVobTreeNode);

			if (useHierarchy && (theApp.vobToCopy == theApp.pickedVob || checkCopyVobSelf) && !pItem && !pickedVobWaypont)
			{
				//print.PrintRed("Не могу вставить воб самого в себя!");
				//return;
				useHierarchy = false;
			}



			theApp.vobToCopy->SetDrawBBox3D(FALSE);

			if (theApp.pickedVob)
			{
				theApp.pickedVob->SetDrawBBox3D(FALSE);
			}


			zCTree<zCVob>* tree = theApp.vobToCopy->globalVobTreeNode;

			

			GetNextChildCopy(tree, parent, true, useHierarchy);


			theApp.SetSelectedVob(baseSelectVob, "Control+V");
			auto onSelect = (onSelectNode)GetProcAddress(theApp.module, "SelectNode");
			onSelect((uint32)theApp.pickedVob);
			print.PrintGreen(GetLang("UNION_VOB_INSERTED"));

			if (pickedFreePoint)
			{
				theApp.SetSelectedVob(NULL);
			}
		}
	}



	void GetChildrenUpdateParent(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();
		zCVob* parentVob = vob->GetParentVob();

		if (vob)
		{
			updateParentAddNode addNode = (updateParentAddNode)GetProcAddress(theApp.module, "updateParentAddNode");
			addNode((uint)vob, (uint)parentVob);
		}

		node = node->GetFirstChild();

		while (node != NULL)
		{
			GetChildrenUpdateParent(node);
			node = node->GetNextChild();
		}
	}


	//vob - Воб, которому меняем родителя; newParent - воб новый родитель
	void HandleParentChange(zCVob* vob, zCVob* newParent)
	{
		zCWorld* pWorld = ogame->GetWorld();


		if (newParent && newParent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;

		};

		if (dynamic_cast<zCVobWaypoint*>(vob) || dynamic_cast<zCVobLevelCompo*>(vob) || dynamic_cast<zCZone*>(vob) && (!dynamic_cast<zCVobSound*>(vob)))
		{

			print.PrintRed(GetLang("CANT_APPLY_PARENT"));

			return;
		}

		if (dynamic_cast<oCItem*>(newParent))
		{
			print.PrintRed(GetLang("PARENT_ERROR_OCITEM"));
			return;
		}


		if (vob  && vob != newParent && pWorld)
		{
			// если есть новый родитель
			if (newParent)
			{
				pWorld->MoveVobSubtreeTo(vob, newParent->globalVobTreeNode);

				vob->drawBBox3D = false;
				newParent->drawBBox3D = false;
			}
			else
			{
				// иначе вставляем в случайный zCVobLevelCompo
				zCArray<zCVob*> resultList;
				pWorld->SearchVobListByClass(zCVobLevelCompo::classDef, resultList, 0);

				if (resultList.GetNumInList() > 0)
				{
					zCVob* parentVob = resultList.GetSafe(rand() % resultList.GetNumInList());

					if (parentVob)
					{
						pWorld->MoveVobSubtreeTo(vob, parentVob->globalVobTreeNode);
					}
				}
				else
				{
					return;
				}


			}

			updateParentVobRemoveNode removeNode = (updateParentVobRemoveNode)GetProcAddress(theApp.module, "UpdateParentRemoveNode");
			removeNode((uint)vob);
			GetChildrenUpdateParent(vob->globalVobTreeNode);


			theApp.SetSelectedVob(vob, "Control+V");
			auto onSelect = (onSelectNode)GetProcAddress(theApp.module, "SelectNode");
			onSelect((uint32)theApp.pickedVob);

			theApp.vobToCopy = NULL;

			print.PrintRed(GetLang("PARENT_CHANGE_OK"));



		}
	}

	int selectedTool = 0;

	void SetSelectedTool(int tool)
	{
		selectedTool = tool;

		if (theApp.isGrattControlActive)
		{
			ControllerEvents.MotionMode = (zEVobMotionMode)tool;
		}
	}


	void CopyClipBoard(zSTRING str)
	{
		const char* output = str.ToChar();
		const size_t len = strlen(output) + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		memcpy(GlobalLock(hMem), output, len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}

	
	void MarkAlhpa()
	{
		static bool init = false;

		if (init) return;

		zCMaterial *mat = zNEW(zCMaterial("Occluder_Poly_Mat"));
		mat->color = zCOLOR(255, 255, 255);

		// [EDENFELD] 1.09 Crash bei ZMARK OCCLUDERPOLYS behoben
		auto bspTree = ogame->GetWorld()->bspTree;

		bspTree.mesh->CreateListsFromArrays();

		for (int i = 0; i<bspTree.mesh->numPoly; i++) {
			zCPolygon *poly = bspTree.mesh->Poly(i);
			// [EDENFELD] 1.09 Portale werden nicht mit markiert
			if (poly->flags.occluder || poly->flags.ghostOccluder) {
				poly->SetMaterial(mat);
			};
		};

		init = true;

	}

	/*
	zCArray<zCVob*> vobs;
	zCArray<zCVob*> vobsNeed;

	ogame->GetWorld()->SearchVobListByClass(zCVob::classDef, vobs, 0);

	ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, vobs, 0);

	for (int i = 0; i < vobs.GetNum(); i++)
	{
	zCVob* pVob = vobs.GetSafe(i);

	if (dynamic_cast<zCVobLevelCompo*>(pVob))	continue;
	if (pVob == ogame->GetCamera()->GetVob())	continue;


	if (pVob && pVob->GetVisual() && pVob->GetVisual()->GetVisualName() == "NW_NATURE_GRASSGROUP_01.3DS")
	{
	vobsNeed.Insert(pVob);

	}


	}
	*/


	void GrassPlacing()
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
				cam->BackProject(ax, ay, p);				// p im camSpace
				p = cam->camMatrixInv * p;					// p im world(obj)Space  
				ray = p - ray00;

				ray = ray.Normalize();

				ogame->GetWorld()->PickScene(*ogame->GetCamera(), ax, ay, -1);



				if (ogame->GetWorld()->TraceRayFirstHit(cam->GetVob()->GetPositionWorld(), ray * 25000, (zCVob*)NULL, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_VOB_IGNORE)) {
					// Poly gefunden
					if (ogame->GetWorld()->traceRayReport.foundPoly) {
						// Schnittpunkt schnappen und Position neu setzen
						auto poly = ogame->GetWorld()->traceRayReport.foundPoly;
						auto posToPlace = ogame->GetWorld()->traceRayReport.foundIntersection;
					

						if (poly)
						{
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

							newVob->SetCollDetStat(FALSE);
							newVob->SetCollDetDyn(FALSE);
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

	zCTexture* GetScreenTex(zSTRING name)
	{
		zCTextureConvert* texCon = zrenderer->CreateTextureConvert();
		zrenderer->Vid_GetFrontBufferCopy(*texCon);
		zCTextureInfo texInfo = texCon->GetTextureInfo();

		texInfo.texFormat = zRND_TEX_FORMAT_RGB_565;
		texInfo.numMipMap = 1;
		texInfo.sizeX = 256;
		texInfo.sizeY = 256;
		texInfo.refSizeX = 256;
		texInfo.refSizeY = 256;
		texCon->SetDetailTextureMode(FALSE);
		texCon->ConvertTextureFormat(texInfo);

		zCTexture* tex = zrenderer->CreateTexture();
		zCTextureExchange::CopyContents(texCon, tex);

		texCon->SaveToFileFormat(name + "-C.TEX");

		return tex;
	}

	void CreateInvIcons()
	{
		if (zKeyToggled(KEY_F1)) {
			zinput->ClearKeyBuffer();

			zSTRING scriptClassName = "C_Item";

			zCView* dtp_item = NULL;

			if (!scriptClassName.IsEmpty())
			{

				int pos, typ, anz, c, classindex;
				// zSTRING* s;

				CString f1, f2, f3;
				classindex = parser->GetIndex(scriptClassName);
				if (classindex != -1)
				{
					parser->GetSymbolInfo(classindex, typ, anz);
					pos = parser->GetInstance(classindex, 0);

					int count = 0;

					while (pos >= 0)
					{
						zSTRING s = parser->GetSymbolInfo(pos, typ, c);
						pos = parser->GetInstance(classindex, pos + 1);

						auto vob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance("oCItem"));
						if (!vob) return;

						if (dynamic_cast<oCItem*>(vob))
							dynamic_cast<oCItem*>(vob)->InitByScript(parser->GetIndex(s), 1);
						else zRELEASE(vob);
						if (!dtp_item)
							dtp_item = new zCView(0, 0, 8191, 8191, VIEW_VIEWPORT);
						zrenderer->Vid_Clear(zCOLOR(100, 100, 100, 255), 3);


						if (dynamic_cast<oCItem*>(vob))
							dynamic_cast<oCItem*>(vob)->oCItem::RenderItem(ogame->GetWorld(), dtp_item, 0.0);

						GetScreenTex(dynamic_cast<oCItem*>(vob)->GetInstanceName());

						count += 1;

						if (count >= 20)
						{
							//break;
						}
					}


				}

			};
		};

	}

	struct BadVobsEntry
	{
		zCVob* parentVob;
		zCArray<zCVob*> vobParentChangeList;

		BadVobsEntry::BadVobsEntry()
		{
			parentVob = NULL;
		}
	};

	
	void CheckVobHasChildren(zCTree<zCVob>* node, zCVob* original, BadVobsEntry* entry)
	{
		zCVob* vob = node->GetData();

		if (vob && vob != original) {


			entry->vobParentChangeList.InsertEnd(vob);
			
		}

		node = node->GetFirstChild();


		while (node != NULL)
		{
			CheckVobHasChildren(node, original, entry);
			node = node->GetNextChild();
		}
	}

	
	void SearchBadHierarchy()
	{
		static int start = 0;
		static zCList<BadVobsEntry> pList;


		/*
		theApp.SetSelectedVob(vobsNeed.GetSafe(i));
							auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
							onSelect((int)theApp.pickedVob);
		
		
		*/
		if (zKeyToggled(KEY_F4))
		{
			auto badEntry = pList.Get(start);

			theApp.SetSelectedVob(badEntry->parentVob);
			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((int)theApp.pickedVob);


			start++;

			if (start > pList.GetNumInList() - 1) start = 0;
		}

		if (zKeyToggled(KEY_F2)) {
			zinput->ClearKeyBuffer();

			start = 0;
			for (int i = 0; i < pList.GetNumInList(); i++)
			{
				auto badEntry = pList.Get(i);

				if (badEntry)
				{


					for (int j = 0; j < badEntry->vobParentChangeList.GetNumInList(); j++)
					{
						auto pVob = badEntry->vobParentChangeList.GetSafe(i);
						HandleParentChange(pVob, NULL);
					}

				}


				//HandleParentChange(badChildren, NULL);
			}

		}


		if (zKeyToggled(KEY_F1)) {
			zinput->ClearKeyBuffer();

			start = 0;
			zCArray<zCVob*> vobs;
			zCArray<zCVob*> vobsNeed;

			ogame->GetWorld()->SearchVobListByClass(zCVob::classDef, vobs, 0);

			ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, vobs, 0);

			pList.DeleteList();

			for (int i = 0; i < vobs.GetNum(); i++)
			{
				zCVob* pVob = vobs.GetSafe(i);

				if (dynamic_cast<zCVobLevelCompo*>(pVob))	continue;
				if (pVob == ogame->GetCamera()->GetVob())	continue;
				if (dynamic_cast<zCMover*>(pVob))	continue;
				if (dynamic_cast<zCTrigger*>(pVob))	continue;
				if (dynamic_cast<zCMoverControler*>(pVob))	continue;

				if (pVob && pVob->IsPFX())
				{
					if (vobsNeed.Search(pVob) == Invalid)
					{
						vobsNeed.Insert(pVob);
					}
					
					
				}
			}

			int found_count = 0;

			for (int i = 0; i < vobsNeed.GetNum(); i++)
			{
				auto pVob = vobsNeed.GetSafe(i);

				if (pVob)
				{
					zCTree<zCVob>* tree = pVob->globalVobTreeNode;

					auto entry = new BadVobsEntry();

					CheckVobHasChildren(tree, pVob, entry);
					entry->parentVob = pVob;
					
					if (entry->vobParentChangeList.GetNumInList() > 0)
					{
						pList.Insert(entry);
					}
					
				}
			}
			cmd << endl;
			cmd << ("Всего PFX: " + Z pList.GetNumInList()) << endl;

			int childCount = 0;

			for (int i = 0; i < pList.GetNumInList(); i++)
			{
				auto badEntry = pList.Get(i);

				if (badEntry)
				{

					

					cmd << ("======== Entry: children: " + Z badEntry->vobParentChangeList.GetNumInList() 
						+ " " + GetVobName(badEntry->parentVob)) + " " + Z (int)badEntry->parentVob << endl;

					for (int j = 0; j < badEntry->vobParentChangeList.GetNumInList(); j++)
					{
						auto pVob = badEntry->vobParentChangeList.GetSafe(i);

						childCount += 1;

						cmd << "Child: " << GetVobName(pVob) + " " + Z(int)pVob << endl;
					}

				}


				//HandleParentChange(badChildren, NULL);
			}

			cmd << "AllVobsToMove: " + Z childCount << endl;
			
			
			
			
		}
	}



	void VobKeys()
	{



		auto pickMode = theApp.GetPickMode();

		if (pickMode == SWM_GRASS)
		{
			if (theApp.GetSelectedVob())
			{
				theApp.SetSelectedVob(NULL);

			}
			GrassPlacing();
			return;
		}

		if (pickMode == SWM_MULTISELECT && !theApp.isGrattControlActive)
		{
			if (theApp.GetSelectedVob())
			{
				theApp.SetSelectedVob(NULL);
			}

			Selector.DoPick();

			return;
		
		}

		if (pickMode != SWM_MULTISELECT)
		{
			Selector.ClearSelection();
		}
		

	
		//SearchBadHierarchy();
		

		if (!selectedTool)
		{
			SetSelectedTool(1);
		}

		zCVob* pickedVob = theApp.GetSelectedVob();


		

		if (keys.KeyPressed("VOB_TRANSLATE", true))
		{
			print.PrintRed(GetLang("TOOL_TRANS"));
			SetSelectedTool(1);
		}

		if (keys.KeyPressed("VOB_ROTATE", true))
		{
			print.PrintRed(GetLang("TOOL_ROT"));
			SetSelectedTool(2);
		}


		

		if (pickMode == SWM_MATERIALS)
		{
			if (!theApp.pickUnshareShow)
			{
				theApp.pickUnshareShow = true;

				print.PrintRed("Unshare features of polygons");

				//ogame->GetWorld()->bspTree.mesh->ArraysToLists();
				//ogame->GetWorld()->bspTree.mesh->UnshareFeatures();


			}
		}

		//rx_respawn
		/*
		if (zinput->KeyPressed(KEY_G))
		{
			zinput->ClearKeyBuffer();
			

			theApp.showRespawnOnVobs = !theApp.showRespawnOnVobs;

			print.PrintRed("ShowRespawnPoints: " + Z theApp.showRespawnOnVobs);

			if (theApp.showRespawnOnVobs)
			{
				theApp.ClearRespList();

				CString funcName = "startup_goldmine";

				parser->SetScriptInt("kapitel", 6);

				parser->CallFunc(parser->GetIndex(funcName));

			}

		}
		*/
		

		if (theApp.isGrattControlActive && keys.KeyPressed("VOB_RESET_AXIS", true, true))
		{
			int motion = (int)ControllerEvents.MotionHeading + 1;

			if (motion > 3)
			{
				motion = 0;
			}

			switch (motion)
			{
				case 0: print.PrintRed("zMH_WORLD"); break;
				case 1: print.PrintRed("zMH_LOCALE"); break;
				case 2: print.PrintRed("zMH_VIEW"); break;
			}

			ControllerEvents.MotionHeading = (zEVobMotionHeading)motion;


		}


		if (keys.KeyPressed("VOB_COPY", true))
		{


			if (pickMode == SWM_VOBS)
			{
				if (!dynamic_cast<zCVobLevelCompo*>(theApp.pickedVob))
				{
					print.PrintRed(GetLang("VOB_COPY_OK"));
					theApp.vobToCopy = theApp.pickedVob;
					theApp.isVobParentChange = false;
				}
			}
			else if (pickMode == SWM_MATERIALS)
			{
				if (mm.selPolyList && mm.selPolyList->GetNumInList() > 0)
				{
					mm.copyMat = mm.selPolyList->Get(0)->GetPolygon()->material;
					print.PrintRed("Материал скопирован");
				}

			}


		}

		if (keys.KeyPressed("VOB_CUT", true))
		{
			print.PrintRed(GetLang("VOB_CUT_OK"));
			theApp.vobToCopy = theApp.pickedVob;
			theApp.isVobParentChange = true;

		}

		if (keys.KeyPressed("VOB_INSERT", true))
		{
			if (pickMode == SWM_VOBS)
			{
				if (theApp.isVobParentChange)
				{
					HandleParentChange(theApp.vobToCopy, pickedVob);
				}
				else
				{


					if (theApp.options.GetIntVal("selectMoveWhenVobInsert"))
					{
						if (!theApp.isGrattControlActive)
							SetSelectedTool(1);
					}

					HandleInsertVobCopy(pickedVob);
				}
			}
			else if (pickMode == SWM_MATERIALS)
			{
				if (mm.copyMat)
				{
					mm.OnPolyApplyTexture();
					mm.PolyApplyMapping();
					mm.RestoreMat();
					print.PrintRed("Применено");
				}
			}

		}


		if (keys.KeyPressed("VOB_NEAR_CAM", true))
		{
			if (theApp.pickedVob)
			{
				if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
				{
					HandleVobTranslation(pickedVob, ogame->GetCamera()->connectedVob->GetPositionWorld() +

						ogame->GetCamera()->connectedVob->GetAtVectorWorld() * 200);

					print.PrintRed(GetLang("VOB_NEAR_CAMERA"));
				}
			}
			if (!theApp.isGrattControlActive)
				SetSelectedTool(1);

		}



		if (keys.KeyPressed("VOB_DISABLE_SELECT", true))
		{

			if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
			if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

			theApp.SetSelectedVob(NULL);
			theApp.pickedWaypoint2nd = NULL;

			auto pickMode = theApp.GetPickMode();

			if (pickMode == SWM_VOBS)
			{
				//print.PrintRed(GetLang("TOOL_UNSELECT"));
			}

			mm.CleanSelectMaterial();
			(callVoidFunc)GetProcAddress(theApp.module, "CleanPropWindow")();
		}

		if (pickedVob)
		{
			if (keys.KeyPressed("WP_TOGGLE", true))	
			{
				theApp.ToggleWP();
			}

			if (keys.KeyPressed("VOB_FLOOR", true))
			{
				print.PrintRed(GetLang("TOOL_FLOOR"));
				SetOnFloor(pickedVob);
			}



			if (keys.KeyPressed("VOB_DELETE", true))
			{
				theApp.RemoveVob(pickedVob);
			}

		}

	}

	void VobMoving()
	{

		//CreateInvIcons();
		
		

			/*
			zinput->ClearKeyBuffer();
			found = 0;

			if (mm.selPolyList->GetNum() == 0)
			{
				return;
			}

			zTBBox3D  camBox;

			zCPolygon **polyList;
			int	numPolys = 0;

			zVEC3 testPos = ogame->GetCamera()->connectedVob->GetPositionWorld();

			camBox.mins = testPos - (zVEC3(1, 1, 1) * 40000);
			camBox.maxs = testPos + (zVEC3(1, 1, 1) * 40000);
			
			
			if (ogame->GetWorld()->GetBspTree()->bspRoot->CollectPolysInBBox3D(camBox, polyList, numPolys))
			{
				zCMaterial*		mat = 0;
				int				planeClass;
				for (int i = 0; i<numPolys; i++)
				{
					mat = polyList[i]->material;

					if (mat->GetName() == mm.selPolyList->Get(0)->GetMaterial()->GetName())
					{
						polyList[i]->SetMaterial(mm.SelectMaterial);
						found++;


						//mm.selPolyList->Insert(polyList[i]);

						//ogame->GetCamera()->connectedVob->SetPositionWorld((*polyList[i]->vertex)->position);
					}
				}
			}

			print.PrintRed("found: " + ToStr found);
		}

		
		

		if (zinput->KeyPressed(KEY_H))
		{
			for (int i = 0; i < polys.GetNumInList(); i++)
			{
				if (indexA == i)
				{
					ogame->GetCamera()->connectedVob->SetPositionWorld((*polys[i]->vertex)->position);
					indexA++;
					break;
				}
			}
		}
		*/	

			/*
			zCArray<zCVob*> vobs;
			zCArray<zCVob*> vobsNeed;

			zVEC3 pos1 = zVEC3(-24041, -4272, 63022);
			zVEC3 pos2 = zVEC3(-15210, -3878, 49644);

			zVEC3 offset = zVEC3(0, 0, 0); // zVEC3(-8883.83, -3753.78, 26805.9) - zVEC3(12868.4, 3665, -21332);
			zCVob* vobPivot = NULL;

			 //(17500)
			//	- 15210, -3878, 49644 (14000)

			ogame->GetWorld()->SearchVobListByClass(zCVob::classDef, vobs, 0);

			ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, vobs, 0);

			for (int i = 0; i < vobs.GetNum(); i++)
			{
				zCVob* pVob= vobs.GetSafe(i);

				if (dynamic_cast<zCVobLevelCompo*>(pVob))	continue;
				if (pVob == ogame->GetCamera()->GetVob())	continue;


				if (pVob->GetVobName() == "VOBTREE_PIVOT")
				{
					vobPivot = pVob;

				}

				if (pVob && ((pVob->GetPositionWorld() - pos1).Length() <= 17500) || (pVob->GetPositionWorld() - pos2).Length() <= 14000)
				{
					
					vobsNeed.Insert(pVob);
				}
			}

			vobsNeed.RemoveDoubles();

			theApp.CreateNewVob("zCVob", "VOBTREE_MAIN_ADDONWORLD", "", 0, 0);

			if (vobPivot)
			{
				theApp.GetSelectedVob()->SetPositionWorld(vobPivot->GetPositionWorld());
			}
			else
			{
				Message::Box("no pivot vob");
				return;
			}
			

			zCVob* parent = theApp.GetSelectedVob();
			int p = 0;

			for (int i = 0; i < vobsNeed.GetNum(); i++)
			{
				zCVob* pVob = vobsNeed.GetSafe(i);

				if (pVob && (pVob->GetParentVob() && dynamic_cast<zCVobLevelCompo*>(pVob->GetParentVob())))
				{
					HandleVobTranslation(pVob, pVob->GetPositionWorld() + offset);
					HandleParentChange(pVob, parent);
					p++;
				}
			}

			print.PrintRed(ToStr vobs.GetNumInList());
			print.PrintRed(ToStr vobsNeed.GetNumInList());
			print.PrintRed(ToStr p);
			return;
		}
		*/


		/*
		if (zinput->GetMouseButtonPressedRight() && selPolyList && selPolyList->GetNum() > 0)
		{
			selPolyList->Get(0)->GetMaterial()->SetTexture(zSTRING("AV_FIREDRAGON_GROUND_03.TGA"));
		}
		*/


		/*
		if (selectedVertext)
		{
			if (keys.KeyPressed("VOB_TRANS_UP", false, true))
			{

				selectedVertext->position[1] += 10;
			}

			if (keys.KeyPressed("VOB_TRANS_DOWN", false, true))
			{

				selectedVertext->position[1] -= 10;
			}

		}
		*/



		

		/*
		if (keys.KeyPressed("TEST_MOVE_LC", true))
		{
			print.PrintRed("Хаарт опускает вобы");

		}
		*/

		if (!selectedTool)
		{
			SetSelectedTool(1);
		}

		zCVob* pickedVob = theApp.GetSelectedVob();

		auto pickMode = theApp.GetPickMode();


		if (!zinput->GetMouseButtonPressedRight() && pickMode == SWM_MATERIALS)
		{

			float mod = 1.0f;
			if (keys.KeyPressed("VOB_SPEED_X10", false, true))
			{
				mod *= 10;
			}


			if (keys.KeyPressed("VOB_SPEED_MINUS_10", false, true))
			{
				mod /= 10;
			}

			if (keys.KeyPressed("VOB_TRANS_UP", false, true))
			{

				
				mm.TextureScale(mod, mod);
				

			}

			if (keys.KeyPressed("VOB_TRANS_DOWN", false, true))
			{
				
				mm.TextureScale(-mod, -mod);
				

			}

			if (keys.KeyPressed("VOB_RESET_AXIS", true, true))
			{

				mm.ResetUV();


			}
		}

		if (!zinput->GetMouseButtonPressedRight() && pickedVob)
		{

			float speedTranslation = ((float)(theApp.options.GetIntVal("vobTransSpeed")) / 100000);
			float rotSpeed = ((float)(theApp.options.GetIntVal("vobRotSpeed")) / 1000);

			//print.PrintRed(zSTRING(speedTranslation, 10));


			float length = 0;

			if (pickedVob->GetVisual())
			{
				zTBBox3D bbox = pickedVob->GetVisual()->GetBBox3D();

				length = (bbox.maxs - bbox.mins).Length();
			}

			zCVobWaypoint* isWaypoint = dynamic_cast<zCVobWaypoint*>(pickedVob);
			zCVobSpot* isFreepoint = dynamic_cast<zCVobSpot*>(pickedVob);

			if (length >= 100 && (!isWaypoint && !isFreepoint))
			{
				speedTranslation = speedTranslation * (1 + length / 200);
			}

			if (isWaypoint || isFreepoint)
			{
				speedTranslation *= 2;
			}

			if (keys.KeyPressed("VOB_SPEED_X10", false, true))
			{
				speedTranslation *= 10;
				rotSpeed *= 3;
			}


			if (keys.KeyPressed("VOB_SPEED_MINUS_10", false, true))
			{
				speedTranslation /= 10;
				rotSpeed /= 10;
			}


			

			


			



			// общие манипуляции, вне зависимости от выбранного инструмента
			if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
			{
				zVEC3 pos = pickedVob->GetPositionWorld();
				zVEC3 vobUnit = pickedVob->GetAtVectorWorld().Normalize();

				zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				int y = pos.n[1];

				if (keys.KeyPressed("VOB_TRANS_UP", false, true))
				{

					if (pickMode == SWM_VOBS)
					{
						pos.n[1] += speedTranslation * ztimer->frameTimeFloat;
						HandleVobTranslation(pickedVob, pos);
					}
					else if (pickMode == SWM_MATERIALS)
					{
						mm.TextureScale(10, 10);
					}
					
				}

				if (keys.KeyPressed("VOB_TRANS_DOWN", false, true))
				{
					if (pickMode == SWM_VOBS)
					{
						pos.n[1] -= speedTranslation * ztimer->frameTimeFloat;
						HandleVobTranslation(pickedVob, pos);
					}
					else if (pickMode == SWM_MATERIALS)
					{
						mm.TextureScale(-10, -10);
					}
					
				}


				if (keys.KeyPressed("VOB_ROT_VERT_RIGHT", false, true))
				{
					HandleVobRotation(pickedVob, 3, ztimer->frameTimeFloat * rotSpeed);
				}

				if (keys.KeyPressed("VOB_ROT_VERT_LEFT", false, true))
				{
					HandleVobRotation(pickedVob, 3, -ztimer->frameTimeFloat * rotSpeed);
				}



				

				static int rotType = 4;

				if (keys.KeyPressed("VOB_RESET_AXIS", true))
				{
					HandleVobRotation(pickedVob, rotType, 0);

					rotType++;

					if (rotType > 6)
					{
						rotType = 4;
					}

				}


				// перемещение
				if (selectedTool == 1)
				{

					y = pos.n[1];
					zVEC3 unit = camUnit;
					unit[1] = 0;


					if (keys.KeyPressed("VOB_TRANS_FORWARD", false, true))
					{
						pos += unit * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);
					}

					if (keys.KeyPressed("VOB_TRANS_BACKWARD", false, true))
					{
						pos -= unit * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);
					}

					if (keys.KeyPressed("VOB_TRANS_LEFT", false, true))
					{
						zVEC3 left = camUnit.Cross(zVEC3(0, 1, 0));
						pos += left * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);
					}

					if (keys.KeyPressed("VOB_TRANS_RIGHT", false, true))
					{
						zVEC3 right = camUnit.Cross(zVEC3(0, -1, 0));
						pos += right * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);
					}



				}
				// вращение
				if (selectedTool == 2)
				{


					if (keys.KeyPressed("VOB_ROT_BACK", false, true))
					{
						HandleVobRotation(pickedVob, 1, ztimer->frameTimeFloat * rotSpeed);
					}

					if (keys.KeyPressed("VOB_ROT_FORWARD", false, true))
					{
						HandleVobRotation(pickedVob, 1, -ztimer->frameTimeFloat * rotSpeed);
					}


					if (keys.KeyPressed("VOB_ROT_RIGHT", false, true))
					{
						HandleVobRotation(pickedVob, 2, ztimer->frameTimeFloat * rotSpeed);
					}

					if (keys.KeyPressed("VOB_ROT_LEFT", false, true))
					{
						HandleVobRotation(pickedVob, 2, -ztimer->frameTimeFloat * rotSpeed);
					}
				}

			}


		}

	}
}