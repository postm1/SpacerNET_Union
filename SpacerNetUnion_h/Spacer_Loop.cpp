
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

	void GetVobInfo() {

		zTBBox3D bbox;
		zCArray<zCVob*> found;
		zREAL distance = 2000;

		bbox.maxs = bbox.mins = ogame->GetCamera()->GetVob()->GetPositionWorld();
		bbox.maxs[0] += distance; bbox.maxs[1] += distance; bbox.maxs[2] += distance;
		bbox.mins[0] -= distance; bbox.mins[1] -= distance; bbox.mins[2] -= distance;
		ogame->GetWorld()->CollectVobsInBBox3D(found, bbox);


		zCVobSpot* spot = NULL;
		zCVobWaypoint* wp = NULL;
		zCOLOR color;
		zPOINT3 wsPoint1, csPoint1;
		zPOINT2 ssPoint1;
		color = GFX_RED;

		screen->SetFontColor(zCOLOR(255, 255, 255));

		int distDraw = 4000;

		// Nach zCVobSpots durchsuchen
		for (int i = 0; i<found.GetNumInList(); i++)
		{
			auto vob = found.GetSafe(i);

			if (vob && vob->GetVobName() == "VOB_TEST_NAME" && vob->GetVisual())
			{
				zCProgMeshProto* progMesh = static_cast<zCProgMeshProto*>(vob->GetVisual());
				// der einfachheit halber nehmen wir das erste material was ok ist 
				for (int i = 0; i<progMesh->numSubMeshes; i++)
				{
					if (auto mat = progMesh->subMeshList[i].material)
					{
						//mat->noCollDet = 1;
						PrintDebug(mat->GetName() + " " + Z mat->GetNoCollDet() + ", NoDet: " + Z mat->noCollDet);
					}
				}
			}
		}
	}



	void SpacerApp::RenderPortals()
	{
		static zCOLOR PORTALINFO_PC_COLOR = zCOLOR(128, 128, 128);	// Color for player portal info (RGB), default: gray color
		static zCOLOR PORTALINFO_CAM_COLOR = zCOLOR(0, 255, 0);
		static int PORTALINFO_TEXT_POS[2] = { 10, 10 };



		if (ogame->GetGameWorld() && ogame->GetGameWorld()->GetBspTree() && ogame->GetCamera() && ogame->GetCamera()->connectedVob)
		{
			zSTRING* name = (zSTRING*)ogame->GetGameWorld()->GetBspTree()->GetSectorNameVobIsIn(ogame->GetCamera()->connectedVob);


			if (name)
			{
				PrintDebug("Portal: " + *name);
			}
			else
			{
				//PrintDebug("No portal");
			}

		}


		zSTRING* camPortalName = NULL;
		zSTRING* playerPortalName = NULL;

		if (ogame->camVob)
			// getting portal name for camera object
			camPortalName = (zSTRING*)ogame->camVob->GetSectorNameVobIsIn();

		if (player)
			// getting portal name for player object
			playerPortalName = (zSTRING*)player->GetSectorNameVobIsIn();

		zCOLOR fontColor_old = screen->fontColor;

		/*
		// output info-text about camera vob
		screen->fontColor = PORTALINFO_CAM_COLOR;
		screen->Print(zPixelX(PORTALINFO_TEXT_POS[0]), zPixelY(PORTALINFO_TEXT_POS[1]), Z "Cam portal: " + (camPortalName ? *camPortalName : ""));

		// output info-text about player vob
		screen->fontColor = PORTALINFO_PC_COLOR;
		screen->Print(zPixelX(PORTALINFO_TEXT_POS[0]), zPixelY(PORTALINFO_TEXT_POS[1]) + screen->FontY(), Z "PC  portal: " + (playerPortalName ? *playerPortalName : ""));
		
		// restore screen font color
		screen->fontColor = fontColor_old;
		*/
		//----------------------------------------------------------------


		// ptr on the sector
		zCBspSector* sector = NULL;

		// protect ptr's
		if (playerPortalName)
		{
			// getting sector ptr for player object
			sector = player->groundPoly->material->bspSectorFront;

			// run through for all polygons of the portal
			for (int i = 0; i < sector->sectorPortals.GetNum(); i++)
			{
				// drawing polygon wire
				zCPolygon* poly = sector->sectorPortals[i];

				DrawPolygon(poly, PORTALINFO_PC_COLOR);
			}
		}


		// protect ptr's
		if (camPortalName)
		{
			// getting sector ptr for camera object
			sector = ogame->camVob->groundPoly->material->bspSectorFront;

			// run through for all polygons of the portal
			for (int i = 0; i < sector->sectorPortals.GetNum(); i++)
			{
				// drawing polygon wire
				zCPolygon* poly = sector->sectorPortals[i];
				DrawPolygon(poly, PORTALINFO_CAM_COLOR);

			}
		}
	}

	void SpacerApp::PluginLoop()
	{
		if (theApp.isExit )
		{
			return;
		}
		// MessageBox(0, "Game_Loop", 0, 0);



		
		
		ResetPrintDebug();
		print.Loop();

		if (mainTimer[1u].Await(150))
		{
			if (options.GetIntVal("alwaysShutSounds"))
			{
				if (zsound) zsound->StopAllSounds();
			}
		}
		

		//print.PrintRed(Z(int)player);

		if (theApp.g_bIsPlayingGame)
		{

			GameLoop();
			return;
		}

		if (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin || GetForegroundWindow() == theApp.vobsWin)
		{
			theApp.Loop();

		}


		RenderVobsLoop();

		theApp.OnLevelReady();

		nograss.Loop();

		mm.Loop();
		theApp.KeysLoop();
		theApp.TriggerLoop();
		theApp.camMan.Loop();
		
		pfxManager.Loop();

		itemsLocator.Loop();
		


		

		if (screen && ogame->GetCamera() && ogame->GetWorld())
		{
			zVEC3 pos = ogame->GetCamera()->activeCamPos;

			screen->SetFont("FONT_OLD_10_WHITE_HI.TGA");
			screen->SetFontColor(zCOLOR(0, 255, 0));


			//GetVobInfo();
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

			if (theApp.options.GetIntVal("showPortalsInfo"))
			{
				RenderPortals();
			}
			


			/*
			if (zmusic)
			{
				auto theme = zmusic->GetActiveTheme();

				if (theme)
				{
					PrintDebug("Theme: " + theme->fileName + " " + theme->name);

				}


				
				
			}
			*/
			
			
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


			
			if (theApp.showRespawnOnVobs)
			{

				zTBBox3D bbox;
				zCArray<zCVob*> found;
				zREAL distance = 4500;

				bbox.maxs = bbox.mins = ogame->GetCamera()->GetVob()->GetPositionWorld();
				bbox.maxs[0] += distance; bbox.maxs[1] += distance; bbox.maxs[2] += distance;
				bbox.mins[0] -= distance; bbox.mins[1] -= distance; bbox.mins[2] -= distance;
				ogame->GetWorld()->CollectVobsInBBox3D(found, bbox);

				
				zCVobSpot* spot = NULL;
				zCVobWaypoint* wp = NULL;
				zCOLOR color;
				zPOINT3 wsPoint1, csPoint1;
				zPOINT2 ssPoint1;
				color = GFX_RED;

				screen->SetFontColor(zCOLOR(255, 255, 255));

				int distDraw = 4000;

				// Nach zCVobSpots durchsuchen
				for (int i = 0; i<found.GetNumInList(); i++)
				{
					auto vob = found.GetSafe(i);

					if (vob)
					{
						auto& foundPair = theApp.respawnShowList[vob->GetVobName()];

						if (!foundPair.IsNull())
						{
							CString info = "";

							for (int j = 0; j < foundPair.GetValue()->monsters.GetNumInList(); j++)
							{
								info += foundPair.GetValue()->monsters.GetSafe(j) + " | ";
							}

							info = info.Cut(info.Length() - 3, 3);

							if (spot = dynamic_cast<zCVobSpot*>(found[i]))
							{

								wsPoint1 = spot->GetPositionWorld() + zVEC3(0, 350, 0);
								csPoint1 = ogame->GetCamera()->Transform(wsPoint1);
								if (csPoint1[VZ] >= 0)	ogame->GetCamera()->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
								if ((csPoint1[VZ]<distDraw) && (csPoint1[VZ]>0))
									screen->Print(screen->anx(ssPoint1[VX]), screen->any(ssPoint1[VY]), info);
							};

							if (wp = dynamic_cast<zCVobWaypoint*>(found[i]))
							{

								wsPoint1 = wp->GetPositionWorld() + zVEC3(0, 150, 0);
								csPoint1 = ogame->GetCamera()->Transform(wsPoint1);
								if (csPoint1[VZ] >= 0)	ogame->GetCamera()->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
								if ((csPoint1[VZ]<distDraw) && (csPoint1[VZ]>0))
									screen->Print(screen->anx(ssPoint1[VX]), screen->any(ssPoint1[VY]), info);
							};
						}
					}
					
					
				}

			}
			
			screen->SetFontColor(zCOLOR(255, 255, 255));
			
			/*

			if (leakPolyList)
			{
				for (int i = 0; i < leakPolyList->GetNumInList(); i++)
				{
					auto poly = leakPolyList->GetSafe(i);

					if (poly)
					{
						poly->DrawWire(zCOLOR(255, 0, 0));
					}
				}
			}
			*/
			
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

		if (keys.KeyPressed("GAME_MODE", true))
		{
			ToggleGame();
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
		return;

		if (pluginsChecked || theApp.options.GetIntVal("showBadPluginsMsg") == 0) return;

		pluginsChecked = true;

		
		whiteArrayPlugins.Insert("SPACERUNIONNET.DLL");
		whiteArrayPlugins.Insert("SPACERUNIONINTERFACE.DLL");
		whiteArrayPlugins.Insert("ZMOUSEFIX.DLL");
		whiteArrayPlugins.Insert("ZBINKFIX.DLL");
		whiteArrayPlugins.Insert("ZACTIVATECTRL.DLL");
		whiteArrayPlugins.Insert("ZWE_CONTROLLER.DLL");


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

			/*
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
			*/
		}
	}


}