// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void InsertIntoWorld(zCVob* newVob, zCVob* parentVobBase, bool selectVob)
	{
		//OutFile("\n================\nInsertIntoWorld: NodesCount (pre-insert): " + A(ogame->GetWorld()->globalVobTree.CountNodes() - 1), false);


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

				//OutFile("Add new waypoint: " + AHEX32((uint)newVob), true);
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

				//OutFile("InsertIntoWorld: newVob: " + AHEX32((uint)newVob) + " parent: " + A GetVobName(parentVobBase), true);
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

		if (auto vobLight = newVob->CastTo<zCVobLight>())
		{
			vobLight->SetCollDet(FALSE);
			vobLight->SetSleeping(TRUE);
			vobLight->SetPhysicsEnabled(FALSE);

			vobLight->lightData.m_bCanMove = true;
			vobLight->SetPositionWorld(vobLight->GetPositionWorld());
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


		auto parent = theApp.GetSelectedVob();

		if (theApp.globalParent) parent = theApp.globalParent;

		if (parent && parent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;
		};



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


			InsertIntoWorld(newvob, parent);


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


	bool SpacerApp::CheckForUniqVobs(CString className)
	{
		bool foundUniqVob = false;


		//cmd << className << endl;
		if (className == "zCZoneVobFarPlaneDefault")
		{
			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();
			world->SearchVobListByClass(zCZoneVobFarPlaneDefault::classDef, resultList, 0);

			if (resultList.GetNumInList() > 0)
				foundUniqVob = true;
		}
		else if (className == "zCZoneZFogDefault")
		{
			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();
			world->SearchVobListByClass(zCZoneZFogDefault::classDef, resultList, 0);

			if (resultList.GetNumInList() > 0)
				foundUniqVob = true;
		}
		else if (className == "zCZoneReverbDefault")
		{
			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();
			world->SearchVobListByClass(zCZoneReverbDefault::classDef, resultList, 0);

			if (resultList.GetNumInList() > 0)
				foundUniqVob = true;
		}
		else if (className == "oCZoneMusicDefault")
		{
			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();
			world->SearchVobListByClass(oCZoneMusicDefault::classDef, resultList, 0);

			if (resultList.GetNumInList() > 0)
				foundUniqVob = true;
		}
		else if (className == "zCVobStartpoint")
		{
			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();
			world->SearchVobListByClass(zCVobStartpoint::classDef, resultList, 0);

			if (resultList.GetNumInList() > 0)
				foundUniqVob = true;
		}

		return foundUniqVob;
	}

	void SpacerApp::CreateNewVob(CString classNamePtr, CString vobNamePtr, CString visual, int dyn, int stat)
	{
		zSTRING className = zSTRING(classNamePtr);
		zSTRING vobName = zSTRING(vobNamePtr);
		zSTRING visualVob = zSTRING(visual);

		zCClassDef* classDef = zCObject::GetClassDef(classNamePtr);

		auto parent = theApp.GetSelectedVob();

		if (theApp.globalParent) parent = theApp.globalParent;

		if (parent && parent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;
		};

		if (parent && (dynamic_cast<zCZone*>(parent)))
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;
		};



		if (!classDef || classDef->IsAbstractClass())
		{

			print.PrintRed(GetLang("UNION_CANT_ABSTRACT"));

			return;
		}

		

		if (CheckForUniqVobs(className))
		{
			print.PrintRed(GetLang("OnlyOneVobCanBe"));
			return;
		}

		
			
		bool flag = nextInsertBlocked;
		nextInsertBlocked = false;
		//std::cout << "Create a vob of the class " << className << std::endl;

		zCVob* newvob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance(className));

		if (newvob)
		{

			OutFile("CreateNewVob: created a new vob with addr: " + AHEX32((uint)newvob), true);


			if (visual.Length() > 0 && dynamic_cast<zCZone*>(newvob))
			{
				//print.PrintRed(GetLang("NoVisualForSuchVob"));
				visual = "";
			}



			if (
				dynamic_cast<zCMover*>(newvob) ||
				dynamic_cast<zCVobWaypoint*>(newvob) ||
				dynamic_cast<zCVobSpot*>(newvob)
				)
			{
				if (vobName.Length() == 0)
				{
					print.PrintRed(GetLang("ENTER_NAME"));

					newvob->Release();
					return;
				}
			}


			auto vobSound = dynamic_cast<zCVobSound*>(newvob);

			if (
				dynamic_cast<zCZone*>(newvob)
				&& !vobSound
				)
			{
				stat = 0;
				dyn = 0;
				parent = NULL;
			}

			if (vobSound || dynamic_cast<zCVobStartpoint*>(newvob) || dynamic_cast<zCVobLight*>(newvob))
			{
				stat = 0;
				dyn = 0;
			}





			newvob->SetVobName(vobName);

			zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
			zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


			HandleVobTranslation(newvob, pos + dir * 250);

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

			//rx_light
			if (auto vobLight = newvob->CastTo<zCVobLight>())
			{
				newvob->SetCollDet(FALSE);
				newvob->SetSleeping(TRUE);
				newvob->SetPhysicsEnabled(FALSE);

				vobLight->lightData.m_bCanMove = true;
				vobLight->SetPositionWorld(vobLight->GetPositionWorld());
			}

			newvob->ResetXZRotationsWorld();
			newvob->SetCollDetDyn(dyn);
			newvob->SetCollDetStat(stat);
			
		

			theApp.selectNextVobForce = true;

			InsertIntoWorld(newvob, parent);

			newvob->Release();


			OutFile("CreateNewVob: after insert into world: " + AHEX32((uint)newvob), false);

		
			theApp.SetSelectedVob(newvob, "CreateNewVob");
		}
		else
		{
			cmd << "Creation Vob fail!!!!!!" << endl;
		}

		nextInsertBlocked = flag;

		if (!theApp.isGrattControlActive)
			SetSelectedTool(1);

	}

	void SpacerApp::CreatePFX(CString name)
	{
		zSTRING pfxName = zSTRING(name).Upper() + ".PFX";


		auto parent = theApp.GetSelectedVob();

		if (theApp.globalParent) parent = theApp.globalParent;

		if (parent && parent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;
		};


		zCVob* pVob = new zCVob();
		zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


		pos = pos + dir * 200;
		pVob->SetCollDet(FALSE);
		pVob->SetCollDetStat(FALSE);
		pVob->SetPositionWorld(pos);
		pVob->SetVisual(pfxName);
		//pVob->SetVobName(pfxName);
		InsertIntoWorld(pVob, parent);

		pVob->Release();
		
		theApp.SetSelectedVob(pVob, "CreatePFX");
	}

	oCItem* SpacerApp::CreateItem(CString name)
	{

		auto parent = theApp.GetSelectedVob();

		if (theApp.globalParent) parent = theApp.globalParent;

		if (parent && parent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return NULL;
		};

		zSTRING itemName = zSTRING(name).Upper();

		int instance2 = parser->GetIndex(itemName);

		oCItem* pItem = dynamic_cast<oCItem*>((zCVob*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, instance2));

		if (!pItem) {
			return NULL;
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


		InsertIntoWorld(pItem, parent);

		pItem->ResetXZRotationsWorld();
		pItem->Release();
		
		return pItem;

		//theApp.SetSelectedVob(pItem, "CreateItem");

	}



	void SpacerApp::OnCreateVob(zCVob* vob, bool select)
	{
		if (vob)
		{
			//OutFile("OnCreateVob: vob: " + Z AHEX32((uint)vob) + Z " select: " + Z select, true);

			static auto addNode = (addNewVob)GetProcAddress(theApp.module, "OnVobInsert");

			nextInsertionIsTempPfx = false;

			Stack_PushString(vob->_GetClassDef()->className);
			Stack_PushString(GetVobName(vob));



			if (vob->GetParent() == 0)
			{
				zSTRING name = GetVobName(vob);

				if (name.Lower().Contains(".pfx"))
				{
					nextInsertionIsTempPfx = true;
				}
			}

			theApp.restorator.AddNewVobPos(vob);

			addNode((uint)vob, vob->GetParent(), selectedWpForCreateIsBlocked, (int)select);
		}

	}

	//rx_remove rx_onremove
	void SpacerApp::OnRemoveVob(zCVob* vob)
	{
		if (vob)
		{
			//OutFile("OnRemoveVob: vob: " + AHEX32((uint)vob), true);
			static auto onRemove = (onVobRemove)GetProcAddress(theApp.module, "OnVobRemove");
			onRemove((uint)vob);

			if (auto pCam = dynamic_cast<zCCSCamera*>(vob))
			{
				//cmd << "onRemove zCCSCamera clear" << endl;
				(callVoidFunc)GetProcAddress(theApp.module, "OnCameraClear_Interface")();
			}
			else if (auto pKey = dynamic_cast<zCCamTrj_KeyFrame*>(vob))
			{
				if (pKey->cscam)
				{
					int index = -1;
					
					if (pKey->type == KF_CAM)
					{
						index = pKey->cscam->SearchCamKey(pKey);

						camMan.blockUpdateCamWindow = true;
						Stack_PushInt(index);
						(callVoidFunc)GetProcAddress(theApp.module, "OnRemoveSplineKey_Interface")();

						camMan.blockUpdateCamWindow = false;
					}
					else
					{
						index = pKey->cscam->SearchTargetKey(pKey);
						//cmd << "OnRemove key index: " << index << endl;
						camMan.blockUpdateCamWindow = true;
						Stack_PushInt(index);
						(callVoidFunc)GetProcAddress(theApp.module, "OnRemoveTargetKey_Interface")();

						camMan.blockUpdateCamWindow = false;
					}
					

					pKey->cscam->Refresh();


					
				}
				
			}

			if (globalParent == vob)
			{
				static auto onRemoveGlobalPar = (callVoidFunc)GetProcAddress(theApp.module, "OnRemoveGlobalParent");
				onRemoveGlobalPar();
			}
		}

	}

	void SpacerApp::MakeGlobalParent(zCVob* pVob)
	{
		if (pVob && pVob != ogame->GetCamera()->connectedVob && !pVob->IsPFX())
		{
			globalParent = pVob;
		}
	}

	zCArray<zCVob*> changeParentList;

	void GetVisibleVobsChildren(zCTree<zCVob>* node, zCVob* pVob)
	{
		zCVob* vob = node->GetData();

		if (vob)
		{
			changeParentList.Insert(vob);
		}

		node = node->GetFirstChild();

		while (node != NULL)
		{
			GetVisibleVobsChildren(node, pVob);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::Extern_RemoveAsParent(zCVob* pVob)
	{
		if (pVob && pVob != ogame->GetCamera()->connectedVob)
		{

			zCVob* parentSafe = pVob;

			changeParentList.DeleteList();

			if (pVob->globalVobTreeNode)
			{
				GetVisibleVobsChildren(pVob->globalVobTreeNode, pVob);

			}

			//cmd << "changeParentList: " << changeParentList.GetNum() << endl;

			for (int i = 0; i < changeParentList.GetNum(); i++)
			{
				auto curVob = changeParentList.GetSafe(i);

				if (curVob && curVob != pVob)
				{
					HandleParentChange(curVob, NULL);
				}
			}

			if (parentSafe)
			{

				SetSelectedVob(parentSafe, "Extern_RemoveAsParent");
				auto onSelect = (onSelectNode)GetProcAddress(module, "SelectNode");
				onSelect((uint32)pickedVob);

			}

			
		}
	}

	

	void SpacerApp::CleanGlobalParent()
	{
		globalParent = NULL;
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


		//OutFile("RemoveVob: vob: " + AHEX32((uint)pVob), true);



		oCItem* isItem = dynamic_cast<oCItem*>(pVob);
	
		// удаляем из списка oCVisualFX от итема
#if ENGINE > Engine_G1
		if (isItem && isItem->effectVob)
		{
			theApp.OnRemoveVob(isItem->effectVob);
			
		}
#endif
		itemsLocator.RemoveByItem(pVob);
		theApp.restorator.RemoveByVob(pVob);
		theApp.OnRemoveVob(pVob);


		zCVobWaypoint* wpvob = dynamic_cast<zCVobWaypoint*>(pVob);
		// Wegpunkt
		if (wpvob)
		{
			zCWayNet* waynet = ogame->GetWorld()->wayNet;
			zCWaypoint* wp = waynet->SearchWaypoint(wpvob);
			if (wp)
			{
				//OutFile("RemoveVob: waypoint " + A wp->name + " " + AHEX32((uint)pVob), true);
				waynet->DeleteWaypoint(wp);
			}
		}
		else
		{


			//OutFile("RemoveVob: vob " + A GetVobName(pVob) + " " + AHEX32((uint)pVob), true);
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