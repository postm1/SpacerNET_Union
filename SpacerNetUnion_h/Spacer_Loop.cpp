
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	zCArray<HWND> childWindows;
	Timer mainTimer;



	bool SetForegroundWindowEx(HWND Wnd)
	{
		BOOL Success = FALSE;

#ifndef COMPILING_SPACER
		// Validate param
		if ((Wnd == NULL))
			return FALSE;

		DWORD	OwnProcessId = GetCurrentProcessId();
		UINT	OwnThreadId = GetCurrentThreadId();
		HWND	TargetWndHandle = GetForegroundWindow();
		DWORD	TargetProcessId = GetCurrentProcessId();
		UINT	TargetThreadId = GetWindowThreadProcessId(TargetWndHandle, &TargetProcessId);

		// Window already owns the input focus
		if ((Wnd == TargetWndHandle))
			return TRUE;
		// Our process already owns the input focus
		if (OwnProcessId == TargetProcessId)
		{
			SetForegroundWindow(Wnd);
			return TRUE;
		}

		// Attach to input queue of the thread of the window which has the focus
		// (if not our won thread, and so on)
		if ((TargetWndHandle != NULL) && (TargetThreadId != 0))
		{
			if ((OwnThreadId == TargetThreadId))
			{
				Success = (SetFocus(Wnd) != NULL);
			}
			else if (AttachThreadInput(OwnThreadId, TargetThreadId, TRUE))
			{
				// Set focus
				Success = (SetFocus(Wnd) != NULL);
				// Detach input queue
				AttachThreadInput(OwnThreadId, TargetThreadId, FALSE);
			}
		}

		// Call original function if no success
		if (!Success)
			Success = SetForegroundWindow(Wnd);

		// [EDENFELD] neuen Code musste ich erst mal wieder deaktivierten, da es hдufiger zu Crashes bei Focusverlust fьhrt als der alte Code
#else
		SetForegroundWindow(Wnd);
		return TRUE;
#endif

		return (Success != 0);
	}

	bool IsAppForeground()
	{
		static auto call = (appActivePointer)GetProcAddress(theApp.module, "IsAppActive");

		bool result = call((uint)hWndApp);
		return result;
	}

	void sysSetFocus()
	{
		SetWindowPos(hWndApp, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE);
		SetForegroundWindowEx(hWndApp);
		SetFocus(hWndApp);

		ShowWindow(hWndApp, SW_RESTORE);							// Restore window position
		ShowWindow(hWndApp, SW_SHOWNORMAL);							// Restore window position
	}

	//0x00503630 void __cdecl HandleFocusLoose(void)
	void __cdecl HandleFocusLoose(void)
	{
		XCALL(0x00503630);
	}
	

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
				//AppWndProc->HandleFocusLoose[zrenderer->SetSurfaceLost(TRUE);, 
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

					//mainTimer.Attach();
					mainTimer.ClearUnused();
					/*
					//if (!IsAppForeground() && !ogame->singleStep)
					if ((GetForegroundWindow() != hWndApp && GetForegroundWindow() != theApp.mainWin) && !ogame->singleStep)
					{
						zrenderer->SetSurfaceLost(TRUE);
						ogame->Pause();
						sysSetFocus();
					}

					*/

					if ((GetForegroundWindow() != hWndApp && GetForegroundWindow() != theApp.mainWin))
					{

						//ogame->game_drawall = false;
						//zrenderer->SetSurfaceLost(TRUE);
						//zrenderer->Vid_SetScreenMode(zRND_SCRMODE_WINDOWED);
					}
					
					

					

				}
				else if (ogame->singleStep)
				{
					if ((GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin))
					{
						//ogame->game_drawall = true;
						//ogame->Unpause();
						//sysSetFocus();
						//zrenderer->Vid_SetScreenMode(zRND_SCRMODE_HIDE);
						//zrenderer->SetSurfaceLost(FALSE);
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

		if (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin || GetForegroundWindow() == theApp.vobsWin)
		{
			theApp.Loop();

		}

		RenderVobsLoop();

		theApp.OnLevelReady();

		mm.Loop();
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
			else if (theApp.pickedVob)
			{
				PrintDebug(ToStr GetVobNameSafe(theApp.pickedVob) + " Ref: " + ToStr theApp.pickedVob->refCtr);

				if (theApp.options.GetIntVal("showVobDist"))
				{
					PrintDebug(ToStr GetLang("UNION_DIST") + ToStr(int)Dist(ogame->GetCamera()->connectedVob, theApp.pickedVob));
				}

			}

			if (ogame->GetGameWorld() && ogame->GetGameWorld()->GetBspTree() && ogame->GetCamera() && ogame->GetCamera()->connectedVob)
			{
				zSTRING* name = (zSTRING*)ogame->GetGameWorld()->GetBspTree()->GetSectorNameVobIsIn(ogame->GetCamera()->connectedVob);


				if (name)
				{
					PrintDebug("Portal: " + *name);
				}
				else
				{
					PrintDebug("No portal");
				}
				
				
			}

			if (ogame->GetGameWorld() && ogame->GetGameWorld()->GetBspTree())
			{
				mm.ShowInfo();
				//mm.DrawAxes();
				
			}
			
			/*
			int day, hour, min, nFont;
			ogame->GetTime(day, hour, min);

			if (min >= 10)
			{
				PrintDebug(zSTRING(day + 1) + " " + zSTRING(hour) + ":" + zSTRING(min));
			}
			else
			{
				PrintDebug(zSTRING(day + 1) + " " + +zSTRING(hour) + ":0" + zSTRING(min));

			}
			*/
		


			//PrintDebug(ToStr zCWorld::s_bWaveAnisEnabled);
			screen->SetFontColor(zCOLOR(255, 255, 255));

			
		}
	}

	void SpacerApp::KeysLoop()
	{

		/*
		if (zinput->KeyPressed(KEY_Z))
		{
			visualEditorActive = !visualEditorActive;
		}

		if (visualEditorActive)
		{
			return;
		}
		*/


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
			ToggleWindowsVisible();
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

		if (keys.KeyPressed("LIGHT_RAD_INC", true))
		{
			if (playerLightInt <= 20000)
			{
				playerLightInt = playerLightInt + 500;
			}

			print.PrintRed(ToStr GetLang("UNION_LIGHT_RAD_INC") + " " + ToStr playerLightInt);
		}

		if(keys.KeyPressed("LIGHT_RAD_DEC", true))
		{
			if (playerLightInt >= 500)
			{
				playerLightInt = playerLightInt - 500;
			}

			print.PrintRed(ToStr GetLang("UNION_LIGHT_RAD_DEC") + " "+ ToStr playerLightInt);
		}

		if (keys.KeyPressed("LIGHT_RAD_ZERO", true))
		{
			playerLightInt = 0;

			print.PrintRed(ToStr GetLang("UNION_LIGHT_RAD_ZERO"));

#if ENGINE > Engine_G1
			if (zCSkyControler::GetActiveSkyControler())
			{
				zCSkyControler::GetActiveSkyControler()->SetLightDirty();
			}
#endif
			playerLightInt = 500;
#if ENGINE > Engine_G1
			if (zCSkyControler::GetActiveSkyControler())
			{
				zCSkyControler::GetActiveSkyControler()->SetLightDirty();
			}
#endif
			playerLightInt = 0;
		}

	}


	void SpacerApp::CheckForBadPlugins()
	{
		if (pluginsChecked || theApp.options.GetIntVal("showBadPluginsMsg") == 0) return;

		pluginsChecked = true;

		
		whiteArrayPlugins.Insert("SPACERUNIONNET.DLL");
		whiteArrayPlugins.Insert("SPACERUNIONINTERFACE.DLL");
		whiteArrayPlugins.Insert("ZMOUSEFIX.DLL");
		whiteArrayPlugins.Insert("ZBINKFIX.DLL");
		whiteArrayPlugins.Insert("ZACTIVATECTRL.DLL");

		const CList<CPlugin>* pluginList = CPlugin::GetPluginList().GetNext();
		while (pluginList) {
			const CPlugin* plug = pluginList->GetData();
			if (plug) {
				zSTRING name = plug->GetName();
				if (!whiteArrayPlugins.IsInList(name)) {
					blackArrayPlugins.Insert(name);
				}
			}

			pluginList = pluginList->GetNext();
		}

		if (blackArrayPlugins.GetNumInList() > 0) {
			zSTRING message = GetLang("CHECK_BADPLUGINS_MSG");
			

			zSTRING mess2 = "\n\n";
			for (int i = 0; i < blackArrayPlugins.GetNumInList(); i++) {
				if (i > 0) {
					mess2 += ", ";
				}
				mess2 += blackArrayPlugins.GetSafe(i);
			}
			mess2 += ".";
			message += mess2;

			MessageBox(0, message, 0, 0);
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

		CheckForBadPlugins();

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