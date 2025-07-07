// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	extern Timer mainTimer;
	extern void _GetCursorPos(POINT* cur);

	// THIS FILE CONTAINS MUCH CODE, NOT ALL OF IT IS USED 

	// update waypoint grid when moving WP
	void WayMovePoint(zCVob* wpvob)
	{
		if (!dynamic_cast<zCVobWaypoint*>(wpvob)) return;
		zCWorld* world = ogame->GetWorld(); if (!world) return;
		zCWayNet* waynet = world->GetWayNet(); if (!waynet) return;

		zCWaypoint* wp = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(wpvob));
		if (wp) wp->UpdatePositionWorld();
	}


	bool GetFloorPositionForVobHelper(zCVob* vob, zVEC3& centerPos, zCPolygon*& polyIntersect, bool& foundVob)
	{
		if (!vob->GetHomeWorld()) return FALSE;

		zREAL diff = vob->GetPositionWorld()[VY] - vob->GetBBox3DWorld().mins[VY];
		zCWorld* wld = vob->GetHomeWorld();

		vob->ignoredByTraceRay = true;

		if (wld->TraceRayNearestHit(centerPos, zVEC3(0, -10000, 0), vob, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_POLY_TEST_WATER)) {
			if (wld->traceRayReport.foundPoly || wld->traceRayReport.foundVob) {
				zVEC3 newpos = wld->traceRayReport.foundIntersection;

				if (wld->traceRayReport.foundPoly)
				{
					polyIntersect = wld->traceRayReport.foundPoly;
				}

				foundVob = wld->traceRayReport.foundVob ? foundVob = true : foundVob = false;

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

		zREAL diff = vob->GetPositionWorld()[VY] - vob->GetBBox3DWorld().mins[VY];
		zCWorld* wld = vob->GetHomeWorld();

		if (wld->TraceRayNearestHit(centerPos, zVEC3(0, -5000, 0), vob, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN)) {
			if (wld->traceRayReport.foundPoly || wld->traceRayReport.foundVob) {
				zVEC3 newpos = wld->traceRayReport.foundIntersection;
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

			if (theApp.rotMod == zMH_WORLD)
			{
				pickedVob->RotateWorld(zVEC3(1, 0, 0), angle);
			}
			else if (theApp.rotMod == zMH_LOCALE)
			{
				pickedVob->RotateLocal(zVEC3(1, 0, 0), angle);
			}
			else
			{
				pickedVob->RotateWorld(camUnit.Cross(zVEC3(0, 1, 0)).Normalize(), angle);
			}
		}

		if (type == 2)
		{

			if (theApp.rotMod == zMH_WORLD)
			{
				pickedVob->RotateWorld(zVEC3(0, 0, 1), angle);
			}
			else if (theApp.rotMod == zMH_LOCALE)
			{
				pickedVob->RotateLocal(zVEC3(0, 0, 1), angle);
			}
			else
			{
				zVEC3 newVec = camUnit;
				newVec[1] = 0;

				pickedVob->RotateWorld(newVec, -angle);
			}

		}

		if (type == 3)
		{
			if (theApp.rotMod == zMH_LOCALE)
			{
				pickedVob->RotateLocalY(angle);
			}
			else
			{
				pickedVob->RotateWorldY(angle);
			}
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

		theApp.updateMatrix.OnUpdateMatrix(pickedVob);

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
			if (theApp.rotMod == zMH_WORLD)
			{
				pickedVob->RotateWorld(zVEC3(1, 0, 0), angle);
			}
			else if (theApp.rotMod == zMH_LOCALE)
			{
				pickedVob->RotateLocal(zVEC3(1, 0, 0), angle);
			}
			else
			{
				pickedVob->RotateWorld(camUnit.Cross(zVEC3(0, 1, 0)).Normalize(), angle);
			}
			
		}

		if (type == 2)
		{

			if (theApp.rotMod == zMH_WORLD)
			{
				pickedVob->RotateWorld(zVEC3(0, 0, 1), angle);
			}
			else if (theApp.rotMod == zMH_LOCALE)
			{
				pickedVob->RotateLocal(zVEC3(0, 0, 1), angle);
			}
			else
			{
				zVEC3 newVec = camUnit;
				newVec[1] = 0;

				pickedVob->RotateWorld(newVec, -angle);
			}

			
		}

		if (type == 3)
		{

			if (theApp.rotMod == zMH_LOCALE)
			{
				pickedVob->RotateLocalY(angle);
			}
			else
			{
				pickedVob->RotateWorldY(angle);
			}
			
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

		theApp.updateMatrix.OnUpdateMatrix(pickedVob);

		//collision check for mover
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

		UpdatePfxBbox(pickedVob);


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

	void Tranverse_OnCopyInsert(zCTree<zCVob>* node, zCVob* parent, bool rootVob = false, bool useHierarchy = false, bool parentRootHasChildren = false)
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
					
					if (theApp.turnWpMode == TurnWpMode::RANDOM)
					{
						newVob->RotateLocalY(GetRandVal(0, 360));
					}
					else if (theApp.turnWpMode == TurnWpMode::AGAINST_CAMERA)
					{
						newVob->SetHeadingWorld(pos);
						newVob->RotateLocalY(180);
						newVob->ResetXZRotationsWorld();
					}
					else if (theApp.turnWpMode == TurnWpMode::ON_CAMERA)
					{
						newVob->SetHeadingWorld(pos);
						newVob->ResetXZRotationsWorld();
					}

					
				}
				else if (theApp.options.GetIntVal("vobInsertVobRotRand") && !parentRootHasChildren)
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
			Tranverse_OnCopyInsert(node, newVob, false, useHierarchy, parentRootHasChildren);
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

			if (theApp.globalParent)
			{
				parent = theApp.globalParent;
			}

			// dont' insert global parent inside itself
			if (theApp.vobToCopy == theApp.globalParent)
			{
				parent = NULL;
			}

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
				useHierarchy = false;
			}

			bool parentHasChildren = theApp.vobToCopy->HasChildren();

			theApp.vobToCopy->SetDrawBBox3D(FALSE);

			if (theApp.pickedVob)
			{
				theApp.pickedVob->SetDrawBBox3D(FALSE);
			}


			zCTree<zCVob>* tree = theApp.vobToCopy->globalVobTreeNode;

			

			Tranverse_OnCopyInsert(tree, parent, true, useHierarchy, parentHasChildren);


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


	//vob - The vob whose parent we are changing; newParent - new parent
	void HandleParentChange(zCVob* vob, zCVob* newParent)
	{
		zCWorld* pWorld = ogame->GetWorld();


		if (vob == newParent)
		{
			print.PrintRed(GetLang("CANT_CHANGE_PARENT_INTO_ITSELF"));
			return;
		}

		if (vob->HasChildrenRec(newParent))
		{
			print.PrintRed(GetLang("CANT_CHANGE_PARENT_INTO_ITSELF"));
			return;
		}


		if (newParent && newParent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;

		};

		if (dynamic_cast<zCVobWaypoint*>(vob) 
			|| dynamic_cast<zCVobLevelCompo*>(vob) 
			|| dynamic_cast<oCZoneMusicDefault*>(vob) 
			|| dynamic_cast<zCZoneZFogDefault*>(vob)
			|| dynamic_cast<zCZoneReverbDefault*>(vob)
			|| dynamic_cast<zCZoneVobFarPlaneDefault*>(vob)
			)
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
			// if there is a new parent
			if (newParent)
			{
				pWorld->MoveVobSubtreeTo(vob, newParent->globalVobTreeNode);

				vob->drawBBox3D = false;
				newParent->drawBBox3D = false;
			}
			else
			{
				// otherwise we insert it into a random zCVobLevelCompo
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

	

	zCTexture* GetScreenTex(zSTRING name)
	{
		zCTexConGeneric* texCon = reinterpret_cast<zCTexConGeneric*>(zrenderer->CreateTextureConvert());
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
						<< "]. Âńĺăî: " << pEntry->amoutLoc << "/"
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

	bool CheckIfVobBlocked(zCVob* pVob)
	{
		if (theApp.options.GetIntVal("checkBoxBlockComplexVob"))
		{
			if (pVob->HasChildren())
			{
				print.PrintRed(GetLang("VOB_CHANGE_POS_BLOCKED"));
				return true;
			}
		}

		return false;
	}

	void VobKeys()
	{
		
		if (theApp.camMan.cameraRun || GetSelectedTool() == TM_BBOXEDIT) return;

		theApp.wasCopiedPressed = false;

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
		

		

		if (GetSelectedTool() == TM_NONE)
		{
			SetSelectedTool(TM_TRANSLATE);

			if (theApp.options.GetIntVal("safeOneMode") && theApp.options.GetIntVal("oneModeToggle"))
			{
				SetSelectedTool(TM_ONEMODE);
			}
		}

		zCVob* pickedVob = theApp.GetSelectedVob();

		if (theApp.filterMatWindowActive) 
		{
			return;
		}

		if (keys.KeyPressed("CHANGE_VOB_COMPLEX_MOVE", true))
		{
			

			int value = theApp.options.GetIntVal("checkBoxBlockComplexVob");
			
			value = !value;

			if (value)
			{
				print.PrintRed(GetLang("CHANGE_VOB_COMPLEX_MOVE") + ": " + GetLang("COMMON_TURN_ON"));
			}
			else
			{
				print.PrintGreen(GetLang("CHANGE_VOB_COMPLEX_MOVE") + ": " + GetLang("COMMON_TURN_OFF"));
			}

			theApp.options.SetIntVal("checkBoxBlockComplexVob", value);
			theApp.options.Apply();

		}


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


		if (keys.KeyPressed("CAMERA_SYNC_VOB_MOD", true))
		{

			theApp.cameraMoveWithVobActive = !theApp.cameraMoveWithVobActive;

			if (theApp.cameraMoveWithVobActive)
			{
				print.PrintGreen(GetLang("MOD_CAMERA_SYNC_ON"));
			}
			else
			{
				print.PrintRed(GetLang("MOD_CAMERA_SYNC_OFF"));
			}
			
			
		}

		if (keys.KeyPressed("FAST_FILTER_IGNOREPFX", true))
		{

			if (theApp.filterPickVobIndex == 0)
			{
				theApp.filterPickVobIndex = 13;
			}
			else
			{
				theApp.filterPickVobIndex = 0;
			}

			auto func = (callIntFunc)GetProcAddress(theApp.module, "SetSelectioFilterByIndex");

			func(theApp.filterPickVobIndex);
	
		}
		
		if (keys.KeyPressed("ROT_MOD_CHANGE", true))
		{
			int motion = (int)theApp.rotMod + 1;

			if (motion > 2)
			{
				motion = 0;
			}

			switch (motion)
			{
			case 0: print.PrintRed("WORLD"); break;
			case 1: print.PrintRed("LOCAL"); break;
			case 2: print.PrintRed("CAMERA"); break;
			}

			theApp.rotMod = (zEVobMotionHeading)motion;
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
		

		if (theApp.isGrattControlActive && keys.KeyPressed("VOB_RESET_AXIS", true, true))
		{
			int motion = (int)ControllerEvents.MotionHeading + 1;

			if (motion > 2)
			{
				motion = 0;
			}

			switch (motion)
			{
				case 0: print.PrintRed("WORLD"); break;
				case 1: print.PrintRed("LOCAL"); break;
				case 2: print.PrintRed("CAMERA"); break;
			}

			ControllerEvents.MotionHeading = (zEVobMotionHeading)motion;


		}


		if (keys.KeyPressed("VOB_COPY", true))
		{
			theApp.wasCopiedPressed = true;

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
					//print.PrintRed("Ěŕňĺđčŕë ńęîďčđîâŕí");
				}

			}
			*/

		}

		if (keys.KeyPressed("VOB_RESTORE_POS", true))
		{
			theApp.restorator.RestoreVobPos(theApp.pickedVob);
		}

		if (keys.KeyPressed("VOB_ADD_FAV", true))
		{
			print.PrintRed(GetLang("MSG_VOB_FAV_ADD"));
			theApp.AddVobToFavorite(theApp.pickedVob);
		}



		if (keys.KeyPressed("VOB_SET_ZERO_PARENT", true))
		{
			if (pickedVob)
			{
				if (auto parentVob = pickedVob->GetParentVob())
				{
					if (!parentVob->CastTo<zCVobLevelCompo>())
					{
						print.PrintRed(GetLang("SET_VOB_SET_ZERO_PARENT"));

						auto pos = parentVob->GetPositionWorld();

						HandleVobTranslation(pickedVob, pos);
					}
					else
					{
						print.PrintRed(GetLang("SET_VOB_SET_ZERO_PARENT_CANT"));
					}
					
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
				
				theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
				if (theApp.isVobParentChange)
				{
					HandleParentChange(theApp.vobToCopy, pickedVob);
				}
				else
				{


					if (theApp.vobToCopy == theApp.globalParent && theApp.globalParent != NULL && pickedVob == theApp.globalParent)
					{
						print.PrintRed(GetLang("CANT_COPY_INITSELF"));
						theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
						return;
					}

					if (theApp.vobToCopy && theApp.vobToCopy->HasChildrenRec(pickedVob))
					{
						print.PrintRed(GetLang("CANT_COPY_INITSELF_USUAL_VOB"));
						theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
						return;
					};

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
			if (CheckIfVobBlocked(theApp.pickedVob))
			{
				return;
			}

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
				if (CheckIfVobBlocked(theApp.pickedVob))
				{
					return;
				}

				print.PrintRed(GetLang("TOOL_FLOOR"));
				SetOnFloor(pickedVob);
			}



			if (keys.KeyPressed("VOB_DELETE", true))
			{
				if (pickedVob->HasChildren())
				{
					Stack_PushString("WIN_MSG_CONFIRM_REMOVEVOB");

					(callVoidFunc)GetProcAddress(theApp.module, "GetConfirmWindowYesNo")();

					int yes = Stack_PeekInt();

					if (yes)
					{
						theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
						theApp.RemoveVob(pickedVob);
						theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
					}
				}
				else
				{
					theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
					theApp.RemoveVob(pickedVob);
					theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
				}
			}

		}

	}


	
	void VobMoving()
	{
		if (GetSelectedTool() == TM_BBOXEDIT)
		{
			return;
		}

		
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


			

			


			



			// general manipulations, regardless of the selected tool
			if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
			{
				zVEC3 pos = pickedVob->GetPositionWorld();
				zVEC3 vobUnit = pickedVob->GetAtVectorWorld().Normalize();

				zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				int y = pos.n[1];

				if (keys.KeyPressed("VOB_TRANS_UP", false, true))
				{
					if (CheckIfVobBlocked(theApp.pickedVob))
					{
						return;
					}

					if (pickMode == SWM_VOBS)
					{
						pos.n[1] += speedTranslation * ztimer->frameTimeFloat;
						HandleVobTranslation(pickedVob, pos);

						if (theApp.cameraMoveWithVobActive)
						{
							auto camPos = ogame->GetCameraVob()->GetPositionWorld();
							camPos.n[1] += speedTranslation * ztimer->frameTimeFloat;
							ogame->GetCameraVob()->SetPositionWorld(camPos);
						}
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
					if (CheckIfVobBlocked(theApp.pickedVob))
					{
						return;
					}

					if (pickMode == SWM_VOBS)
					{
						pos.n[1] -= speedTranslation * ztimer->frameTimeFloat;
						HandleVobTranslation(pickedVob, pos);

						if (theApp.cameraMoveWithVobActive)
						{
							auto camPos = ogame->GetCameraVob()->GetPositionWorld();
							camPos.n[1] -= speedTranslation * ztimer->frameTimeFloat;
							ogame->GetCameraVob()->SetPositionWorld(camPos);
						}
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
					if (CheckIfVobBlocked(theApp.pickedVob))
					{
						return;
					}

					HandleVobRotation(pickedVob, 3, ztimer->frameTimeFloat * rotSpeed);
				}

				if (keys.KeyPressed("VOB_ROT_VERT_LEFT", false, true))
				{
					if (CheckIfVobBlocked(theApp.pickedVob))
					{
						return;
					}

					HandleVobRotation(pickedVob, 3, -ztimer->frameTimeFloat * rotSpeed);
				}



				

				static int rotType = 4;

				if (keys.KeyPressed("VOB_RESET_AXIS", true))
				{
					if (CheckIfVobBlocked(theApp.pickedVob))
					{
						return;
					}

					HandleVobRotation(pickedVob, rotType, 0);

					rotType++;

					if (rotType > 6)
					{
						rotType = 4;
					}

				}


				// moving
				if (selectedTool == TM_TRANSLATE || selectedTool == TM_ONEMODE)
				{

					y = pos.n[1];
					zVEC3 unit = camUnit;
					unit[1] = 0;

					//print.PrintRed("1");

					if (keys.KeyPressed("VOB_TRANS_FORWARD", false, true))
					{
						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						pos += unit * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);


						if (theApp.cameraMoveWithVobActive)
						{
							auto camPos = ogame->GetCameraVob()->GetPositionWorld();
							camPos += unit * speedTranslation * ztimer->frameTimeFloat;
							ogame->GetCameraVob()->SetPositionWorld(camPos);
						}
					
					}

					if (keys.KeyPressed("VOB_TRANS_BACKWARD", false, true))
					{
						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						pos -= unit * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);


						if (theApp.cameraMoveWithVobActive)
						{
							auto camPos = ogame->GetCameraVob()->GetPositionWorld();
							camPos -= unit * speedTranslation * ztimer->frameTimeFloat;
							ogame->GetCameraVob()->SetPositionWorld(camPos);
						}

						
					}

					if (keys.KeyPressed("VOB_TRANS_LEFT", false, true))
					{
						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						zVEC3 left = camUnit.Cross(zVEC3(0, 1, 0));
						pos += left * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);

						if (theApp.cameraMoveWithVobActive)
						{
							auto camPos = ogame->GetCameraVob()->GetPositionWorld();
							camPos += left * speedTranslation * ztimer->frameTimeFloat;
							ogame->GetCameraVob()->SetPositionWorld(camPos);
						}

						
					}

					if (keys.KeyPressed("VOB_TRANS_RIGHT", false, true))
					{
						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						zVEC3 right = camUnit.Cross(zVEC3(0, -1, 0));
						pos += right * speedTranslation * ztimer->frameTimeFloat;
						//pos.n[1] = y;
						HandleVobTranslation(pickedVob, pos);


						if (theApp.cameraMoveWithVobActive)
						{
							auto camPos = ogame->GetCameraVob()->GetPositionWorld();
							camPos += right * speedTranslation * ztimer->frameTimeFloat;
							ogame->GetCameraVob()->SetPositionWorld(camPos);
						}
						
					}



				}
				// rotation
				if (selectedTool == TM_ROTATE || selectedTool == TM_ONEMODE)
				{
					//print.PrintRed("2");

					if (keys.KeyPressed("VOB_ROT_BACK", false, true))
					{
						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						HandleVobRotation(pickedVob, 1, ztimer->frameTimeFloat * rotSpeed);
					}

					if (keys.KeyPressed("VOB_ROT_FORWARD", false, true))
					{

						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						HandleVobRotation(pickedVob, 1, -ztimer->frameTimeFloat * rotSpeed);
					}


					if (keys.KeyPressed("VOB_ROT_RIGHT", false, true))
					{

						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						HandleVobRotation(pickedVob, 2, ztimer->frameTimeFloat * rotSpeed);
					}

					if (keys.KeyPressed("VOB_ROT_LEFT", false, true))
					{

						if (CheckIfVobBlocked(theApp.pickedVob))
						{
							return;
						}

						HandleVobRotation(pickedVob, 2, -ztimer->frameTimeFloat * rotSpeed);
					}
				}

			}


		}

	}
}