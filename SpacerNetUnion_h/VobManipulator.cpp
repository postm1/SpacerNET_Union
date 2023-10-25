// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	extern Timer mainTimer;
	extern void _GetCursorPos(POINT* cur);

	// Add your code here . . .
	// �������� ����� ���������� ��� ����������� WP
	void WayMovePoint(zCVob* wpvob)
	{
		if (!dynamic_cast<zCVobWaypoint*>(wpvob)) return;
		zCWorld* world = ogame->GetWorld(); if (!world) return;
		zCWayNet* waynet = world->GetWayNet(); if (!waynet) return;

		zCWaypoint* wp = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(wpvob));
		// neue koordinaten des Wp
		if (wp) wp->UpdatePositionWorld();
	}


	bool GetFloorPositionForVobHelper(zCVob* vob, zVEC3& centerPos, zCPolygon*& polyIntersect, bool& foundVob)
	{
		if (!vob->GetHomeWorld()) return FALSE;

		// Auf passende H�he setzen
		zREAL diff = vob->GetPositionWorld()[VY] - vob->GetBBox3DWorld().mins[VY];
		zCWorld* wld = vob->GetHomeWorld();

		vob->ignoredByTraceRay = true;

		if (wld->TraceRayNearestHit(centerPos, zVEC3(0, -10000, 0), vob, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_POLY_TEST_WATER)) {
			// Poly gefunden
			if (wld->traceRayReport.foundPoly || wld->traceRayReport.foundVob) {
				// Schnittpunkt schnappen und Position neu setzen
				zVEC3 newpos = wld->traceRayReport.foundIntersection;

				if (wld->traceRayReport.foundPoly)
				{
					polyIntersect = wld->traceRayReport.foundPoly;
				}

				foundVob = wld->traceRayReport.foundVob ? foundVob = true : foundVob = false;

				// 4 cm bis zum Bodem, um Kollision zu vermeiden.
				centerPos = newpos;
				vob->ignoredByTraceRay = false;
				return TRUE;
			}
		}
		vob->ignoredByTraceRay = false;
		return FALSE;
	}

	bool GetFloorPosition(zCVob* vob, zVEC3& centerPos)
	{
		if (!vob->GetHomeWorld()) return FALSE;

		// Auf passende H�he setzen
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

	void HandleVobRotationMatrix(zCVob* pVob, zMAT4 rot)
	{
		if (pVob->globalVobTreeNode)
		{
			GetChildren(pVob->globalVobTreeNode, pVob);
		}

		pVob->SetTrafoObjToWorld(rot);


		for (int i = 0; i < vobsToMove.GetNumInList(); i++)
		{
			vobsToMove.Get(i)->RestoreColl();
		}


		vobsToMove.DeleteListDatas();
	}


	bool IsVobMover(zCVob* pVob)
	{
		bool result = false;

		if (auto pMover = pVob->CastTo<zCMover>())
		{
			result = true;
		}
		else if (auto pParent = pVob->GetParentVob())
		{
			if (auto pMoverParent = pParent->CastTo<zCMover>())
			{
				result = true;
			}
			else if (auto pParent2 = pParent->GetParentVob())
			{
				if (auto pMover2 = pParent2->CastTo<zCMover>())
				{
					result = true;
				}
			}

		}


		return result;
	}


	void HandleVobRotationMover(zCVob* pickedVob, int type, float angle)
	{
		zVEC3 pos = pickedVob->GetPositionWorld();
		zVEC3 vobUnit = pickedVob->GetAtVectorWorld().Normalize();

		zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

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
	}

	void HandleVobRotation(zCVob* pickedVob, int type, float angle)
	{

		if (IsVobMover(pickedVob))
		{
			HandleVobRotationMover(pickedVob, type, angle);
			return;
		}

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


		//�������� �������� ��� ������
		if (IsVobMover(pickedVob))
		{
			pickedVob->SetPositionWorld(pos);
			return;
		}

		if (pickedVob->globalVobTreeNode)
		{
			GetChildren(pickedVob->globalVobTreeNode, pickedVob);

			
		}

		
		pickedVob->SetPositionWorld(pos);


		if (dynamic_cast<zCVobWaypoint*>(pickedVob))
		{
			WayMovePoint(pickedVob);
		}

		if (zCCamTrj_KeyFrame* pKey = dynamic_cast<zCCamTrj_KeyFrame*>(pickedVob))
		{
			if (pKey->cscam)
			{
				pKey->cscam->Refresh();
			}
		}


		for (int i = 0; i < vobsToMove.GetNumInList(); i++)
		{
			vobsToMove.Get(i)->RestoreColl();
		}


		vobsToMove.DeleteListDatas();

		theApp.OnVobMovement(pickedVob);
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
				//print.PrintRed("�� ���� �������� ��� ������ � ����!");
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
			//theApp.SelectObject(baseSelectVob);
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


	//vob - ���, �������� ������ ��������; newParent - ��� ����� ��������
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
			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
			// ���� ���� ����� ��������
			if (newParent)
			{
				pWorld->MoveVobSubtreeTo(vob, newParent->globalVobTreeNode);

				vob->drawBBox3D = false;
				newParent->drawBBox3D = false;
			}
			else
			{
				// ����� ��������� � ��������� zCVobLevelCompo
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


			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
		}
	}

	SpacerToolMode selectedTool = TM_NONE;

	void SetSelectedTool(SpacerToolMode tool)
	{
		//cmd << "SetSelectedTool: " << (int)tool << endl;

		selectedTool = tool;

		if (theApp.isGrattControlActive)
		{
			ControllerEvents.MotionMode = (zEVobMotionMode)tool;
		}
	}

	SpacerToolMode GetSelectedTool()
	{
		return selectedTool;
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

		auto bspTree = ogame->GetWorld()->bspTree;

		bspTree.mesh->CreateListsFromArrays();

		for (int i = 0; i<bspTree.mesh->numPoly; i++) {
			zCPolygon *poly = bspTree.mesh->Poly(i);
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
			cmd << ("����� PFX: " + Z pList.GetNumInList()) << endl;

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


	struct EntryFoundItem
	{
		zSTRING name;
		int countLoc;
		int countCont;

		int amountAll;

		int amoutLoc;
		int amountCont;

		EntryFoundItem::EntryFoundItem(zSTRING name)
		{
			this->name = name.Upper();
			this->countLoc = 0;
			this->countCont = 0;
			this->amountAll = 0;

			this->amoutLoc = 0;
			this->amountCont = 0;
		}
	};

	struct FoundVobStruct {

		zCArray<EntryFoundItem*> itemsLoc;

		void Clear()
		{
			ZeroMemory(&vobFoundStruct, sizeof vobFoundStruct);

			itemsLoc.DeleteListDatas();

			
			itemsLoc.Insert(new EntryFoundItem("ItMi_Gold"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_OldCoin"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_HolderGoldCandle"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_NecklaceGold"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_SilverRing"));

			itemsLoc.Insert(new EntryFoundItem("ItMi_SilverCup"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_SilverPlate"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_PlateGold"));

			itemsLoc.Insert(new EntryFoundItem("ItMi_CupGold"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_RingGold"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_SilverChalice"));


			itemsLoc.Insert(new EntryFoundItem("ItMi_ChaliceGold"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_ChestGold"));
			itemsLoc.Insert(new EntryFoundItem("ItMi_JeweleryChest"));



			/*
			itemsLoc.Insert(new EntryFoundItem("ITPO_PERM_HEALTH"));
			itemsLoc.Insert(new EntryFoundItem("ITPO_PERM_MANA"));
			itemsLoc.Insert(new EntryFoundItem("ItPo_Perm_STR"));
			itemsLoc.Insert(new EntryFoundItem("ItPo_Perm_DEX"));
			itemsLoc.Insert(new EntryFoundItem("ItPo_Perm_Stamina"));


			itemsLoc.Insert(new EntryFoundItem("ItPl_Perm_Herb"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Strength_Herb_01"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Dex_Herb_01"));
			itemsLoc.Insert(new EntryFoundItem("ITPL_SUPER_HERB"));
			itemsLoc.Insert(new EntryFoundItem("itpl_xagitta_herb_mis"));

			itemsLoc.Insert(new EntryFoundItem("ItPl_Planeberry"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Forestberry"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Health_Herb_03"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Mana_Herb_03"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Blueplant"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Mushroom_01"));
			itemsLoc.Insert(new EntryFoundItem("ItPl_Mushroom_02"));

			itemsLoc.Insert(new EntryFoundItem("ITFO_BREAD"));
			itemsLoc.Insert(new EntryFoundItem("ITFO_CHEESE"));
			itemsLoc.Insert(new EntryFoundItem("ITFO_APPLE"));
			itemsLoc.Insert(new EntryFoundItem("ITFO_MILK"));
			itemsLoc.Insert(new EntryFoundItem("ItFo_Honey"));


			itemsLoc.Insert(new EntryFoundItem("ItWr_StonePlateCommon_Addon"));
			itemsLoc.Insert(new EntryFoundItem("itwr_staminapointstoneplate"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_HitPointStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_StrStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_DexStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_OneHStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_TwoHStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_BowStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_CrsBowStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_ManaStonePlate1_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_IntStonePlate_Addon"));

			itemsLoc.Insert(new EntryFoundItem("itwr_druidplate_00"));
			itemsLoc.Insert(new EntryFoundItem("itwr_druidplate_01"));
			itemsLoc.Insert(new EntryFoundItem("itwr_druidplate_02"));
			


			itemsLoc.Insert(new EntryFoundItem("ItWr_StrStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_StrStonePlate3_Addon"));

			itemsLoc.Insert(new EntryFoundItem("ItWr_DexStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_DexStonePlate3_Addon"));

			itemsLoc.Insert(new EntryFoundItem("ItWr_HitPointStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_HitPointStonePlate3_Addon"));

			itemsLoc.Insert(new EntryFoundItem("ItWr_ManaStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_ManaStonePlate3_Addon"));


			itemsLoc.Insert(new EntryFoundItem("ItWr_OneHStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_OneHStonePlate3_Addon"));



			itemsLoc.Insert(new EntryFoundItem("ItWr_TwoHStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_TwoHStonePlate3_Addon"));


			itemsLoc.Insert(new EntryFoundItem("ItWr_BowStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_BowStonePlate3_Addon"));

			itemsLoc.Insert(new EntryFoundItem("ItWr_CrsBowStonePlate2_Addon"));
			itemsLoc.Insert(new EntryFoundItem("ItWr_CrsBowStonePlate3_Addon"));
			*/
		}

		void IncreaseByName(int num, zSTRING name, int type)
		{
			for (int i = 0; i < itemsLoc.GetNumInList(); i++)
			{
				auto pEntry = itemsLoc.GetSafe(i);

				if (pEntry && pEntry->name == name)
				{
					pEntry->countCont += 1;
					pEntry->amountAll += num;
					pEntry->amountCont += num;
				}
			}
		}

		void IncreaseByName(oCItem* pVob, zSTRING name, int type)
		{

			for (int i = 0; i < itemsLoc.GetNumInList(); i++)
			{
				auto pEntry = itemsLoc.GetSafe(i);

				if (pEntry && pEntry->name == name)
				{
					pEntry->countLoc += 1;
					pEntry->amountAll += pVob->amount;
					pEntry->amoutLoc += pVob->amount;

				}
			}

		}

		void Print()
		{

			cmd << ">Start<" << endl;

			for (int i = 0; i < itemsLoc.GetNumInList(); i++)
			{
				auto pEntry = itemsLoc.GetSafe(i);

				if (pEntry)
				{
					//cmd << pEntry->name << ", " << pEntry->countLoc << "/" << pEntry->countCont << endl;

					if (pEntry->countLoc == 0 && pEntry->countCont == 0)
					{
						//cmd << endl;
						continue;
					}

					cmd << pEntry->name << ", [" << pEntry->countLoc << "/" << pEntry->countCont 
						<< "]. �����: " << pEntry->amoutLoc << "/"
						<< pEntry->amountCont
						<< " (" << pEntry->amountAll << ")"
						<< endl;
				}
			}

			cmd << ">End<" << endl;
		}
	} vobFoundStruct;

	void FindStatsVobs()
	{
		cmd << "============================ Looking for vobs ============================" << endl;

		zCArray<zCVob*> result;
		zCArray<oCItem*> foundItems;
		zCArray<oCMobContainer*> foundChests;

		vobFoundStruct.Clear();

		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, result, 0);


		int num = result.GetNumInList();

		for (int i = 0; i < num; i++)
		{
			auto pVob = result.GetSafe(i);

			if (pVob)
			{
				if (auto pItem = pVob->CastTo<oCItem>())
				{
					foundItems.InsertEnd(pItem);
				}

				if (auto pContainer = dynamic_cast<oCMobContainer*>(pVob))
				{
					foundChests.InsertEnd(pContainer);
				}
			}
			
		}


		cmd << "Found Items: " << foundItems.GetNumInList() << endl;
		cmd << "Found Chests: " << foundChests.GetNumInList() << endl;
		cmd << endl;

		for (int i = 0; i < foundItems.GetNumInList(); i++)
		{
			auto pVob = foundItems.GetSafe(i);

			if (pVob)
			{
				vobFoundStruct.IncreaseByName(pVob, pVob->GetInstanceName(), 0);

			}
		}

		for (int i = 0; i < foundChests.GetNumInList(); i++)
		{
			auto pVob = foundChests.GetSafe(i);

			if (pVob)
			{
				zSTRING s = pVob->contains.Upper();

				zSTRING info;
				int wordnr = 1;
				// Get Instance Name
				do {
					info = s.PickWord(wordnr, ",", "\r\t ");
					if (!info.IsEmpty()) {
						// Create these Objects
						zSTRING name = info.PickWord(1, ":", "\r\t ");
						int num = info.PickWord(3, ":", "\r\t ").ToInt();
						if (num <= 0) num = 1;

						int index = parser->GetIndex(name);
						if (index >= 0)
						{
							vobFoundStruct.IncreaseByName(num, name, 1);
						}
					}
					wordnr += 2;
				} while (!info.IsEmpty());

			}
		}


		vobFoundStruct.Print();
	}

	void VobKeys()
	{
		
		if (theApp.camMan.cameraRun || GetSelectedTool() == TM_BBOXEDIT) return;

		theApp.TestLoop();

		auto pickMode = theApp.GetPickMode();


		if (pickMode == SWM_GRASS)
		{
			if (theApp.GetSelectedVob())
			{
				theApp.SetSelectedVob(NULL);

			}

			theApp.gp.DoPlace();

			return;
		}

		if (pickMode == SWM_MULTISELECT && !theApp.isGrattControlActive)
		{
			if (theApp.GetSelectedVob())
			{
				theApp.SetSelectedVob(NULL);
			}

			Selector.ManageKeys();

			return;
		
		}

		if (pickMode != SWM_MULTISELECT)
		{
			Selector.ClearSelection();
		}
		

	
		//SearchBadHierarchy();
		

		if (GetSelectedTool() == TM_NONE)
		{
			SetSelectedTool(TM_TRANSLATE);

			if (theApp.options.GetIntVal("safeOneMode") && theApp.options.GetIntVal("oneModeToggle"))
			{
				SetSelectedTool(TM_ONEMODE);
			}
		}

		zCVob* pickedVob = theApp.GetSelectedVob();

		

		if (keys.KeyPressed("VOB_TRANSLATE", true))
		{
			print.PrintRed(GetLang("TOOL_TRANS"));
			SetSelectedTool(TM_TRANSLATE);
		}

		if (keys.KeyPressed("VOB_ROTATE", true))
		{
			print.PrintRed(GetLang("TOOL_ROT"));
			SetSelectedTool(TM_ROTATE);
		}

		if (keys.KeyPressed("VOB_ONEMODE", true))
		{
			if (selectedTool != TM_ONEMODE)
			{
				print.PrintRed(GetLang("VOB_ONEMODE"));
				SetSelectedTool(TM_ONEMODE);
				theApp.options.SetIntVal("oneModeToggle", TRUE);
			}
			else
			{
				print.PrintRed(GetLang("VOB_ONEMODE_OFF"));
				SetSelectedTool(TM_TRANSLATE);
				theApp.options.SetIntVal("oneModeToggle", FALSE);
			}
			
		}
		

		if (pickMode == SWM_MATERIALS)
		{
			if (!theApp.pickUnshareShow)
			{
				theApp.pickUnshareShow = true;

				//print.PrintRed("Unshare features of polygons");

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


				parser->SetScriptInt("kapitel", 1);
				parser->SetScriptInt("RX_NewMonsterSpawn", 1);
				parser->SetScriptInt("RX_IsInterUpdate", 1);
				
				parser->CallFunc(parser->GetIndex("startup_newworld"));
				parser->CallFunc(parser->GetIndex("init_newworld"));
				parser->CallFunc(parser->GetIndex("b_enter_newworld"));
				parser->CallFunc(parser->GetIndex("RX_NewLegSpawn"));
				parser->CallFunc(parser->GetIndex("xardas_firstlock"));
				
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
					if (dynamic_cast<zCCSCamera*>(theApp.pickedVob) || dynamic_cast<zCCamTrj_KeyFrame*>(theApp.pickedVob))
					{
						print.PrintRed(GetLang("MSG_VOB_CANT_BE_COPIED"));
						return;
					}
					else
					{
						print.PrintRed(GetLang("VOB_COPY_OK"));
						theApp.vobToCopy = theApp.pickedVob;
						theApp.isVobParentChange = false;
					}
					
				}
			}
			/*
			else if (pickMode == SWM_MATERIALS)
			{
				if (mm.selPolyList && mm.selPolyList->GetNumInList() > 0)
				{
					mm.CopyTextureName();
					//mm.copyMat = mm.selPolyList->Get(0)->GetPolygon()->material;
					//print.PrintRed("�������� ����������");
				}

			}
			*/

		}

		if (keys.KeyPressed("VOB_RESTORE_POS", true))
		{
			theApp.restorator.RestoreVobPos(theApp.pickedVob);
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
				
				theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
				if (theApp.isVobParentChange)
				{
					HandleParentChange(theApp.vobToCopy, pickedVob);
				}
				else
				{


					if (theApp.vobToCopy == theApp.globalParent && theApp.globalParent != NULL)
					{
						print.PrintRed(GetLang("CANT_COPY_INITSELF"));
						return;
					}


					if (theApp.options.GetIntVal("selectMoveWhenVobInsert"))
					{
						if (!theApp.isGrattControlActive && GetSelectedTool() != TM_ONEMODE)
							SetSelectedTool(TM_TRANSLATE);
					}

					HandleInsertVobCopy(pickedVob);
				}

				theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);

				if (auto selVob = theApp.GetSelectedVob())
				{

					auto onSelect = (onSelectNode)GetProcAddress(theApp.module, "Export_SelectNodeByPtr");
					onSelect((uint32)selVob);
					
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
			if (!theApp.isGrattControlActive && GetSelectedTool() != TM_ONEMODE)
				SetSelectedTool(TM_TRANSLATE);

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

			mm.CleanSelection();
			(callVoidFunc)GetProcAddress(theApp.module, "CleanPropWindow")();
			//(callVoidFunc)GetProcAddress(theApp.module, "ClearSelectedObjectInObjTree")();
			
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
				theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
				theApp.RemoveVob(pickedVob);
				theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
			}

		}

	}


	
	void VobMoving()
	{
		if (GetSelectedTool() == TM_BBOXEDIT)
		{
			return;
		}



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
			print.PrintRed("����� �������� ����");

		}
		*/
		
		if (GetSelectedTool() == TM_NONE)
		{
			//cmd << "tool is none, set tranlation" << endl;

			SetSelectedTool(TM_TRANSLATE);

			if (theApp.options.GetIntVal("safeOneMode") && theApp.options.GetIntVal("oneModeToggle"))
			{
				SetSelectedTool(TM_ONEMODE);
			}
		}

		zCVob* pickedVob = theApp.GetSelectedVob();

		auto pickMode = theApp.GetPickMode();


		if (!zinput->GetMouseButtonPressedRight() && pickMode == SWM_MATERIALS)
		{
			/*

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
			*/
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


			

			


			



			// ����� �����������, ��� ����������� �� ���������� �����������
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
					/*
					else if (pickMode == SWM_MATERIALS)
					{
						mm.TextureScale(10, 10);
					}
					*/
					
				}

				if (keys.KeyPressed("VOB_TRANS_DOWN", false, true))
				{
					if (pickMode == SWM_VOBS)
					{
						pos.n[1] -= speedTranslation * ztimer->frameTimeFloat;
						HandleVobTranslation(pickedVob, pos);
					}
					/*
					else if (pickMode == SWM_MATERIALS)
					{
						mm.TextureScale(-10, -10);
					}
					*/
					
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


				// �����������
				if (selectedTool == TM_TRANSLATE || selectedTool == TM_ONEMODE)
				{

					y = pos.n[1];
					zVEC3 unit = camUnit;
					unit[1] = 0;

					//print.PrintRed("1");

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
				// ��������
				if (selectedTool == TM_ROTATE || selectedTool == TM_ONEMODE)
				{
					//print.PrintRed("2");

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