// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	zCArray<HWND> childWindows;


	bool IsAppForeground()
	{
		bool result = (intFuncPointer)GetProcAddress(theApp.module, "IsAppActive");
		return result;
	}

	CTimer mainTimer;

	HOOK Ivk_CGameManager_Run AS(&CGameManager::Run, &CGameManager::Run_Spacer);

	void CGameManager::Run_Spacer()
	{
		//Message::Box("Run_AST which is Loop");

		if (dontStartGame) return;

		gameSession->game_testmode = TRUE;

		exitGame = FALSE;
		zSTRING inivalue = zoptions->ParmValue("ini");
		if (inivalue.IsEmpty())
			inivalue = "Gothic.ini";
		zoptions->Save(inivalue);

		//gameSession->LoadGame(SAVEGAME_SLOT_NEW, "");

		InitScreen_Close();

		if (!exitGame)
		{
			do
			{
				sysEvent();

				POINT   mPos;
				GetCursorPos(&mPos);
				zCView::GetInput();

				if (theApp.g_bIsPlayingGame)
				{
					theApp.GameLoop();
				}

				//Base render
				if (GetGame() && gameSession && ogame->GetWorld() && ogame->GetCamera() && theApp.IsAWorldLoaded() && ogame->GetWorld()->compiled)
				{

					theApp.PreRender();

					gameSession->Render();

					theApp.AfterRender();


					gameSession->RenderBlit();

					mainTimer.Attach();

					if (mainTimer(1, 500))
					{
						if (!IsAppForeground() && !ogame->singleStep)
						{
							ogame->Pause(FALSE);
						}

					}


					if (ogame->singleStep && (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin))
					{
						ogame->Unpause();
					}

				}
				else
				{

					theApp.RenderStartScreen();
				}
			} while (!exitGame);
		}
	}


	void SpacerApp::PluginLoop()
	{
		if (theApp.isExit || theApp.g_bIsPlayingGame)
		{
			return;
		}
		// MessageBox(0, "Game_Loop", 0, 0);

		print.Loop();

		if (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin)
		{
			theApp.Loop();

		}

		RenderVobsLoop();

		theApp.OnLevelReady();
		theApp.KeysLoop();

		pfxManager.Loop();

		ResetPrintDebug();

		if (screen && ogame->GetCamera() && ogame->GetWorld())
		{
			zVEC3 pos = ogame->GetCamera()->activeCamPos;

			screen->SetFont("FONT_OLD_10_WHITE_HI.TGA");
			screen->SetFontColor(zCOLOR(0, 255, 0));

			//screen->Print(0, 0, "TEST");
			//PrintDebug(zSTRING("ForeWindow: ") + zSTRING((int)GetForegroundWindow()));

			if (theApp.options.GetIntVal("showFps"))
			{
				PrintDebug(zSTRING("FPS: ") + zSTRING(GetFPS()));
			}

			if (theApp.options.GetIntVal("showTris"))
			{
				zTRnd_Stats stat;
				zrenderer->GetStatistics(stat);
				PrintDebug(zSTRING(GetLang("UNION_SHOW_TRIS")) + zSTRING(int(stat.numTrisRendered)));
			}


			if (theApp.options.GetIntVal("showCamCoords"))
			{
				PrintDebug((zSTRING(GetLang("UNION_CAM_POS")) + "(" + zSTRING(pos.n[0], 6) + zSTRING(", ") + zSTRING(pos.n[1], 6)

					+ zSTRING(", ") + zSTRING(pos.n[2], 6) + zSTRING(")")));
			}

			if (theApp.options.GetIntVal("showVobsCount"))
			{
				PrintDebug(zSTRING(GetLang("UNION_VOB_COUNT")) + zSTRING(ogame->GetWorld()->globalVobTree.CountNodes() - 1));
			}

			if (theApp.options.GetIntVal("showWaypointsCount"))
			{
				PrintDebug(zSTRING(GetLang("UNION_WP_COUNT")) + zSTRING(ogame->GetWorld()->wayNet->wplist.GetNumInList()));
			}

			screen->SetFontColor(zCOLOR(240, 123, 14));


			if (theApp.pickedWaypoint && theApp.pickedWaypoint2nd)
			{
				PrintDebug(ToStr "WP 1: " + GetVobNameSafe(theApp.pickedWaypoint));
				PrintDebug(ToStr "WP 2: " + GetVobNameSafe(theApp.pickedWaypoint2nd));
			}
			else
				if (theApp.pickedVob)
				{
					PrintDebug(ToStr GetVobNameSafe(theApp.pickedVob) + " Ref: " + ToStr theApp.pickedVob->refCtr);

					if (theApp.options.GetIntVal("showVobDist"))
					{
						PrintDebug(ToStr GetLang("UNION_DIST") + ToStr(int)Dist(ogame->GetCamera()->connectedVob, theApp.pickedVob));
					}

				}

			//PrintDebug(ToStr zCWorld::s_bWaveAnisEnabled);
			screen->SetFontColor(zCOLOR(255, 255, 255));

			
		}
	}

	void SpacerApp::KeysLoop()
	{


		if (keys.KeyPressed("VOBLIST_COLLECT", true))
		{
			FindNearestVobs();
		}

		if (keys.KeyPressed("WP_CREATEFAST", true))
		{
			CreateNewWaypointFromButtonClick();
		}



		if (keys.KeyPressed("WIN_HIDEALL", true))
		{
			hideWindows = !hideWindows;

			hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
		}


		if (GetSelectedVob() && keys.KeyPressed("OPEN_CONTAINER", true))
		{
			(voidFuncPointer)GetProcAddress(theApp.module, "PropsOpenContainer")();
		}


		if (keys.KeyPressed("TOGGLE_VOBS", true))
		{
			auto call = (callIntFunc)GetProcAddress(theApp.module, "Menu_Toggle");
			call(ToggleMenuType::ToggleVobs);
		}

		if (keys.KeyPressed("TOGGLE_WAYNET", true))
		{
			auto call = (callIntFunc)GetProcAddress(theApp.module, "Menu_Toggle");
			call(ToggleMenuType::ToggleWaynet);
		}

		if (keys.KeyPressed("TOGGLE_HELPERS", true))
		{
			auto call = (callIntFunc)GetProcAddress(theApp.module, "Menu_Toggle");
			call(ToggleMenuType::ToggleHelpers);
		}

		if (keys.KeyPressed("TOGGLE_BBOX", true))
		{
			auto call = (callIntFunc)GetProcAddress(theApp.module, "Menu_Toggle");
			call(ToggleMenuType::ToggleBbox);
		}

		if (keys.KeyPressed("TOGGLE_INVIS", true))
		{
			auto call = (callIntFunc)GetProcAddress(theApp.module, "Menu_Toggle");
			call(ToggleMenuType::ToggleInvis);
		}

		if (KeyPress(KEY_F4))
		{
			/*
			zSTRING name = "DSDsdasds";
			PlaySoundGame(name);

			print.PrintRed("playing...");
			/*

			print.PrintRed("Vob...");



			zCVob* vob = theApp.GetSelectedVob();

			int& s_bAddVobsToMesh = *(int*)0x008D8870;

			if (vob)
			{

			ogame->GetWorld()->cbspTree->BuildTree(1.0f);
			ogame->GetWorld()->bspTree.Build(ogame->GetWorld()->cbspTree);
			s_bAddVobsToMesh = TRUE;
			ogame->GetWorld()->TraverseBspAddVobsToMesh(ogame->GetWorld()->cbspTree, vob->globalVobTreeNode);
			s_bAddVobsToMesh = FALSE;

			ogame->GetWorld()->cbspTree->DeleteTree();

			MessageBox(0, "1232", 0, 0);
			//ogame->GetWorld()->bspTree.DisposeTree();
			//ogame->GetWorld()->UpdateVobTreeBspDependencies(&ogame->GetWorld()->globalVobTree);
			//ogame->GetWorld()->cbspTree->CreateBspSectors();

			//ogame->GetWorld()->UpdateVobTreeBspDependencies(ogame->GetWorld()->globalVobTree.GetFirstChild());
			//ogame->GetWorld()->cbspTree->CreateBspSectors();
			}
			*/

			zinput->ClearKey(KEY_F4);

		}


		if (KeyPress(KEY_F5))
		{
			//ScaleVob(theApp.GetSelectedVob(), 0.5f);
			/*
			print.PrintRed("Playing the game...");
			PlayTheGame();
			*/

			//SaveCurrentWorldToMSH();
			zinput->ClearKey(KEY_F5);
		}


		if (KeyPress(KEY_NUMPADPLUS))
		{
			if (playerLightInt <= 20000)
			{
				playerLightInt = playerLightInt + 500;
			}

			print.PrintRed(ToStr GetLang("UNION_LIGHT_RAD_INC") + ToStr playerLightInt);
			zinput->ClearKey(KEY_NUMPADPLUS);
		}

		if (KeyPress(KEY_NUMPADMINUS))
		{
			if (playerLightInt >= 500)
			{
				playerLightInt = playerLightInt - 500;
			}

			print.PrintRed(ToStr GetLang("UNION_LIGHT_RAD_DEC") + ToStr playerLightInt);
			zinput->ClearKey(KEY_NUMPADMINUS);
		}




		if (KeyPress(KEY_NUMPADSTAR))
		{
			playerLightInt = 0;

			print.PrintRed(ToStr GetLang("UNION_LIGHT_RAD_ZERO"));

			if (zCSkyControler::GetActiveSkyControler())
			{
				zCSkyControler::GetActiveSkyControler()->SetLightDirty();
			}

			zinput->ClearKey(KEY_NUMPADSTAR);
		}

	}

	void SpacerApp::RenderStartScreen()
	{
		static zCView* view;

		if (!view)
		{
			view = new zCView(0, 0, 8192, 8192);
			view->InsertBack("BLACK.TGA");
		}

		screen->InsertItem(view);

		view->ClrPrintwin();

		static int xRes = zoptions->ReadInt("VIDEO", "zVidResFullscreenX", 800);
		static int yRes = zoptions->ReadInt("VIDEO", "zVidResFullscreenY", 800);

		if (isMesh)
		{
			view->SetFontColor(GFX_RED);
			view->Print(40, 1500, GetLang("UNION_MESH_LOADED"));
		}
		else if (isMerged)
		{
			view->SetFontColor(GFX_RED);
			view->Print(40, 1500, GetLang("UNION_MESH_READY"));
		}
		else
		{
			auto getVersion = (callVoidFunc)GetProcAddress(theApp.module, "GetSpacerVersion");
			getVersion();

			CString version = Stack_PeekString();

			view->SetFontColor(zCOLOR(255, 255, 255, 255));
			view->Print(40, 250, "Spacer.NET " + version);
			view->Print(40, 500, GetLang("UNION_EDITOR"));
			view->SetFontColor(GFX_GREEN);
			view->Print(40, 750, GetLang("UNION_TEAM"));
			view->Print(40, 1000, ToStr GetLang("UNION_DATE_COMPILE") + ToStr __DATE__ + " " + ToStr __TIME__);

			view->Print(40, 1250, ToStr GetLang("UNION_RESOLUTION") + ToStr xRes + "x" + ToStr yRes);

			view->SetFontColor(GFX_RED);
			view->Print(40, 1750, GetLang("UNION_NOWORLD"));

		}


		gameMan->gameSession->Render();
		gameMan->gameSession->RenderBlit();
		screen->RemoveItem(view);

		if (!firstZenLoad)
		{
			firstZenLoad = true;

			if (options.GetIntVal("openLastZen"))
			{
				zSTRING path = options.GetVal("openLastZenPath");

				if (path.Length() > 0)
				{
					auto loadZen = (loadZenAuto)GetProcAddress(theApp.module, "LoadZenAuto");
					Stack_PushString(path.Upper());
					loadZen();
				}
			}
		}
	}


}