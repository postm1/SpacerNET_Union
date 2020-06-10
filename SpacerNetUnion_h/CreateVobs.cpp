// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void InsertIntoWorld(zCVob* newVob, zCVob* parentVobBase, bool selectVob)
	{
		OutFile("\n================\nInsertIntoWorld: NodesCount (pre-insert): " + A(ogame->GetWorld()->globalVobTree.CountNodes() - 1), false);


		if (dynamic_cast<zCVobWaypoint*>(newVob))
		{
			ogame->GetGameWorld()->AddVob(newVob);

			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

			if (waynet)
			{
				zCWaypoint* wp = zfactory->CreateWaypoint();
				wp->Init(dynamic_cast<zCVobWaypoint*>(newVob));
				waynet->InsertWaypoint(wp);
				waynet->CorrectHeight();

				OutFile("Add new waypoint: " + AHEX32((uint)newVob), true);
			}


		}
		else if (dynamic_cast<zCVobLevelCompo*>(newVob))
		{
			newVob->SetCollDetDyn(FALSE);
			newVob->SetCollDetStat(FALSE);

			if (newVob->GetVobName().IsEmpty())
			{ 
				int num = GetZenLevelCompoCount();

				newVob->SetVobName("LEVEL-VOB-" + ToStr num);
			}
			ogame->GetGameWorld()->AddVob(newVob);
		}
		else
		{

			zCVobWaypoint* pickedVobWaypont = dynamic_cast<zCVobWaypoint*>(parentVobBase);
			oCItem* pItemParent = dynamic_cast<oCItem*>(parentVobBase);


			// Вставляем в родителя, только если он не Вейпоинт и не итем, иначе ищем compoLevel и он будет родителем нового воба
			if (parentVobBase && !pickedVobWaypont && !pItemParent)
			{
				ogame->GetGameWorld()->AddVobAsChild(newVob, parentVobBase->globalVobTreeNode);
				//std::cout << "Union: Add new vob INTO parent: " << (uint)pickedVob << " Name: " << GetVobName(pickedVob) << std::endl;
				//std::cout << "OnCopy insert in vob: " << newVob->GetVobName() << " from " << (int)pickedVob << std::endl;

				OutFile("InsertIntoWorld: newVob: " + AHEX32((uint)newVob) + " parent: " + A GetVobName(parentVobBase), true);
			}
			else
			{
				zCArray<zCVob*> resultList;
				zCWorld* world = ogame->GetWorld();

				world->SearchVobListByClass(zCVobLevelCompo::classDef, resultList, 0);

				zCVob* parentVob = NULL;

				if (resultList.GetNumInList() > 0)
				{
					parentVob = resultList.GetSafe(rand() % resultList.GetNumInList());
				}
				else
				{
					parentVob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance("zCVobLevelCompo"));
					parentVob->SetCollDetDyn(FALSE);
					parentVob->SetCollDetStat(FALSE);
					parentVob->SetVobName("LEVEL-VOB-0");
					ogame->GetGameWorld()->AddVob(parentVob);
					zRELEASE(parentVob);
				}

				if (parentVob)
				{
					OutFile("InsertIntoWorld: newVob: " + AHEX32((uint)newVob) + " parent zCVobLevelCompo: " + A GetVobName(parentVob), true);

					ogame->GetGameWorld()->AddVobAsChild(newVob, parentVob->globalVobTreeNode);
					//std::cout << "Union: Add new vob with parent compo: " << (uint)parentVob << std::endl;

					/*
					auto updateName = (onUpdateVobName)GetProcAddress(theApp.module, "UpdateVobName");
					Stack_PushString(GetVobName(parentVob));
					updateName((uint)parentVob);
					*/
				}
				else
				{
					std::cout << "Union: no compo parent found: " << std::endl;

					WriteLine("Union: No compoLevel parent was found!");
				}



			}
		}

		OutFile("InsertIntoWorld: NodesCount (after-insert): " + A(ogame->GetWorld()->globalVobTree.CountNodes() - 1) + "\n================", false);
		theApp.selectNextVob = selectVob;

		// убираем превью с экрана
		AddVobToRender("", false);
	}


	void SpacerApp::CreateNewFreepoint(CString classNameChar, CString nameVob, bool autoName, bool ground)
	{
		zSTRING className = classNameChar;
		zSTRING vobName = zSTRING(nameVob);


		zCVob* newvob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance(className));

		if (newvob)
		{
			if (autoName)
			{
				newvob->SetVobName(GetNextFreePointName(vobName));
			}
			else
			{
				newvob->SetVobName(vobName);
			}


			zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
			zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

			newvob->SetPositionWorld(pos + dir * 200);


			InsertIntoWorld(newvob, NULL);


			if (theApp.turnWpMode == TurnWpMode::AGAINST_CAMERA)
			{
				newvob->SetHeadingWorld(pos);
				newvob->RotateLocalY(180);
			}

			if (theApp.turnWpMode == TurnWpMode::ON_CAMERA)
			{
				newvob->SetHeadingWorld(pos);
			}

			newvob->ResetXZRotationsWorld();
			newvob->Release();

			if (ground)
			{
				SetOnFloor(newvob);
			}

			theApp.SetSelectedVob(newvob, "CreateNewVob");
		}

	}

	void SpacerApp::CreateNewVob(CString classNamePtr, CString vobNamePtr, CString visual, int dyn, int stat)
	{
		zSTRING className = zSTRING(classNamePtr);
		zSTRING vobName = zSTRING(vobNamePtr);
		zSTRING visualVob = zSTRING(visual);

		zCClassDef* classDef = zCObject::GetClassDef(classNamePtr);


		if (!classDef || classDef->IsAbstractClass())
		{
			MessageBox(0, GetLang("UNION_CANT_ABSTRACT"), 0, 0);
			return;
		}


		OutFile("CreateNewVob: " + A vobName + " " + A className, true);

		bool flag = nextInsertBlocked;
		nextInsertBlocked = false;
		//std::cout << "Create a vob of the class " << className << std::endl;

		zCVob* newvob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance(className));

		if (newvob)
		{

			OutFile("CreateNewVob: created a new vob with addr: " + AHEX32((uint)newvob), true);

			if (
				dynamic_cast<zCMover*>(newvob) ||
				dynamic_cast<zCVobWaypoint*>(newvob) ||
				dynamic_cast<zCVobSpot*>(newvob)
				)
			{
				if (vobName.Length() == 0)
				{
					MessageBox(0, GetLang("ENTER_NAME"), 0, 0);
					newvob->Release();
					return;
				}
			}

			newvob->SetVobName(vobName);

			zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
			zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


			HandleVobTranslation(newvob, pos + dir * 200);

			if (visualVob.Length() > 0)
			{
				newvob->SetVisual(visualVob);
			}


			if (newvob->GetVisual())
			{
				zCDecal* dec = ((zCDecal*)newvob->GetVisual());

				if (dec)
				{
					//cmd << "VOB ROTATE DECAL" << endl;
					newvob->SetHeadingWorld(pos);
					newvob->RotateLocalY(180);
				}
			}

			newvob->ResetXZRotationsWorld();
			newvob->SetCollDetDyn(dyn);
			newvob->SetCollDetStat(stat);

			InsertIntoWorld(newvob, pickedVob);

			newvob->Release();


			OutFile("CreateNewVob: after insert into world: " + AHEX32((uint)newvob), false);

			//OnCreateVob(newvob);
			theApp.SetSelectedVob(newvob, "CreateNewVob");
		}

		nextInsertBlocked = flag;
		SetSelectedTool(1);

	}

	void SpacerApp::CreatePFX(CString name)
	{
		zSTRING pfxName = zSTRING(name).Upper() + ".PFX";

		zCVob* pVob = new zCVob();
		zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


		pos = pos + dir * 200;
		pVob->SetCollDet(FALSE);
		pVob->SetCollDetStat(FALSE);
		pVob->SetPositionWorld(pos);
		pVob->SetVisual(pfxName);
		//pVob->SetVobName(pfxName);
		InsertIntoWorld(pVob, pickedVob);

		pVob->Release();
		//OnCreateVob(pVob);
		theApp.SetSelectedVob(pVob, "CreatePFX");
	}

	void SpacerApp::CreateItem(CString name)
	{
		zSTRING itemName = zSTRING(name).Upper();

		int instance2 = parser->GetIndex(itemName);

		oCItem* pItem = dynamic_cast<oCItem*>((zCVob*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, instance2));

		if (!pItem) {
			return;
		}


		zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


		/*
		if (theApp.options.GetIntVal("vobInsertItemLevel"))
		{
		zVEC3 levelVec = pos + dir * 150;
		levelVec.n[1] = pos[1];

		pItem->SetPositionWorld(levelVec);
		}
		else
		{
		pItem->SetPositionWorld(pos + dir * 150);
		}
		*/
		pItem->SetPositionWorld(pos + dir * 150);

		//ogame->GetGameWorld()->AddVob(pItem);

		OutFile("CreateItem: newVob: " + AHEX32((uint)pItem), true);


		InsertIntoWorld(pItem, pickedVob);

		pItem->ResetXZRotationsWorld();
		pItem->Release();
		//OnCreateVob(pItem);

		//theApp.SetSelectedVob(pItem, "CreateItem");

	}



	void SpacerApp::OnCreateVob(zCVob* vob, bool select)
	{
		if (vob)
		{
			OutFile("OnCreateVob: vob: " + AHEX32((uint)vob), true);

			auto addNode = (addNewVob)GetProcAddress(theApp.module, "OnVobInsert");

			Stack_PushString(vob->_GetClassDef()->className);
			Stack_PushString(GetVobName(vob));

			addNode((uint)vob, vob->GetParent(), selectedWpForCreateIsBlocked, select);
		}

	}

	void SpacerApp::OnRemoveVob(zCVob* vob)
	{
		if (vob)
		{
			OutFile("OnRemoveVob: vob: " + AHEX32((uint)vob), true);
			auto onRemove = (onVobRemove)GetProcAddress(theApp.module, "OnVobRemove");
			onRemove((uint)vob);
		}

	}

	void SpacerApp::RemoveVob(zCVob* pVob)
	{

		if (zCVobLevelCompo* level = dynamic_cast<zCVobLevelCompo*>(pVob))
		{
			if (level->globalVobTreeNode->GetFirstChild() != NULL)
			{
				print.PrintRed(GetLang("CANT_DELETE_LEVELCOMPO"));
				return;
			}
		}

		if (pVob == ogame->GetCamera()->connectedVob)
		{
			print.PrintRed(GetLang("CANT_DELETE_CAM"));
			return;
		}

		if (pVob == theApp.vobToCopy)
		{
			theApp.vobToCopy = NULL;
			theApp.treeToCopy = NULL;
		}

		theApp.SetSelectedVob(NULL, "DELETE VOB");


		if (pVob)
		{
			pVob->AddRef();
		}

		theApp.pickedWaypoint = NULL;
		theApp.pickedWaypoint2nd = NULL;


		OutFile("RemoveVob: vob: " + AHEX32((uint)pVob), true);


		theApp.OnRemoveVob(pVob);

		zCVobWaypoint* wpvob = dynamic_cast<zCVobWaypoint*>(pVob);
		// Wegpunkt
		if (wpvob)
		{
			zCWayNet* waynet = ogame->GetWorld()->wayNet;
			zCWaypoint* wp = waynet->SearchWaypoint(wpvob);
			if (wp)
			{
				OutFile("RemoveVob: waypoint " + A wp->name + " " + AHEX32((uint)pVob), true);
				waynet->DeleteWaypoint(wp);
			}
		}
		else
		{
			OutFile("RemoveVob: vob " + A GetVobName(pVob) + " " + AHEX32((uint)pVob), true);
			ogame->GetWorld()->RemoveVobSubtree(pVob);
		}

		//print.PrintWin(ToStr "Remove vob refCount: " + ToStr pVob->refCtr, GFX_RED);

		zRELEASE(pVob);

	}



	void SpacerApp::ApplyPhysicsVob()
	{

		zCVob* pVob = pickedVob;
		zCWorld* world = ogame->GetWorld();
		zVEC3 vec = ogame->GetCamera()->GetVob()->GetAtVectorWorld();

		if (pVob && !pVob->staticVob && world)
		{
			zVEC3 cam = ogame->GetCamera()->GetVob()->GetAtVectorWorld();



			if (dynamic_cast<zCVobWaypoint*>(pVob))
			{
				zCWayNet* waynet = world->GetWayNet();
				zCWaypoint* wp = waynet->SearchWaypoint(dynamic_cast<zCVobWaypoint*>(pVob));
				if (wp) wp->CorrectHeight(world);
				return;
			}

			if (!pVob->GetCollDetStat()) return;


			if (pVob && pVob->GetRigidBody())
			{

				// aktuelle Position holen ...
				zPOINT3 oldpos, pos, posneu;
				oldpos = pVob->GetPositionWorld();
				pos = posneu = oldpos;

				// Sleepingmode und Physics entsprechend merken und setzen
				// zTVobSleepingMode	mode	= _vob->GetSleepingMode();
				zBOOL				physics = pVob->GetPhysicsEnabled();
				pVob->SetSleeping(FALSE);
				pVob->SetPhysicsEnabled(TRUE);

				if (zinput->KeyPressed(KEY_LSHIFT))
				{
					vec = vec * 10000;
				}
				else
				{
					vec.n[VX] = 0;
					vec.n[VZ] = 0;
					vec.n[VY] = -300;
				}
				//zerr.Message("B: (Spacer) ApplyForceCM on Vob " + zSTRING(vec.n[VX]) + "/" + zSTRING(vec.n[VY]) + "/" + zSTRING(vec.n[VZ]));
				pVob->GetRigidBody()->ApplyImpulseCM(vec);


				//pVob->SetPhysicsEnabled(physics);
			}
		}
	}
}