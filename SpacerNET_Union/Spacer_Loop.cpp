
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	zCArray<HWND> childWindows;
	Timer mainTimer;



	bool SetForegroundWindowEx(HWND Wnd)
	{
		BOOL Success = FALSE;

		if ((Wnd == NULL))
			return FALSE;

		DWORD	OwnProcessId = GetCurrentProcessId();
		UINT	OwnThreadId = GetCurrentThreadId();
		HWND	TargetWndHandle = GetForegroundWindow();
		DWORD	TargetProcessId = GetCurrentProcessId();
		UINT	TargetThreadId = GetWindowThreadProcessId(TargetWndHandle, &TargetProcessId);


		if ((Wnd == TargetWndHandle))
			return TRUE;

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
				Success = (SetFocus(Wnd) != NULL);

				AttachThreadInput(OwnThreadId, TargetThreadId, FALSE);
			}
		}

		if (!Success)
			Success = SetForegroundWindow(Wnd);

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

		ShowWindow(hWndApp, SW_RESTORE);							
		ShowWindow(hWndApp, SW_SHOWNORMAL);							
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
				sysEvent();

				POINT   mPos;
				GetCursorPos(&mPos);
				zCView::GetInput();


				if (GetGame() && gameSession && ogame->GetWorld() && ogame->GetCamera() && theApp.IsAWorldLoaded() && ogame->GetWorld()->compiled)
				{

					theApp.PreRender();

					gameSession->Render();

					theApp.AfterRender();

					

					gameSession->RenderBlit();

				
					mainTimer.ClearUnused();


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

		for (int i = 0; i<found.GetNumInList(); i++)
		{
			auto vob = found.GetSafe(i);

			if (vob && vob->GetVobName() == "VOB_TEST_NAME" && vob->GetVisual())
			{
				zCProgMeshProto* progMesh = static_cast<zCProgMeshProto*>(vob->GetVisual());
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
		static zCOLOR PORTALINFO_PC_COLOR = zCOLOR(128, 128, 128);	
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



		zCBspSector* sector = NULL;


		if (playerPortalName)
		{

			sector = player->groundPoly->material->bspSectorFront;

			for (int i = 0; i < sector->sectorPortals.GetNum(); i++)
			{

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

	zCArray<zSTRING> visualNotFound;

	void SpacerApp::ExtractVisualInfoShow(zCVisual* visual, int& start, int startX, int& longestLine, zSTRING& textToCopy)
	{
		if (!visual)
		{
			return;
		}


		// 3DS
		if (auto pProgMesh = visual->CastTo<zCProgMeshProto>())
		{
			auto trisCount = pProgMesh->GetNumTri();

			zSTRING text = "TrisCount: ";
			int lenOffset = pViewVobInfo->FontSize(text);

			pViewVobInfo->SetFontColor(zCOLOR(0, 255, 0));
			pViewVobInfo->Print(startX, start, text);

			pViewVobInfo->SetFontColor(zCOLOR(255, 255, 255, 255));
			pViewVobInfo->Print(startX + lenOffset, start, Z trisCount);
			start += 220;

			text = "\n" + text + Z trisCount;

			textToCopy += text;

			for (int i = 0; i < pProgMesh->numSubMeshes; i++)
			{

				auto mat = pProgMesh->subMeshList[i].material;

				

				if (mat && mat->texture)
				{
					zSTRING text = GetMaterialInfoAsString(mat);

					auto textureName = mat->texture->GetObjectName();

					auto checkName = textureName;

					checkName.Replace(".TGA", "-C.TEX");

					if (!Union_FileExists(checkName))
					{
						if (!visualNotFound.IsInList(textureName))
						{
							visualNotFound.InsertEnd(textureName);

							cmd << "NoTexture: " << textureName << endl;
						}
					}
					

					int curLineSize = pViewVobInfo->FontSize(text);

					if (curLineSize > longestLine)
					{
						longestLine = curLineSize;
					}

					pViewVobInfo->Print(startX, start, text);

					textToCopy += "\n" + text;

					start += 220;
				}
			}

			//cmd << "===========" << endl;
		}

		// MMS
		if (auto pMorph = visual->CastTo<zCMorphMesh>())
		{




			if (pMorph->morphMesh)
			{
				for (int i = 0; i < pMorph->morphMesh->numSubMeshes; i++)
				{


					auto mat = pMorph->morphMesh->subMeshList[i].material;

					if (mat && mat->texture)
					{
						zSTRING text = GetMaterialInfoAsString(mat);

						int curLineSize = pViewVobInfo->FontSize(text);

						if (curLineSize > longestLine)
						{
							longestLine = curLineSize;
						}

						pViewVobInfo->Print(startX, start, text);

						textToCopy += "\n" + text;

						start += 220;
					}
				}
			}

		}

		// ASC/MDS
		if (auto pModel = visual->CastTo<zCModel>())
		{
			for (int i = 0; i < pModel->meshSoftSkinList.GetNum(); i++)
			{
				for (int n = 0; n < pModel->meshSoftSkinList[i]->numSubMeshes; n++)
				{
					if (!pModel->meshSoftSkinList[i])
					{
						continue;
					}

					auto mat = pModel->meshSoftSkinList[i]->subMeshList[n].material;

					if (mat && mat->texture)
					{
						zSTRING text = GetMaterialInfoAsString(mat);

						int curLineSize = pViewVobInfo->FontSize(text);

						if (curLineSize > longestLine)
						{
							longestLine = curLineSize;
						}

						pViewVobInfo->Print(startX, start, text);

						textToCopy += "\n" + text;

						start += 220;
					}
					//cmd << pModel->meshSoftSkinList[i]->subMeshList[n].material->GetObjectName() << endl;
				}
			}

			// search in all the nodes
			for (int i = 0; i < pModel->nodeList.GetNum(); i++)
			{
				if (!pModel->nodeList[i])
				{
					continue;
				}

				// если нет визуала узла
				if (!pModel->nodeList[i]->nodeVisual)
					continue;

				ExtractVisualInfoShow(pModel->nodeList[i]->nodeVisual, start, startX, longestLine, textToCopy);

			}
		}
	}
	void SpacerApp::ShowVobInfo()
	{
		if (!pViewVobInfo)
		{
			pViewVobInfo = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
			pViewVobInfo->SetFont("FONT_OLD_10_WHITE.TGA");
			pViewVobInfo->SetFontColor(zCOLOR(255, 255, 255, 255));
			pViewVobInfoBack = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
			pViewVobInfoBack->SetAlphaBlendFunc(zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND);
			screen->InsertItem(pViewVobInfoBack);
			screen->InsertItem(pViewVobInfo);
		}

		pViewVobInfo->ClrPrintwin();


		if (!showVobVisualInfo)
		{
			pViewVobInfoBack->InsertBack("");
			return;
		}

		auto pVob = GetSelectedVob();

		if (!pVob)
		{
			pVob = currentVobRender;
		}

		if (!pVob)
		{
			pVob = currenItemRender;
		}

		if (pVob)
		{
			

			pViewVobInfo->SetFontColor(zCOLOR(255, 255, 255, 255));

			zSTRING textToCopy;

			
			int startX = options.GetIntVal("showVisualInfoX");
			int startY = options.GetIntVal("showVisualInfoY");
			int saveStartY = startY;
			int longestLine = 0;

			if (auto visual = pVob->visual)
			{

				pViewVobInfo->SetFontColor(zCOLOR(209, 162, 32));
				zSTRING copyInfo = "(" + Z GetLang("VISUALINFO_COPY") + ")";
				pViewVobInfo->Print(startX, startY, copyInfo);
				startY += 220;

				textToCopy = "VisualName: ";

				longestLine = pViewVobInfo->FontSize(textToCopy);

				// output VisualName as diff color
				pViewVobInfo->SetFontColor(zCOLOR(0, 255, 0));
				pViewVobInfo->Print(startX, startY, textToCopy);

				// output Visual info with usual color
				pViewVobInfo->SetFontColor(zCOLOR(255, 255, 255, 255));
				pViewVobInfo->Print(startX + longestLine, startY, visual->GetVisualName());

				textToCopy += visual->GetVisualName();

				
				startY += 220;

				pViewVobInfo->SetFontColor(zCOLOR(255, 255, 255, 255));

				ExtractVisualInfoShow(visual, startY, startX, longestLine, textToCopy);
			}
			else
			{
				pViewVobInfoBack->InsertBack("");
				return;
			}

			


			int padding = 50;
			
			int posY = saveStartY - padding;
			int linesCount = textToCopy.Search('\n', 0);

			pViewVobInfoBack->InsertBack("BLACK.TGA");
			pViewVobInfoBack->SetPos(startX - padding, posY);
			pViewVobInfoBack->SetSize(longestLine + padding * 2, startY - saveStartY + padding * 3);
			pViewVobInfoBack->alpha = 165;

			if (wasCopiedPressed)
			{
				textToCopy.TrimLeft(' ');

				print.PrintRed(GetLang("VOB_INFO_VISUAL_COPIED"));

				cmd << textToCopy << endl << endl;

				const char* output = textToCopy.ToChar();

				Clipboard(output);

				wasCopiedPressed = false;
			}

			if (auto pItem = pVob->CastTo <oCItem>())
			{
				PrintItemName(pItem);
			}
		}
		else
		{
			pViewVobInfoBack->InsertBack("");
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

		if (mainTimer[TIMER_ID_SHUTDOWN_SOUNDS].Await(150))
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

		debug.Loop();
		RenderVobsLoop();

		theApp.OnLevelReady();

		nograss.Loop();

		mm.Loop();
		mf.Loop();
		theApp.KeysLoop();
		theApp.TriggerLoop();
		theApp.camMan.Loop();
		
		pfxManager.Loop();

		itemsLocator.Loop();
		Loop_PFXEditor();
		DrawLightSphere_Loop();


		if (screen && ogame->GetCamera() && ogame->GetWorld())
		{
			screen->SetFont("FONT_OLD_10_WHITE_HI.TGA");

			if (theApp.showAfterWorldCompileMessage)
			{
				screen->SetFontColor(zCOLOR(255, 0, 0));
				screen->PrintCX(3000, zSTRING(GetLang("WORLD_COMPILED_PRE_WORK_WARN")));
				screen->SetFontColor(zCOLOR(255, 255, 255));
			}

			if (theApp.showScreenInfo)
			{
				zVEC3 pos = ogame->GetCamera()->activeCamPos;
				zVEC3 rot = ogame->GetCamera()->camMatrix.zMAT4::GetEulerAngles() * DEGREE;


				// Fixing float output for Roll angle
				if (rot[2] < 0.00001 && rot[2] > 0)
				{
					rot[2] = 0;
				}

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

					PrintDebug((zSTRING(GetLang("UNION_CAM_ROT")) + "(" + zSTRING(rot.n[0], 6) + zSTRING(", ") + zSTRING(rot.n[1], 6)

						+ zSTRING(", ") + zSTRING(rot.n[2], 6) + zSTRING(")"))); // Z-axis obviously could remain zSTRING("0.0") because it's unnecessary.


					if (theApp.pickedVob)
					{
						auto pos = theApp.pickedVob->GetPositionWorld();

						PrintDebug((zSTRING(GetLang("UNION_VOB_POS")) + "(" + zSTRING(pos.n[0], 6) + zSTRING(", ") + zSTRING(pos.n[1], 6)

							+ zSTRING(", ") + zSTRING(pos.n[2], 6) + zSTRING(")")));
					}

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


				if (GetSelectedTool() == TM_BBOXEDIT)
				{
					screen->SetFontColor(zCOLOR(255, 0, 0));

					PrintDebug(zSTRING(GetLang("TOOL_BBOX_EDIT_MODE_SELECTED")));
					screen->SetFontColor(zCOLOR(255, 255, 255));
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
					zREAL distance = theApp.showRespawnOnVobsRadius;

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

					int distDraw = distance - 500;
					for (int i = 0; i < found.GetNumInList(); i++)
					{
						auto vob = found.GetSafe(i);

						if (vob)
						{
							spot = dynamic_cast<zCVobSpot*>(found[i]);
							wp = dynamic_cast<zCVobWaypoint*>(found[i]);


							if (spot || wp)
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

									if (spot)
									{

										wsPoint1 = spot->GetPositionWorld() + zVEC3(0, 350, 0);
										csPoint1 = ogame->GetCamera()->Transform(wsPoint1);
										if (csPoint1[VZ] >= 0)	ogame->GetCamera()->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
										if ((csPoint1[VZ] < distDraw) && (csPoint1[VZ] > 0))
											screen->Print(screen->anx(ssPoint1[VX]), screen->any(ssPoint1[VY]), info);
									};

									if (wp)
									{

										wsPoint1 = wp->GetPositionWorld() + zVEC3(0, 150, 0);
										csPoint1 = ogame->GetCamera()->Transform(wsPoint1);
										if (csPoint1[VZ] >= 0)	ogame->GetCamera()->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
										if ((csPoint1[VZ] < distDraw) && (csPoint1[VZ] > 0))
											screen->Print(screen->anx(ssPoint1[VX]), screen->any(ssPoint1[VY]), info);
									};
								}
							}


						}


					}

				}

				screen->SetFontColor(zCOLOR(255, 255, 255));

				if (theApp.pickedVob)
				{
					if (auto pMob = theApp.pickedVob->CastTo<oCMOB>())
					{
						screen->SetFontColor(zCOLOR(229, 182, 52));

						PrintMobName(pMob);

						screen->SetFontColor(zCOLOR(255, 255, 255));
					}
				}
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



		ShowVobInfo();


		if (mobInterSlotsVobs.GetNumInList() == 0)
		{
			for (int i = 0; i < 10; i++)
			{
				auto pVobNew = new zCVob();
				pVobNew->SetVobName("SPACER_VOB_MOBINTER_SLOT_" + Z i);
				pVobNew->dontWriteIntoArchive = true;
				pVobNew->SetCollDet(FALSE);
				pVobNew->ignoredByTraceRay = true;

				pVobNew->SetVisual("SPACERNET_MODEL_SLOT_ANI.3DS");

				theApp.nextInsertBlocked = true;
				ogame->GetWorld()->AddVob(pVobNew);
				theApp.nextInsertBlocked = false;

				pVobNew->SetShowVisual(FALSE);
				pVobNew->SetPositionWorld(zVEC3(0, 0, 0));

				mobInterSlotsVobs.InsertEnd(pVobNew);
			}
		}


		


		auto vob = GetSelectedVob();


		if (vob)
		{
			if (theApp.options.GetIntVal("bShowMobInterSlots"))
			{

				HideMobInterHelpVobs();

				if (mobInterSlotsVobs.GetNumInList() == 0) return;

				if (auto mob = vob->CastTo<oCMobInter>())
				{
					auto savePos = mob->GetPositionWorld();

					//reset startpos for ScanIdealPositions
					mob->startPos = zVEC3(0, 0, 0);
					mob->ScanIdealPositions();


					zCList<TMobOptPos>* data = mob->optimalPosList.GetNextInList();

					int counter = 0;

					while (data)
					{
						if (auto entry = data->GetData())
						{

							auto startPos = entry->trafo.GetTranslation();

							auto pVob = mobInterSlotsVobs.GetSafe(counter);

							if (pVob)
							{
								//print.PrintRed(entry->trafo.GetAtVector().ToString());
								pVob->SetPositionWorld(startPos);
								pVob->SetHeadingAtWorld(entry->trafo.GetAtVector());
								pVob->SetShowVisual(TRUE);
							}
						}

						counter += 1;
						data = data->GetNextInList();
					}

				}
			}
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

		if (keys.KeyPressed("TOGGLE_SCREENINFO", true))
		{
			showScreenInfo = !showScreenInfo;
		}


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


		if (keys.KeyPressed("TOGGLE_MUSIC", true))
		{
			int opt = zoptions->ReadInt("SOUND", "musicEnabled", 0);

			opt = !opt;


			if (opt)
			{
				print.PrintGreen(ToStr GetLang("UNION_MUSIC_ON"));
				zoptions->WriteInt("SOUND", "musicEnabled", 1, 0);
				zCMusicSystem::DisableMusicSystem(FALSE);
				zCZoneMusic::SetAutochange(TRUE);
			}
			else
			{
				print.PrintRed(ToStr GetLang("UNION_MUSIC_OFF"));
				zoptions->WriteInt("SOUND", "musicEnabled", 0, 0);
				zCZoneMusic::SetAutochange(FALSE);
				zCMusicSystem::DisableMusicSystem(TRUE);

				if (zsound) {
					//zsound->StopAllSounds();
				}
			}

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

#if ENGINE >= Engine_G2
			if (zCSkyControler::GetActiveSkyControler())
			{
				zCSkyControler::GetActiveSkyControler()->SetLightDirty();
			}
#endif
			playerLightInt = 500;
#if ENGINE >= Engine_G2
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
			
			CString renderStr = ToStr GetLang("UNION_RESOLUTION") + ToStr xRes + "x" + ToStr yRes;

			if (IsDx11Active())
			{
				renderStr += " (DX 11)";
			}
			else
			{
				renderStr += " (DX 7)";
			}

			view->Print(40, 1250, renderStr);

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