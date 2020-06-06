// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	void SpacerApp::SaveFile(zSTRING worldName, int type)
	{



		//MessageBox(0, worldName + " with index: " + ToStr type, 0, 0);

		switch (type)
		{
		case SAVE_ZEN:
			ogame->OpenSavescreen(FALSE);

			// [EDENFELD] 1.33c: Kein Mesh hier mit abspeichern
			ogame->SaveWorld(worldName, zCWorld::zWLD_SAVE_EDITOR_COMPILED, false, false); // mesh, ascii

			break;
		case SAVE_ZEN_UC:
			ogame->OpenSavescreen(FALSE);
			ogame->SaveWorld(worldName, zCWorld::zWLD_SAVE_EDITOR_UNCOMPILED, false, false); // mesh, ascii

			break;
		case SAVE_ZEN_BIN:
			ogame->OpenSavescreen(FALSE);
			ogame->SaveWorld(worldName, zCWorld::zWLD_SAVE_EDITOR_COMPILED, false, true); // noMesh, Binary
			break;
		}
	}

	// фикс визуалов определенных вобов при сохранении зена
	void callBackVob(zCVob* vob)
	{
		static const zSTRING INVISIBLE_FILEPREFIX = "INVISIBLE_";

		if (vob && vob->GetVisual())
		{
			zSTRING visualName = vob->GetVisual()->GetVisualName();
			visualName.Upper();
			if (
				dynamic_cast< zCVobLight*		>	(vob) ||
				dynamic_cast< zCZone*			>	(vob) ||
				dynamic_cast< zCVobSpot*		>	(vob) ||
				dynamic_cast< zCVobStartpoint*	>	(vob) ||
				dynamic_cast< zCVobWaypoint*	>	(vob) ||
				dynamic_cast< zCCSCamera*		>	(vob) ||
				dynamic_cast< zCCamTrj_KeyFrame*>	(vob) ||
				dynamic_cast< zCVobScreenFX*	>	(vob) ||
				dynamic_cast< zCPFXControler*	>	(vob) ||
				(visualName.Search(INVISIBLE_FILEPREFIX, 1, false) >= 0) ||
				(visualName.Search("VOBBOX", 1, false) >= 0)
				)
			{
				cmd << Col16(CMD_RED) << "Fix visual for: " << GetVobName(vob) << endl;
				vob->SetVisual(0);
				vob->SetShowVisual(FALSE);
			};

		}
	}

	void GetTreeChild(zCTree<zCVob>* node, void(*callBackVob)(zCVob*))
	{
		zCVob* vob = node->GetData();

		callBackVob(vob);

		node = node->GetFirstChild();

		while (node != NULL)
		{
			GetTreeChild(node, callBackVob);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::HandleWorldBeforeSave()
	{
		if (ogame->GetCamera()->connectedVob && ogame->GetCamera()->connectedVob->GetHomeWorld())
			ogame->GetCamera()->connectedVob->GetHomeWorld()->RemoveVob(ogame->GetCamera()->connectedVob);


		zCTree<zCVob>* tree = ogame->GetWorld()->globalVobTree.GetFirstChild();
		while (tree)
		{
			GetTreeChild(tree, callBackVob);
			tree = tree->GetNextChild();
		}

		cmd << Col16(CMD_WHITE);

		treeIsReady = false;
	}

	void SpacerApp::HandleWorldAfterSave()
	{
		ogame->CloseSavescreen();
		ogame->GetWorld()->AddVob(ogame->GetCamera()->connectedVob);


		/*
		zCListSort<zCVob>* mobNode = ogame->GetGameWorld()->GetVobList()->GetNextInList();
		oCMobFire* mob = NULL;
		while (mobNode) {
		mob = zDYNAMIC_CAST<oCMobFire>(mobNode->GetData());
		mobNode = mobNode->GetNextInList();
		if (mob) mob->PostSave();
		}
		*/

		//std::cout << "Union: BuildTree..." << std::endl;
		RebuiltWaypoints();
		treeIsReady = true;
	}

	void SpacerApp::SaveWorld(zSTRING worldName, int type)
	{
		if (!ogame->GetWorld())
		{
			return;
		}

		auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
		load(3);


		//std::cout << "S1: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;

		HandleWorldBeforeSave();


		zoptions->ChangeDir(DIR_WORLD);

		//std::cout << "S2: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;
		SaveFile(worldName, type);
		//std::cout << "S3: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;

		HandleWorldAfterSave();

		//std::cout << "S4: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;

		(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();
	}


	void SpacerApp::LoadMesh(zSTRING worldName)
	{
		isMesh = true;

		zoptions->ChangeDir(DIR_MESHES);
		ogame->GetGameWorld()->LoadWorld(worldName, zCWorld::zWLD_LOAD_EDITOR_UNCOMPILED);

		oCNpc::SetNpcAIDisabled(TRUE);
		dynamic_cast<oCGame*>(gameMan->gameSession)->GetSpawnManager()->SetSpawningEnabled(FALSE);
	}


	void SpacerApp::MergeZen(zSTRING worldName)
	{
		isMesh = false;
		isMerged = true;
		treeIsReady = false;

		zoptions->ChangeDir(DIR_WORLD);
		ogame->GetGameWorld()->MergeVobSubtree(worldName, 0, zCWorld::zWLD_LOAD_MERGE_ADD);



		if (ogame->GetWorld())
		{
			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();
			if (waynet)	
			{
				zSTRING result = waynet->MergeWaypoints();
				if (!result.IsEmpty())
				{
					//AfxMessageBox(zSTRING(zSTRING("Merged the following waypoints: \r\n") + result).ToChar(), MB_OK | MB_TOPMOST);
				};
			};
		}

		WorldAfterLoad();
	}

	void SpacerApp::LoadWorld(zSTRING worldName)
	{

		auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
		load(0);


		isMesh = false;
		isMerged = false;
		WorldPreLoad();

		zoptions->ChangeDir(DIR_WORLD);
		ogame->GetGameWorld()->LoadWorld(worldName, zCWorld::zWLD_LOAD_EDITOR_COMPILED);

		WorldAfterLoad();

		(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();
	}

	void SpacerApp::WorldPreLoad()
	{
		theApp.SetSelectedVob(NULL, "WorldPreLoad");

		Reset();

		if (ogame->GetWorld())
		{
			ogame->GetWorld()->addZonesToWorld = TRUE;
		}

	}

	void SpacerApp::WorldAfterLoad()
	{
		oCNpc::SetNpcAIDisabled(TRUE);
		dynamic_cast<oCGame*>(gameMan->gameSession)->GetSpawnManager()->SetSpawningEnabled(FALSE);
		zCWorld::s_bAlternateRenderOrder = false;

		ogame->CamInit();
		//ogame->EnvironmentInit();
		ogame->GetCamera()->connectedVob->SetVobName("EDITOR_CAMERA_VOB");
		ogame->GetCamera()->connectedVob->SetAI(0);
		//ogame->GetCamera()->connectedVob->bbox3D.mins = zVEC3(5, 5, 5);
		//ogame->GetCamera()->connectedVob->bbox3D.maxs = zVEC3(5, 5, 5);
		ogame->GetGameWorld()->AddVob(ogame->GetCamera()->connectedVob);
		ogame->GetCamera()->connectedVob->dontWriteIntoArchive = true;


		/*
		oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));
		player->dontWriteIntoArchive = true;
		player->SetSleeping(TRUE);
		player->SetPhysicsEnabled(FALSE);
		*/

		//ogame->InsertObjectRoutine(0, "FIREPLACE", 0, 0, 1);
		//ogame->InsertObjectRoutine(0, "FIREPLACE", 7, 0, 0);

		levelReady = false;

		// убиваем gLogStatistics
		*(int*)0x008C2B50 = 0;


		if (options.GetIntVal("musicZenOff"))
		{
			ToggleMusic(false);
		}

		// создает список итемов, pfx, 1 раз на все время работы программы
		if (!dataFilled)
		{
			dataFilled = true;


			std::cout << "Union: Creating PFX/Items/Sounds/Visuals lists..." << std::endl;



			BuildClassHierarchy();

			CreatePFXList();
			CreateItemsList();
			CreateSoundList();
			CreateVisualsList();
			CreateMobNameList();

		}

		std::cout << "Union: BuildTree..." << std::endl;
		BuildTree();



	}

	// применяется на первом тике когда уровень загружен
	void SpacerApp::OnLevelReady()
	{
		if (!levelReady)
		{

			if (ogame->GetCamera() && zCSkyControler::s_activeSkyControler)
			{
				SetTime(12, 0);
				zCTexture::RefreshTexMaxSize(16384);
				zCSkyControler::s_activeSkyControler->m_fRelightTime = 0;

				SetRangeVobs();
				SetRangeWorld();
			}

			//zCBspTree::s_renderAllPortals = TRUE;
			//zCBspTree::s_showPortals = TRUE;
			ogame->GetWorld()->SetWaveAnisEnabled(FALSE);



			zCWorld::s_bAlternateRenderOrder = FALSE;


			options.Apply();

			levelReady = true;

			std::cout << "Union: Apply settings for World after loading..." << std::endl;

			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}
	}

	void SpacerApp::DoCompileLight(int type, int radius)
	{
		zTBBox3D* bbox = 0;

		if (dynLightCompile)
		{
			bbox = new zTBBox3D;
			float x, y, z;
			zCVobLight *light = NULL;// CMainFrame::mainframe->winInfoBar_Lights->cur_vob;
			light->GetPositionWorld(x, y, z);
			bbox->maxs = light->GetBBox3DWorld().maxs;
			bbox->mins = light->GetBBox3DWorld().mins;
		}
		else if (radius > 0)
		{
			bbox = new zTBBox3D;
			float x, y, z;
			ogame->GetCameraVob()->GetPositionWorld(x, y, z);
			bbox->maxs = zVEC3(x + 1, y + 1, z + 1);
			bbox->mins = zVEC3(x - 1, y - 1, z - 1);
			bbox->Scale(radius);
		}


		if (type == CompileLightMod::VertexOnly)
		{
			//MessageBox(0, ToStr radius, 0, 0);
			ogame->GetWorld()->SetWorldRenderMode(zWLD_RENDER_MODE_VERT_LIGHT);
			ogame->GetWorld()->GenerateStaticWorldLighting(zCWorld::zWLD_LIGHT_VERTLIGHT_ONLY, bbox);
		}
		else
		{
			int level = type;

			if (dynLightCompile) level = 1;

			switch (level)
			{
			case 0:
			case 1:

				ogame->GetWorld()->SetWorldRenderMode(zWLD_RENDER_MODE_LIGHTMAPS);
				ogame->GetWorld()->GenerateStaticWorldLighting(zCWorld::zWLD_LIGHT_VERTLIGHT_LIGHTMAPS_LOW_QUAL, bbox);

				break;
			case 2:

				ogame->GetWorld()->SetWorldRenderMode(zWLD_RENDER_MODE_LIGHTMAPS);
				ogame->GetWorld()->GenerateStaticWorldLighting(zCWorld::zWLD_LIGHT_VERTLIGHT_LIGHTMAPS_MID_QUAL, bbox);
				break;
			case 3:
			default:

				ogame->GetWorld()->GenerateStaticWorldLighting(zCWorld::zWLD_LIGHT_VERTLIGHT_LIGHTMAPS_HI_QUAL, bbox);
				break;
			};

		}
	}

	void SpacerApp::DoCompileWorld(int type)
	{
		if (!ogame->GetWorld() || ogame->GetWorld()->IsCompiled())
		{
			return;
		}

		theApp.SetSelectedVob(NULL, "DoCompileWorld");

		float value = 1.0f;

		//if (mode.quick)			value = 0.2F; else value = 1.0F;
		//if (mode.editormode)	value = 0.1F; // Editormode don't need optimization
		zCWorld* world = ogame->GetWorld();
		treeIsReady = false;
		world->DisposeStaticWorld();

		if (type == 0)
		{
			world->CompileWorld(zBSP_MODE_INDOOR, value, FALSE, 0, NULL);
		}
		else
		{
			world->CompileWorld(zBSP_MODE_OUTDOOR, value, FALSE, 0, NULL);
		}

		WorldAfterLoad();

		print.PrintRed(GetLang("UNION_WORLD_ONCOMPILE"));
	}

	void SpacerApp::SaveVobTree(zSTRING path)
	{
		if (!pickedVob)
		{
			return;
		}
		zoptions->ChangeDir(DIR_WORLD);

		HandleWorldBeforeSave();
		ogame->GetWorld()->SaveVobSubtree(path, pickedVob, 0, 0);
		HandleWorldAfterSave();

		print.PrintRed(GetLang("UNION_VOBTREE_SAVE"));
	}

	zCVob* SpacerApp::GetParentVob(zCVob* candidate)
	{
		if (candidate) return candidate;

		zCTree<zCVob>* tree = ogame->GetWorld()->globalVobTree.GetFirstChild();
		while (tree)
		{
			if (tree->GetData() && tree->GetData()->GetVobType() == zVOB_TYPE_LEVEL_COMPONENT)
			{
				return tree->GetData();
			}
			tree = tree->GetNextChild();
		}
		return 0;
	}


	void AddChildsToList(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		//MessageBox(0, ToStr *(int*)vob, 0, 0);

		//auto addNode = (strFuncPoiterTree)GetProcAddress(theApp.module, "AddTreeNode");
		//theApp.OnCreateVob(vob);

		//MessageBox(0, s + " [3] " + zSTRING(parentId), 0, 0);
		//addNode(GetVobName(vob), (int)vob, vob->GetParent(), vob->_GetClassDef()->className.ToChar());

		node = node->GetFirstChild();
		while (node != NULL)
		{
			AddChildsToList(node);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::OpenVobTree(zSTRING path, bool globalInsert)
	{

		zoptions->ChangeDir(DIR_WORLD);

		zCVob* baseVob = pickedVob;
		zCVob* vobWaypoint = dynamic_cast<zCVobWaypoint*>(pickedVob);
		oCItem* pItem = dynamic_cast<oCItem*>(pickedVob);

		if (!baseVob || vobWaypoint || pItem || globalInsert)
		{
			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();

			world->SearchVobListByClass(zCVobLevelCompo::classDef, resultList, 0);

			if (resultList.GetNumInList() > 0)
			{
				baseVob = resultList.GetSafe(rand() % resultList.GetNumInList());

				if (!baseVob)
				{
					std::cout << "Can't find a levelCompo to insert a vobtree!" << std::endl;
					return;
				}
			}
		}



		zCVob* parentVob = GetParentVob(baseVob);

		parentVob->AddRef();

		zCVob* vob = ogame->GetWorld()->MergeVobSubtree(path, parentVob, zCWorld::zWLD_LOAD_MERGE_ADD);

		zVEC3	pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3	dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld();

		HandleVobTranslation(vob, pos + dir * 150);


		zRELEASE(parentVob);

		AddChildsToList(vob->globalVobTreeNode);



		theApp.SetSelectedVob(vob, "OpenVobTree");

		if (theApp.pickedVob)
		{
			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((uint)theApp.pickedVob);
		}

		SelectObject(theApp.pickedVob);


		if (vobWaypoint || pItem)
		{
			//print.PrintRed("VobTree вставлен глобально!");
			//print.PrintRed("Потому что нельзя вставлять в oCItem или zCVobWaypoint!");
		}
		else
		{

		}
		print.PrintRed(GetLang("UNION_VOBTREE_INSERT"));


		zinput->ClearKey(MOUSE_LEFT);
		ClearLMB();
	}

}