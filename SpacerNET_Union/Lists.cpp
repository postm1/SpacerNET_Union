// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	void CreateBaseVobTreeBuildTree(zCTree<zCVob>* node, bool& foundBadItem);

	// Add your code here . . .
	// ������ ������ � ����
	void SpacerApp::CreateItemsList()
	{
		auto addItem = (callVoidFunc)GetProcAddress(theApp.module, "AddItemToList");

		zSTRING scriptClassName = "C_ITEM";

		int pos, typ, anz, c, classindex;

		classindex = parser->GetIndex(scriptClassName);


		exports.toggleUIElement(UIElementType::UI_LIST_ITEMS, FALSE);

		if (classindex != -1)
		{
			parser->GetSymbolInfo(classindex, typ, anz);
			pos = parser->GetInstance(classindex, 0);


			while (pos >= 0)
			{
				zSTRING s = parser->GetSymbolInfo(pos, typ, c);
				// instance_list.Insert(s);
				Stack_PushString(s.Upper());
				addItem();
				pos = parser->GetInstance(classindex, pos + 1);
			}
		}

		auto sort = (voidFuncPointer)GetProcAddress(theApp.module, "SortItems");
		sort();

		exports.toggleUIElement(UIElementType::UI_LIST_ITEMS, TRUE);
	}


	void SpacerApp::CreateMobNameList()
	{
		return;

		zCArray<zSTRING> mobNames;


		cmd << "Looking for mobnames..." << endl;


		//MOBNAME_SIGH
		for (int i = 0; i < parser->symtab.table.GetNumInList(); i++)
		{
			if (parser->symtab.table.GetSafe(i)->name.Search("MOBNAME_", 1) != -1)
			{
				mobNames.Insert(parser->symtab.table.GetSafe(i)->name);
			}
		}

		for (int i = 0; i < mobNames.GetNumInList(); i++)
		{
			//cmd << CString(mobNames.GetSafe(i)) << endl;
		}

	}
	// list of all visuals from the game file system
	void SpacerApp::CreateVisualsList()
	{


		CreateFileList();

		auto addVisuals = (callVoidFunc)GetProcAddress(theApp.module, "AddVisualToListVDF");

		for (int i = 0; i < modelsStrVDF.GetNum(); i++)
		{
			Stack_PushString(modelsStrVDF.GetSafe(i).Upper());
			addVisuals();
		}


		addVisuals = (callVoidFunc)GetProcAddress(theApp.module, "AddVisualToListWORK");

		for (int i = 0; i < modelsStrWORK.GetNum(); i++)
		{
			Stack_PushString(modelsStrWORK.GetSafe(i));
			addVisuals();
		}



		auto sort = (callVoidFunc)GetProcAddress(theApp.module, "SortVisuals");
		sort();

		modelsStrWORK.DeleteList();
		modelsStrVDF.DeleteList();
	}

	// PFX list into the CreateVisualsList window
	void SpacerApp::CreatePFXList()
	{
		auto addPFX = (callVoidFunc)GetProcAddress(theApp.module, "AddPacticleToList");

		exports.toggleUIElement(UIElementType::UI_LIST_PFX, FALSE);


		for (int i = 0; i<zCParticleFX::s_emitterPresetList.GetNumInList(); i++)
		{
			Stack_PushString(zCParticleFX::s_emitterPresetList.GetSafe(i)->particleFXName.Upper());
			addPFX();
		}
		auto sort = (callVoidFunc)GetProcAddress(theApp.module, "SortPFX");
		sort();

		exports.toggleUIElement(UIElementType::UI_LIST_PFX, TRUE);
	}

	void SpacerApp::CreateCameraPresetsList()
	{
		return;// for now
		auto addCamPreset = (callVoidFunc)GetProcAddress(theApp.module, "AddCameraPresetToList");

		exports.toggleUIElement(UIElementType::UI_CAM_PRESETS, FALSE);



		for (int i = 0; i < presetsLib.GetNumInList(); i++)
		{
			if (auto pEntry = presetsLib.GetSafe(i))
			{
				zCCSCamera* camera = dynamic_cast<zCCSCamera*>(pEntry->obj);

				if (camera)
				{
					Stack_PushString(pEntry->presetName.Upper());
					addCamPreset();
				}
			}
		}

		exports.toggleUIElement(UIElementType::UI_CAM_PRESETS, TRUE);
	}

	// list of Sounds
	void SpacerApp::CreateSoundList()
	{
		auto addSound = (callVoidFunc)GetProcAddress(theApp.module, "AddSoundToList");


		zCParser* soundParser = zsound->GetSFXParser();;
		soundParser->SetStopOnError(FALSE);
		zoptions->ChangeDir(DIR_SCRIPTS);

		zSTRING classname = "C_SFX";

		int pos, typ, anz, c, classindex;
		zSTRING s;

		classindex = soundParser->GetIndex(classname);


		exports.toggleUIElement(UIElementType::UI_SOUND_LIST, FALSE);

		if (classindex != -1)
		{
			// Get instances
			soundParser->GetSymbolInfo(classindex, typ, anz);
			pos = soundParser->GetInstance(classindex, 0);
			while (pos >= 0)
			{
				s = soundParser->GetSymbolInfo(pos, typ, c);
				Stack_PushString(s);
				addSound();
				pos = soundParser->GetInstance(classindex, pos + 1);
			}

		}

		


		(voidFuncPointer)GetProcAddress(theApp.module, "SortSounds")();


		exports.toggleUIElement(UIElementType::UI_SOUND_LIST, TRUE);

		//				static int chekcflag = false;

		auto addMusic = (callVoidFunc)GetProcAddress(theApp.module, "AddMusicToList");

		auto base = parserMusic->GetBase(parserMusic->GetIndex("C_MUSICTHEME"));



		exports.toggleUIElement(UIElementType::UI_MUSIC_LIST, FALSE);

		for (int i = 0; i < parserMusic->symtab.table.GetNum(); i++)
		{
			auto pSym = parserMusic->symtab.table.GetSafe(i);
						
			if (pSym && pSym->type == zPAR_TYPE_INSTANCE)
			{
				auto parentProto = pSym->GetParent();

				if (parentProto)
				{
					auto classParent = parentProto->GetParent();

					if (classParent && (classParent->name == "C_MUSICJINGLE" || classParent->name == "C_MUSICTHEME"))
					{
						Stack_PushString(pSym->name);
						addMusic();


						/*
						cmd << (pSym->name + " " + Z(int)pSym->parent) << endl;

						if (pSym->GetParent())
						{
							cmd << "Parent: " << pSym->GetParent()->name << endl;
						}
						*/
					}
				}
				
			}
						
		}

		(voidFuncPointer)GetProcAddress(theApp.module, "SortMusic")();

		exports.toggleUIElement(UIElementType::UI_MUSIC_LIST, TRUE);

				
	}

	void SpacerApp::BuildTree()
	{
		zCWorld* world = ogame->GetWorld();

		cmd << "[Union: BuildTree]" << endl;

		exports.toggleUIElement(UIElementType::UI_ALL_VOBS_TREE_LIST, FALSE);

		(callVoidFunc)GetProcAddress(theApp.module, "ClearAllEntries")();
	
		zCTree<zCVob>* tree = world->globalVobTree.GetFirstChild();

		bool foundBadItem = false;

		while (tree)
		{
			CreateBaseVobTreeBuildTree(tree, foundBadItem);
			tree = tree->GetNextChild();
		}


		treeIsReady = true;

		if (foundBadItem)
		{
			print.PrintRed("Found Broken Item (No script found)", 12);
		}

		

		RX_Begin(8);

		CreateTree();

		RX_End(8);
		cmd << "[CreateTree UI List finished] " << RX_PerfString(8) << endl;

		exports.toggleUIElement(UIElementType::UI_ALL_VOBS_TREE_LIST, TRUE);

	}


	void CreateBaseVobTreeNew(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();


		if (vob)
		{

			if (vob->GetVisual())
			{
				vob->GetVisual()->Release();
			}
			vob->SetShowVisual(FALSE);

			vob->RemoveVobFromBspTree();
			//if ((vob->GetVisual()) && (vob->GetHomeWorld()))	vob->GetVisual()->HostVobRemovedFromWorld(vob, vob->GetHomeWorld());
			//vob->RemoveVobFromGlobalVobTree();
		}


		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateBaseVobTreeNew(node);
			node = node->GetNextChild();
		}
	}




	void CreateWaypointVobTree(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		static AddGlobalEntryPointer addEntry = NULL;

		if (!addEntry)
		{
			addEntry = (AddGlobalEntryPointer)GetProcAddress(theApp.module, "AddGlobalEntry");
		}


		if (dynamic_cast<zCVobWaypoint*>(vob))
		{
			Stack_PushString(vob->_GetClassDef()->className);
			Stack_PushString(GetVobName(vob));

			addEntry((uint)vob, vob->GetParent());
		}

		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateWaypointVobTree(node);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::RebuiltWaypoints()
	{
		zCWorld* world = ogame->GetWorld();

		(callVoidFunc)GetProcAddress(theApp.module, "ClearWaypoints")();


		zCTree<zCVob>* tree = world->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateWaypointVobTree(tree);
			tree = tree->GetNextChild();
		}

		(callVoidFunc)GetProcAddress(theApp.module, "ReloadWaypoint")();
	}
}