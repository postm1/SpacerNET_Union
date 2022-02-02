
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	
	/*
	//rx_report
	HOOK ivk_zERROR_Init AS(&zERROR::Init, &zERROR::Init_Union);
	void zERROR::Init_Union(zSTRING cmd) {
		THISCALL(ivk_zERROR_Init)(cmd);
		int target = 0;
		this->SetTarget(target | zERR_TARGET_SPY); //задать цель вывода zSpy, или файл в корне диска C: zERR_TARGET_FILE/zERR_TARGET_SPY
	}

	//0x0044C8D0 public: int __thiscall zERROR::Report(enum zERROR_TYPE, int, class zSTRING const &, signed char, unsigned int, int, char *, char *)

	zSTRING lastMsg = "";

	HOOK ivk_zERROR_Report AS(&zERROR::Report, &zERROR::Report_Union);
	int zERROR::Report_Union(zERROR_TYPE type, int id, zSTRING const& str_text, signed char levelPrio, unsigned int flag, int line, char* file, char* function) {


		if (str_text != lastMsg)
		{
			cmd << str_text << endl;
			lastMsg = str_text;
		}
		
		//OutFile("zERROR_TYPE: " + str_text, true);

		return 0;
		//return THISCALL(ivk_zERROR_Report)(type, id, str_text, levelPrio, flag, line, file, function);
	}
	*/



	#define MAXSIZE 24
	#define d(i) (((char *)data)+(i)*size)
	static zCSparseArray<const void *, int>& s_polyVertIndex = *(zCSparseArray<const void *, int>*)(0x8D8798);

	void insertionsort(void *data, size_t num, size_t size, int(__cdecl *compare)(const void *, const void *), bool falltoqs) {
		char swapplace[MAXSIZE];

		OutFile("Sort: num: " + ToStr (int)num + " size: " + ToStr(int)size, true);

		if (size>MAXSIZE) {
			qsort(data, num, size, compare);
			return;
		}

		int swaps = 0;
		for (int i = 1; i<(int)num; i++) {
			void *lower = d(i);
			for (int j = i - 1; j >= 0; j--) {
				void *upper = lower;
				lower = d(j);
				if ((*compare)(upper, lower) < 0) { // ok. Ist im Moment BubbleSort. Was solls...
					swaps++;
					memcpy(&swapplace, upper, size);
					memcpy(upper, lower, size);
					memcpy(lower, &swapplace, size);
				}
				else
					j = 0; // hier kann man die innere Schleife schon abbrechen.
			}
			if (falltoqs && swaps > 5 * i + 5) {
				qsort(data, num, size, compare);
				return;
			}
		}

	}

	static int S_ComparePolyVerts(const void *A_, const void *B) {
		void *a = *((void **)A_);
		void *b = *((void **)B);

		int *AI = s_polyVertIndex[a];
		int *BI = s_polyVertIndex[b];

		if (!AI || !BI)
			return 0;

		int ai = *AI;
		int bi = *BI;

		if (ai < bi)
			return -1;
		if (ai > bi)
			return 1;

		return 0;
	}

	// Add your code here . . .
	//0x00572DC0 public: void __thiscall zCMesh::SortPolysByList(class zCPolygon * *,int)
	HOOK Invk_SortPolysByList   AS(&zCMesh::SortPolysByList, &zCMesh::SortPolysByList_Hook);
	void zCMesh::SortPolysByList_Hook(zCPolygon** list, int listLength)
	{
		THISCALL(Invk_SortPolysByList)(list, listLength);

		return;

		if (theApp.firstTimeZenSaved)
		{
			OutFile("No sorting polygons anymore...", true);
			return;
		}

		
		theApp.firstTimeZenSaved = true;
		return;

		OutFile("SortPolysByList_Hook Start: " + ToStr listLength, true);


		int i = 0;
		
		//zCMesh::DeleteAll();

		OutFile("ArraysToLists", false);
		this->ClearLists();
		this->ArraysToLists();

		
		//this->s_usesVertexCache = 0;
		OutFile("s_polyVertIndex.Clear()", false);
		s_polyVertIndex.Clear();
		
		

		

		OutFile("listLength: " + ToStr listLength, false);

		OutFile("Insert", false);

		for (i = listLength - 1; i >= 0; i--)
			*(s_polyVertIndex.Insert(list[i])) = i;

		
		
		OutFile("numPoly: " + ToStr numPoly, false);
		OutFile("s_numVertListScene: " + ToStr (*polyList)->s_numVertListScene, false);
		OutFile("s_numFeatListScene: " + ToStr(*polyList)->s_numFeatListScene, false);
		OutFile("polyNumVert: " + ToStr(*polyList)->polyNumVert, false);
		OutFile("s_actNumClipVert: " + ToStr(*polyList)->s_actNumClipVert, false);
		

		OutFile("insertionsort", false);
		insertionsort(polyList, numPoly, sizeof(void *), &S_ComparePolyVerts, true);

		

		if (featList)
		{
			OutFile("=================featList", false);
			//OutFile("s_actNumClipVert: " + ToStr((*featList)->, false);
			//zERR_MESSAGE(5, 0, "M: Assigning indices to features...    ");
			OutFile("s_polyVertIndex.Insert", false);
			OutFile("numPoly: " + ToStr numPoly, false);
			OutFile("numFeat: " + ToStr numFeat, false);
			OutFile("numVert: " + ToStr numVert, false);

			OutFile("polyNumVert: " + ToStr(*polyList)->polyNumVert, false);
			//		 gib den Vertices dann die Nummer des Polygons, in dem sie zuerst auftauchen
			for (i = numPoly - 1; i >= 0; i--)
			{
				zCPolygon *p = polyList[i];
				//OutFile(" => numPoly: " + ToStr (p->polyNumVert), false);
				for (int j = p->polyNumVert - 1; j >= 0; j--)
					*(s_polyVertIndex.Insert(p->feature[j])) = i;
			}
			
			
			OutFile("numFeat: " + ToStr this->numFeat, false);
			OutFile("insertionsort 2", false);
			insertionsort(featList, numFeat, sizeof(void *), &S_ComparePolyVerts, false);
			
		}
		else
		{
			OutFile("!featList", false);
		}


		OutFile("s_polyVertIndex.Clear()", false);
		s_polyVertIndex.Clear();
		OutFile("SortPolysByList_Hook End: " + ToStr listLength, false);
		
	}

	void SpacerApp::RemoveBadLevelCompoVisual()
	{
		zCVob* foundVob = NULL;

		zCArray<zCVob*> resultList;
		zCWorld* world = ogame->GetWorld();

		world->SearchVobListByClass(zCVobLevelCompo::classDef, resultList, 0);

		int countResult = resultList.GetNumInList();

		for (int i = 0; i < countResult; i++)
		{
			auto vob = resultList.GetSafe(i);

			if (vob && vob->visual && (vob->visual->GetVisualName().Contains(".3ds")
				|| vob->visual->GetVisualName().Contains(".3DS")))
			{

				cmd << "zCVobLevelCompo: Set visual NULL for: " << vob->visual->GetVisualName() << endl;

				vob->SetVisual(NULL);

			}
		}

	}

	void SpacerApp::SaveFile(zSTRING worldName, int type)
	{


		//0x00831C10 const zCSparseArray<void const *,int>::`vftable'
		//static zCSparseArray<const void *, int> s_polyVertIndex; 8D8798
		//static	zCTree<zCVob>*& s_firstVobSaveWorld = *(zCTree<zCVob>**)(0x9A4410);


		//zCSparseArrayBase::Clear(&unk_8D8798);


		//OutFile("s_firstVobSaveWorld: " + ToStr (int)s_firstVobSaveWorld->GetData(), false);

		//static zCSparseArray<const void *, int> s_polyVertIndex = *(zCSparseArray<const void *, int>*)(0x8D8798);
		//static zCSparseArray<const void *, void*> s_accounts = *(zCSparseArray<const void *, void*>*)(0xAAC2F8);
		
		//s_polyVertIndex.Clear();
		//s_polyVertIndex.Destroy();
		//s_polyVertIndex.Create(12);
		
		//s_accounts.Clear();
		//s_accounts.Destroy();

		//s_polyVertIndex.;



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
				//dynamic_cast< zCVobScreenFX*	>	(vob) ||
				//dynamic_cast< zCPFXControler*	>	(vob) ||
				(visualName.Search(INVISIBLE_FILEPREFIX, 1) >= 0) ||
				(visualName.Search("VOBBOX", 1) >= 0)
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
		mm.CleanSelectMaterial();
		if (ogame->GetCamera()->connectedVob && ogame->GetCamera()->connectedVob->GetHomeWorld())
			ogame->GetCamera()->connectedVob->GetHomeWorld()->RemoveVob(ogame->GetCamera()->connectedVob);





		/*
		zCTree<zCVob>* tree = ogame->GetWorld()->globalVobTree.GetFirstChild();
		while (tree)
		{
			GetTreeChild(tree, callBackVob);
			tree = tree->GetNextChild();
		}
		*/

		if (theApp.options.GetIntVal("checkBoxAutoRemoveAllVisuals"))
		{
			this->RemoveBadLevelCompoVisual();
		}
		
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


		//entry
		treeIsReady = true;
	}

	void SpacerApp::SaveWorld(zSTRING worldName, int type)
	{
		if (!ogame->GetWorld() || !ogame->world->globalVobTree.GetFirstChild())
		{
			return;
		}

		auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
		load(3);


		//std::cout << "S1: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;

		HandleWorldBeforeSave();


		zCWorld* world = ogame->GetWorld();

		if (world && !world->IsCompiled())
		{
			Message::Box("no compiled!");
			return;
		}
		else
		{
			//WorldAfterLoad();
		}


		if (ogame->GetWorld() && ogame->GetWorld()->compiled)
		{
			//ogame->GetWorld()->bspTree.mesh->ArraysToLists();
			//ogame->GetWorld()->bspTree.mesh->UnshareFeatures();
		}



		zoptions->ChangeDir(DIR_WORLD);

		//std::cout << "S2: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;
		SaveFile(worldName, type);
		//std::cout << "S3: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;

		HandleWorldAfterSave();

		//std::cout << "S4: " << zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1) << std::endl;

		(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();
	}

	
	void SpacerApp::MergeMesh(CString worldName)
	{
		zoptions->ChangeDir(DIR_MESHES);
		ogame->GetWorld()->LoadWorld(worldName, zCWorld::zWLD_LOAD_EDITOR_UNCOMPILED);

		oCNpc::SetNpcAIDisabled(TRUE);
		dynamic_cast<oCGame*>(gameMan->gameSession)->GetSpawnManager()->SetSpawningEnabled(FALSE);
	}
	


	void SpacerApp::LoadMesh(zSTRING worldName)
	{
		
		mm.CleanSelectMaterial();

		Reset();
		

		isMesh = true;
		bDebugSpacerLoadMesh = true;

		zoptions->ChangeDir(DIR_MESHES);
		ogame->GetGameWorld()->LoadWorld(worldName, zCWorld::zWLD_LOAD_EDITOR_UNCOMPILED);

		oCNpc::SetNpcAIDisabled(TRUE);
		dynamic_cast<oCGame*>(gameMan->gameSession)->GetSpawnManager()->SetSpawningEnabled(FALSE);

		if (theApp.options.GetIntVal("autoCompileWorldAfterLoad"))
		{
			DoCompileWorld(1);
			DoCompileLight(2, 0);
		}
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

		if (theApp.options.GetIntVal("autoCompileWorldLight"))
		{
			int light = theApp.options.GetIntVal("lightCompileType");
			int world = theApp.options.GetIntVal("worldCompileType");

			auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
			load(1);
			theApp.DoCompileWorld(world);

			(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();

			PlaySoundGame(ToStr "CS_IAI_ME_ME");


			load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
			load(2);

			theApp.DoCompileLight(light, 0);

			(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();


			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}

		if (theApp.options.GetIntVal("autoRemoveLevelCompo"))
		{
			zCVob* foundVob = NULL;

			zCArray<zCVob*> resultList;
			zCWorld* world = ogame->GetWorld();

			world->SearchVobListByClass(zCVobLevelCompo::classDef, resultList, 0);

			int countResult = resultList.GetNumInList();

			for (int i = 0; i < countResult; i++)
			{
				auto vob = resultList.GetSafe(i);

				if (vob && vob->visual && (vob->visual->GetVisualName().Contains(".3ds") 
					|| vob->visual->GetVisualName().Contains(".3DS")))
				{

					if (countResult > 1)
					{
						foundVob = vob;
						break;
					}
					else
					{
						vob->SetVisual(NULL);
					}
					
				}
			}


			if (foundVob)
			{

				theApp.RemoveVob(foundVob);
			}
			
		}
	}

	

	int globalWorldLoadType = zCWorld::zWLD_LOAD_EDITOR_COMPILED;
	// 006C65A0 ; void __thiscall oCGame::LoadGame(oCGame *this, int, const struct zSTRING *)
	void __fastcall oCGame_LoadGame(oCGame* _this, void* vt, int slotID, const struct zSTRING& wldName);

	HOOK Hook_oCGame_LoadGame AS(0x006C65A0, &oCGame_LoadGame);

	

	void __fastcall oCGame_LoadGame(oCGame* _this, void* vt, int slotID, const struct zSTRING& wldName) {

		switch (globalWorldLoadType)
		{
			case 1: ogame->GetGameWorld()->LoadWorld(wldName, zCWorld::zWLD_LOAD_EDITOR_COMPILED); break;
			case 2: ogame->GetGameWorld()->LoadWorld(wldName, zCWorld::zWLD_LOAD_EDITOR_UNCOMPILED); break;
		}
	}

	void SpacerApp::LoadWorld(zSTRING worldName, int type)
	{

		auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
		load(0);

		if (IsVirtualFile(worldName))
		{
			(callVoidFunc)GetProcAddress(theApp.module, "ShowVdfWarning")();
		}


		isMesh = false;
		isMerged = false;
		WorldPreLoad();

		zoptions->ChangeDir(DIR_WORLD);

		globalWorldLoadType = type;

		ogame->LoadGame(-2, worldName);
		
		zCWorld* world = ogame->GetWorld();

		if (world && !world->IsCompiled())
		{
			if (theApp.options.GetIntVal("autoCompileWorldLightForUnc"))
			{
				int light = theApp.options.GetIntVal("lightCompileType");
				int world = theApp.options.GetIntVal("worldCompileType");

				auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
				load(1);
				theApp.DoCompileWorld(world);

				(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();

				PlaySoundGame(ToStr "CS_IAI_ME_ME");


				load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
				load(2);

				theApp.DoCompileLight(light, 0);

				(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();


				PlaySoundGame(ToStr "CS_IAI_ME_ME");
			}
		}
		else
		{
			WorldAfterLoad();
		}


		

		(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();
	}

	void SpacerApp::WorldPreLoad()
	{
		theApp.SetSelectedVob(NULL, "WorldPreLoad");

		if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
		if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

		theApp.pickedWaypoint2nd = NULL;
		mm.CleanSelectMaterial();
		theApp.ClearRespList();


		Reset();

		if (ogame->GetWorld())
		{
			ogame->GetWorld()->addZonesToWorld = TRUE;
		}

	}

	void SpacerApp::WorldAfterLoad()
	{
		if (!viewInfo)
		{
			viewInfo = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
			viewInfo->SetFont("FONT_OLD_10_WHITE.TGA");
			viewInfo->SetFontColor(zCOLOR(15, 255, 15));
			viewInfo->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
		}

		viewInfo->ClrPrintwin();

		this->pickUnshareShow = false;

		oCNpc::SetNpcAIDisabled(TRUE);
		dynamic_cast<oCGame*>(gameMan->gameSession)->GetSpawnManager()->SetSpawningEnabled(FALSE);
#if ENGINE > Engine_G1
		zCWorld::s_bAlternateRenderOrder = false;
#endif
		ogame->CamInit();
		//ogame->EnvironmentInit();
		ogame->GetCamera()->connectedVob->SetVobName("EDITOR_CAMERA_VOB");
		ogame->GetCamera()->connectedVob->SetAI(0);
		//ogame->GetCamera()->connectedVob->bbox3D.mins = zVEC3(5, 5, 5);
		//ogame->GetCamera()->connectedVob->bbox3D.maxs = zVEC3(5, 5, 5);
		ogame->GetGameWorld()->AddVob(ogame->GetCamera()->connectedVob);
		ogame->GetCamera()->connectedVob->dontWriteIntoArchive = true;


		if (auto sym = parser->GetSymbol("RX_IsSpacetNet"))
		{
			parser->SetScriptInt("RX_IsSpacetNet", 1);
		}
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

		restorator.GenerateVobsPos();

		if (options.GetIntVal("soundZenOff"))
		{
			if (zsound) zsound->StopAllSounds();
		}

		
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
#if ENGINE > Engine_G1
				zCSkyControler::s_activeSkyControler->m_fRelightTime = 0;
#endif
				SetRangeVobs();
				SetRangeWorld();
			}

			//zCBspTree::s_renderAllPortals = TRUE;
			//zCBspTree::s_showPortals = TRUE;
#if ENGINE > Engine_G1
			ogame->GetWorld()->SetWaveAnisEnabled(FALSE);



			zCWorld::s_bAlternateRenderOrder = FALSE;
#endif

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
		if (!ogame->GetWorld())// || ogame->GetWorld()->IsCompiled())
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
		

		//MessageBox(0, s + " [3] " + zSTRING(parentId), 0, 0);
		//addNode(GetVobName(vob), (int)vob, vob->GetParent(), vob->_GetClassDef()->className.ToChar());

		node = node->GetFirstChild();
		while (node != NULL)
		{
			AddChildsToList(node);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::ExportWorldMesh(zSTRING worldNameFile) {


		oCWorld* world = ogame->GetGameWorld();
		zCBspTree& bspTree = world->bspTree;
		zCMesh* mesh = bspTree.mesh;
		zSTRING worldName = world->GetWorldFilename().GetWord("\\", -1);
		worldName.Replace(".ZEN", ".MSH");
		
		zCFileBIN file;
		file.BinCreate(worldNameFile);
		bspTree.mesh->CreateListsFromArrays(); // !
		mesh->SaveMSH(file);
		file.BinClose();

	}


	void SpacerApp::OpenVobTree(zSTRING path, bool globalInsert, bool insertNearCamera)
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

		if (parentVob && parentVob->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;

		};


		parentVob->AddRef();

		zCVob* vob = ogame->GetWorld()->MergeVobSubtree(path, parentVob, zCWorld::zWLD_LOAD_MERGE_ADD);


		if (insertNearCamera)
		{
			zVEC3	pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
			zVEC3	dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld();

			HandleVobTranslation(vob, pos + dir * 150);
		}
		


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