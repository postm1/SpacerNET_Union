// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
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

		if (wld->TraceRayNearestHit(centerPos, zVEC3(0, -1000, 0), vob, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN)) {
			// Poly gefunden
			if (wld->traceRayReport.foundPoly || wld->traceRayReport.foundVob) {
				// Schnittpunkt schnappen und Position neu setzen
				zVEC3 newpos = wld->traceRayReport.foundIntersection;
				// 4 cm bis zum Bodem, um Kollision zu vermeiden.
				newpos[VY] += diff + 4;
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
						zVEC3 levelVec = pos + dir * 200;
						levelVec.n[1] = theApp.vobToCopy->GetPositionWorld().n[1];

						HandleVobTranslation(newVob, levelVec);
					}
					else
					{
						HandleVobTranslation(newVob, pos + dir * 200);
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

					if (theApp.turnWpMode == TurnWpMode::AGAINST_CAMERA)
					{
						newVob->SetHeadingWorld(pos);
						newVob->RotateLocalY(180);
					}

					if (theApp.turnWpMode == TurnWpMode::ON_CAMERA)
					{
						newVob->SetHeadingWorld(pos);
					}
				}
				else if (theApp.options.GetIntVal("vobInsertVobRotRand"))
				{
					newVob->RotateLocalY(GetRandVal(0, 360));
				}

				newVob->ResetXZRotationsWorld();

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


			bool useHierarchy = theApp.options.GetIntVal("vobInsertHierarchy");

			zCVobWaypoint* pickedVobWaypont = dynamic_cast<zCVobWaypoint*>(theApp.vobToCopy);
			oCItem* pItem = dynamic_cast<oCItem*>(theApp.vobToCopy);

			checkCopyVobSelf = false;

			CheckVobs(theApp.vobToCopy->globalVobTreeNode);

			if (useHierarchy && (theApp.vobToCopy == theApp.pickedVob || checkCopyVobSelf) && !pItem && !pickedVobWaypont)
			{
				//print.PrintRed("Ќе могу вставить воб самого в себ€!");
				//return;
				useHierarchy = false;
			}



			theApp.vobToCopy->SetDrawBBox3D(FALSE);

			if (theApp.pickedVob)
			{
				theApp.pickedVob->SetDrawBBox3D(FALSE);
			}


			zCTree<zCVob>* tree = theApp.vobToCopy->globalVobTreeNode;



			GetNextChildCopy(tree, theApp.GetSelectedVob(), true, useHierarchy);


			theApp.SetSelectedVob(baseSelectVob, "Control+V");
			auto onSelect = (onSelectNode)GetProcAddress(theApp.module, "SelectNode");
			onSelect((uint32)theApp.pickedVob);
			print.PrintGreen(GetLang("UNION_VOB_INSERTED"));
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


	//vob - ¬об, которому мен€ем родител€; newParent - воб новый родитель
	void HandleParentChange(zCVob* vob, zCVob* newParent)
	{
		zCWorld* pWorld = ogame->GetWorld();
		if (dynamic_cast<zCVobWaypoint*>(vob) || dynamic_cast<zCVobLevelCompo*>(vob) || dynamic_cast<zCZone*>(vob))
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
				// иначе вставл€ем в случайный zCVobLevelCompo
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
	}

	void VobMoving()
	{


		if (!selectedTool)
		{
			SetSelectedTool(1);
		}

		zCVob* pickedVob = theApp.GetSelectedVob();


		if (keys.KeyPressed("VOB_COPY", true))
		{
			print.PrintRed(GetLang("VOB_COPY_OK"));
			theApp.vobToCopy = theApp.pickedVob;
			theApp.isVobParentChange = false;
		}

		if (keys.KeyPressed("VOB_CUT", true))
		{
			print.PrintRed(GetLang("VOB_CUT_OK"));
			theApp.vobToCopy = theApp.pickedVob;
			theApp.isVobParentChange = true;

		}

		if (keys.KeyPressed("VOB_INSERT", true))
		{

			SetSelectedTool(1);

			if (theApp.isVobParentChange)
			{
				HandleParentChange(theApp.vobToCopy, pickedVob);
			}
			else
			{
				HandleInsertVobCopy(pickedVob);
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
			SetSelectedTool(1);

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

			if (keys.KeyPressed("WP_TOGGLE", true))
			{
				theApp.ToggleWP();
			}


			if (keys.KeyPressed("VOB_DISABLE_SELECT", true))
			{

				if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
				if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

				theApp.SetSelectedVob(NULL);
				print.PrintRed(GetLang("TOOL_UNSELECT"));
				(callVoidFunc)GetProcAddress(theApp.module, "CleanPropWindow")();
			}



			// общие манипул€ции, вне зависимости от выбранного инструмента
			if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
			{
				zVEC3 pos = pickedVob->GetPositionWorld();
				zVEC3 vobUnit = pickedVob->GetAtVectorWorld().Normalize();

				zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				int y = pos.n[1];

				if (keys.KeyPressed("VOB_TRANS_UP", false, true))
				{
					pos.n[1] += speedTranslation * ztimer->frameTimeFloat;
					HandleVobTranslation(pickedVob, pos);
				}

				if (keys.KeyPressed("VOB_TRANS_DOWN", false, true))
				{
					pos.n[1] -= speedTranslation * ztimer->frameTimeFloat;
					HandleVobTranslation(pickedVob, pos);
				}


				if (keys.KeyPressed("VOB_ROT_VERT_RIGHT", false, true))
				{
					HandleVobRotation(pickedVob, 3, ztimer->frameTimeFloat * rotSpeed);
				}

				if (keys.KeyPressed("VOB_ROT_VERT_LEFT", false, true))
				{
					HandleVobRotation(pickedVob, 3, -ztimer->frameTimeFloat * rotSpeed);
				}



				if (keys.KeyPressed("VOB_FLOOR", true))
				{
					print.PrintRed(GetLang("TOOL_FLOOR"));
					SetOnFloor(pickedVob);
				}



				if (keys.KeyPressed("VOB_DELETE"))
				{
					theApp.RemoveVob(pickedVob);
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