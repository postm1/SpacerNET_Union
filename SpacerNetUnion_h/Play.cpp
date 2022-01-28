// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	typedef zCListSort<zCVob>		oTVobList;
	typedef zCListSort<oCNpc>		oTVobListNpcs;
	typedef zCListSort<oCItem>		oTVobListItems;

	// Add your code here . . .
	oTVobListNpcs* npclist = NULL;
	oTVobListNpcs* npcListCopy = NULL;

	zVEC3 camPos;
	zSTRING CamModNormal("CAMMODNORMAL"); 
	zSTRING CamModRun("CAMMODRUN");
	zSTRING CamModLook("CAMMODLOOK");

	RECT rcOldClip;   // предыдущая область для ClipCursor
	bool winHide = false;

	void SpacerApp::GameLoop()
	{

		if (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin)
		{
			if (KeyPress(KEY_ESCAPE) || KeyPress(KEY_RETURN))
			{
				ToggleGame();
				zinput->ClearKeyBuffer();
			}

			if (!g_bIsPlayingGame) return;

			zCAICamera* aiCam = zCAICamera::GetCurrent();
			if (aiCam) {

				zBOOL param = false;

				aiCam->DoAI(player, param);
				aiCam->veloRot = 30;
				aiCam->veloTrans = 120;
				aiCam->moveTracker->inertiaTrans = 0.0;
				aiCam->moveTracker->inertiaTargetRot = 0;


				/*
				//aiCam->SetMode(CamModLook, 0);
				//aiCam->bestRotY = 30;
				PrintDebug("maxRotY: " + Z aiCam->maxRotY);
				PrintDebug("maxRotX: " + Z aiCam->maxRotX);
				PrintDebug("veloTrans: " + Z aiCam->veloTrans);
				PrintDebug("veloRot: " + Z aiCam->veloRot);
				PrintDebug("sysChanged: " + Z aiCam->sysChanged);
				PrintDebug("actAzi: " + Z aiCam->moveTracker->actAzi);
				PrintDebug("actElev: " + Z aiCam->moveTracker->actElev);

				PrintDebug("bestRotX: " + Z aiCam->bestRotX);

				PrintDebug("inertiaTrans: " + Z aiCam->moveTracker->inertiaTrans);
				PrintDebug("bMouseUsed: " + Z aiCam->moveTracker->bMouseUsed);
				PrintDebug("curcammode: " + Z aiCam->curcammode);
				*/
				
			}

			RECT clipRect;
			GetClientRect(mainWin, &clipRect);

			
 			ClipCursor(&clipRect);
			
		}
	}
	void SpacerApp::ToggleGame()
	{
		//print.PrintRed("Playing the game...");
		if (!g_bIsPlayingGame)
		{

			GetClipCursor(&rcOldClip);

			auto call = (callIntFunc)GetProcAddress(theApp.module, "GameModeToggleInterface");
			call(0);

			g_bIsPlayingGame = true;

			SetSelectedVob(NULL);

			// Parser Global Vars resetten
			parser->ResetGlobalVars();
			parser->SetInstance("SELF", NULL);
			parser->SetInstance("OTHER", NULL);
			parser->SetInstance("VICTIM", NULL);
			parser->SetInstance("ITEM", NULL);
			// SpawnListe leeren 
			ogame->spawnman->ClearList();
			// InfoMan clearen
			ogame->infoman = zNEW(oCInfoManager)(parser);
			// Clear Logs
			oCLogManager::GetLogManager().Clear();
			// Clear NewsManager

			ogame->newsman = zNEW(oCNewsManager());
			// Clear Soundos
			zsound->StopAllSounds();
			// delete all homeworld references of all CVisualFX Objects 
			oCVisualFX::PreSaveGameProcessing(TRUE);
			oCVisualFX::PostSaveGameProcessing();
			// Clear DebugList
			oCNpc::SetNpcAIDisabled(FALSE);
			ogame->GetSpawnManager()->SetSpawningEnabled(TRUE);

			// clear Cam
			//InitCamera(oCNpc::player->GetPositionWorld());

			oCVob::ClearDebugList();
			// Clear ProtalManager
			if (ogame->portalman) ogame->portalman->CleanUp();
			// Clear ObjectRoutineList
			ogame->ClearObjectRoutineList();
			ogame->CallScriptStartup();
			ogame->CallScriptInit();


			if (auto sym = parser->GetSymbol("RX_IsSpacetNet"))
			{
				parser->SetScriptInt("RX_IsSpacetNet", 1);
			}

			auto safePosPlayer = ogame->GetCamera()->connectedVob->GetPositionWorld();

			if (!player)
			{
				oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));
			}

			player->variousFlags = 2;
			player->SetAttribute(NPC_ATR_HITPOINTSMAX, 100000);
			player->CompleteHeal();
			

			ogame->EnterWorld(oCNpc::player, TRUE, "EDITOR_CAMERA_VOB");


			player->dontWriteIntoArchive = true;
			ogame->InitNpcAttitudes();
			player->ai_disabled = false;
			
			player->human_ai->PC_Turnings(1);
			player->SetCollDet(FALSE);
			player->SetPositionWorld(safePosPlayer);
			player->SetCollDet(TRUE);

			if (!hideWindows)
			{
				hideWindows = !hideWindows;

				winHide = true;
				hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
			}
			else
			{
				winHide = false;
			}
			

			while (ShowCursor(FALSE) >= 0);

			
			if (player->human_ai) player->human_ai->SetCamMode(CamModLook, 0);
			//changed[dennis]
			oCNpcFocus::SetFocusMode(FOCUS_NORMAL);


			zCAICamera* aiCam = zCAICamera::GetCurrent();
			if (aiCam) {
				aiCam->ClearTargetList();
				aiCam->SetTarget(oCNpc::player);
				aiCam->ReceiveMsg(zPLAYER_BEAMED);
				aiCam->SetMode(CamModLook, 0);

				ogame->GetCameraVob()->callback_ai = ogame->aiCam;
				ogame->GetCameraVob()->SetSleeping(FALSE);
				
			};
			
		}
		else
		{
			zVEC3 camFuturePos = player->GetPositionWorld();

			//SpacerIsActive = TRUE;
			
			// Parser Global Vars resetten
			parser->ResetGlobalVars();
			parser->SetInstance("SELF", NULL);
			parser->SetInstance("OTHER", NULL);
			parser->SetInstance("VICTIM", NULL);
			parser->SetInstance("ITEM", NULL);
			// SpawnListe leeren 
			ogame->spawnman->ClearList();
			// InfoMan clearen
			delete ogame->infoman;
			ogame->infoman = zNEW(oCInfoManager)(parser);
			// Clear Logs
			oCLogManager::GetLogManager().Clear();
			// Clear NewsManager
			delete ogame->newsman;
			ogame->newsman = zNEW(oCNewsManager());
			// Clear Soundos
			zsound->StopAllSounds();
			// delete all homeworld references of all CVisualFX Objects 
			oCVisualFX::PreSaveGameProcessing(TRUE);
			oCVisualFX::PostSaveGameProcessing();
			
			//ogame->GetCamera()->connectedVob->SetPositionWorld(oCNpc::player->GetPositionWorld());
			// Clear Cam ?!
			//CEditorDoc::doc->InitCamera(oCNpc::player->GetPositionWorld());
			// Clear DebugList
			oCVob::ClearDebugList();
			// Clear ProtalManager
			if (ogame->portalman) ogame->portalman->CleanUp();
			// Clear ObjectRoutineList
			ogame->ClearObjectRoutineList();



			auto originalCam = ogame->world->SearchVobByName("EDITOR_CAMERA_VOB");

			if (originalCam) theApp.RemoveVob(originalCam);

			


			ogame->CamInit();
			//ogame->EnvironmentInit();
			ogame->GetCamera()->connectedVob->SetVobName("EDITOR_CAMERA_VOB");
			ogame->GetCamera()->connectedVob->SetAI(0);
			//ogame->GetCamera()->connectedVob->bbox3D.mins = zVEC3(5, 5, 5);
			//ogame->GetCamera()->connectedVob->bbox3D.maxs = zVEC3(5, 5, 5);
			ogame->GetGameWorld()->AddVob(ogame->GetCamera()->connectedVob);
			ogame->GetCamera()->connectedVob->dontWriteIntoArchive = true;

			originalCam = ogame->GetCamera()->connectedVob;

			zCArray<zCVob*> targetList;

			ogame->world->SearchVobListByName("ZCAICAMERA", targetList);

			ogame->GetCamera()->SetVob(originalCam);

			player->dontWriteIntoArchive = true;
			player->showVisual = 0;
			theApp.OnRemoveVob(player);

			ogame->RemovePlayerFromWorld();

			// удаляем камеры из c# списков
			for (int i = 0; i < targetList.GetNumInList(); i++)
			{
				auto pVob = targetList.GetSafe(i);


				if (pVob)
				{
					pVob->dontWriteIntoArchive = true;
					theApp.OnRemoveVob(pVob);
				}

			}


			if (auto sym = parser->GetSymbol("RX_IsSpacetNet"))
			{
				parser->SetScriptInt("RX_IsSpacetNet", 1);
			}

			oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));

			oCNpc::player->GetModel();
			player->dontWriteIntoArchive = true;

			if (originalCam)
			{
				//ogame->GetCamera()->SetVob(originalCam);
				ogame->GetCamera()->connectedVob->SetAI(0);

				//ogame->GetGameWorld()->AddVob(ogame->GetCamera()->connectedVob);
				ogame->GetCamera()->connectedVob->dontWriteIntoArchive = true;
				ogame->GetCamera()->connectedVob->SetPositionWorld(camFuturePos);
				ogame->GetCamera()->connectedVob->dontWriteIntoArchive = true;
				//ogame->GetCamera()->SetVob(NULL);
				//theApp.RemoveVob(camVob);
			}
			//ogame->CamInit();
			//ogame->GetCamera()->connectedVob->SetVobName("EDITOR_CAMERA_VOB");

			g_bIsPlayingGame = false;

			ShowCursor(TRUE);

			if (winHide && hideWindows)
			{
				hideWindows = !hideWindows;

				hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();


			}
			
			ClipCursor(&rcOldClip);

			auto call = (callIntFunc)GetProcAddress(theApp.module, "GameModeToggleInterface");
			call(1);
		}


		
	}

	/*
	void SpacerApp::ToggleGame()
	{
		return;

		if (!g_bIsPlayingGame)
		{
			cmd << "Player ini ok 0 " << endl;

			if (!player)
			{
				cmd << "Player ini ok 1 " << endl;

				oCNpc::player = new oCNpc();
				oCNpc::player->InitByScript(parser->GetIndex(zSTRING("PC_HERO")), 1);
				if (!oCNpcFocus::focus) oCNpcFocus::InitFocusModes();

				
			}
			cmd << "Player ini ok";

			player->SetAttribute(NPC_ATR_HITPOINTSMAX, 10000);
			player->CompleteHeal();


			hideWindows = !hideWindows;

			hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();

			while (ShowCursor(FALSE) >= 0);

			zVEC3 safePos = ogame->GetCamera()->connectedVob->GetPositionWorld();

			oCNpc::SetNpcAIDisabled(FALSE);


			ogame->spawnman->SetSpawningEnabled(TRUE);

			cmd << "Player ini ok 2";

			player->SetMovLock(FALSE);
			player->SetSleeping(FALSE);
			player->SetPhysicsEnabled(TRUE);


			player->dontWriteIntoArchive = true;
			ogame->InitNpcAttitudes();

			player->SetPositionWorld(safePos);

			cmd << "Player ini ok 3";
		}
		else
		{
		}

		g_bIsPlayingGame = !g_bIsPlayingGame;
	}
	*/
	/*
	void SpacerApp::ToggleGame()
	{

		static zCCamera* cam = NULL;

		if (!g_bIsPlayingGame)
		{
			print.PrintRed("Start the game...");

			g_bIsPlayingGame = true;

			if (!player)
			{
				oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));
				player->dontWriteIntoArchive = true;
				player->SetSleeping(TRUE);
				player->SetPhysicsEnabled(FALSE);
				player->SetMovLock(TRUE);
				player->ai_disabled = true;
				player->RemoveVobFromWorld();
				player->Disable();
			}

			player->SetAttribute(NPC_ATR_HITPOINTSMAX, 10000);
			player->CompleteHeal();
			

			if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
			if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

			theApp.SetSelectedVob(NULL);


			hideWindows = !hideWindows;

			hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();

			while (ShowCursor(FALSE) >= 0);


			cam = ogame->GetCamera();

			zVEC3 safePos = cam->connectedVob->GetPositionWorld();

			oCNpc::SetNpcAIDisabled(FALSE);
			zCVob::SetShowHelperVisuals(FALSE);
			//dynamic_cast<oCGame*>(ogame)->GetSpawnManager()->SetSpawningEnabled(TRUE);

			parser->ResetGlobalVars();
			parser->SetInstance("SELF", NULL);
			parser->SetInstance("OTHER", NULL);
			parser->SetInstance("VICTIM", NULL);
			parser->SetInstance("ITEM", NULL);
			// SpawnListe leeren 
			ogame->spawnman->ClearList();
			// InfoMan clearen
			

			ogame->infoman = zNEW(oCInfoManager)(parser);
			// Clear Logs
			oCLogManager::GetLogManager().Clear();
			// Clear NewsManager

			ogame->newsman = zNEW(oCNewsManager());
			// Clear Soundos
			zsound->StopAllSounds();
			// delete all homeworld references of all CVisualFX Objects 
			oCVisualFX::PreSaveGameProcessing(TRUE);
			oCVisualFX::PostSaveGameProcessing();
			// Clear DebugList
			oCVob::ClearDebugList();
			// Clear ProtalManager
			if (ogame->portalman) ogame->portalman->CleanUp();
			// Clear ObjectRoutineList



			ogame->spawnman->SetSpawningEnabled(TRUE);

			oCWorld* world = dynamic_cast<oCWorld*>(ogame->GetWorld());

			npclist = (world ? world->GetVobListNpcs() : 0);
			npcListCopy = zNEW(oTVobListNpcs);

			if (npclist)
			{
				print.PrintRed(Z npclist->GetNumInList());

				for (int i = 0; i<npclist->GetNumInList(); i++)
				{
					if (npclist->Get(i))
					{
						oCNpc *nextNpc = npclist->Get(i);
						npcListCopy->Insert(nextNpc);
						nextNpc->AddRef();
						nextNpc->DeleteHumanAI();
						nextNpc->InitHumanAI();
					}
				};
			};


		

			world->worldName = "WESTCOAST";

			ogame->EnterWorld(oCNpc::player, TRUE, "EDITOR_CAMERA_VOB");

			ogame->scriptStartup = true;
			ogame->ClearObjectRoutineList();
			ogame->CallScriptStartup();
			ogame->CallScriptInit();
			player->ai_disabled = false;


			
			oCNpc::SetNpcAIDisabled(TRUE);
			ogame->spawnman->SetSpawningEnabled(FALSE);

			if (npclist)
			{
				for (int i = 0; i<npclist->GetNumInList(); i++)
				{
					oCNpc *nextNpc = npclist->Get(i);
					if (nextNpc)
					{
						if (npcListCopy->IsInList(nextNpc))
						{
							// war vor "PLAY THE GAME" auch schon drin. nur die stand ai wieder einschalten, dann is gut
							nextNpc->UseStandAI();
							npcListCopy->Remove(nextNpc);
							zRELEASE(nextNpc);
						}
						else
						{
							// wurde in CGameManager::Run() neu eingefьgt. Kann also komplett aus der Welt verschwinden
							world->RemoveVob(nextNpc);
						}
					}
				};
			};
			

			player->SetMovLock(FALSE);
			player->SetSleeping(FALSE);
			player->SetPhysicsEnabled(TRUE);
			

			player->dontWriteIntoArchive = true;
			ogame->InitNpcAttitudes();

			player->SetPositionWorld(safePos);


			


		}
		else
		{

			print.PrintRed("Stop the game...");

			g_bIsPlayingGame = false;

			zVEC3 safePos = player->GetPositionWorld();

			parser->ResetGlobalVars();
			parser->SetInstance("SELF", NULL);
			parser->SetInstance("OTHER", NULL);
			parser->SetInstance("VICTIM", NULL);
			parser->SetInstance("ITEM", NULL);
			// SpawnListe leeren 
			ogame->spawnman->ClearList();
			// InfoMan clearen
			delete ogame->infoman;
			ogame->infoman = zNEW(oCInfoManager)(parser);
			// Clear Logs
			oCLogManager::GetLogManager().Clear();
			// Clear NewsManager
			delete ogame->newsman;
			ogame->newsman = zNEW(oCNewsManager());
			// Clear Soundos
			zsound->StopAllSounds();
			// delete all homeworld references of all CVisualFX Objects 
			oCVisualFX::PreSaveGameProcessing(TRUE);
			oCVisualFX::PostSaveGameProcessing();
			// Clear Cam ?!
			ogame->GetCamera()->connectedVob->SetPositionWorld(safePos);
			// Clear DebugList
			oCVob::ClearDebugList();
			// Clear ProtalManager
			if (ogame->portalman) ogame->portalman->CleanUp();
			// Clear ObjectRoutineList
			ogame->ClearObjectRoutineList();
			//ogame->RemovePlayerFromWorld();
			OnRemoveVob(ogame->GetCamera()->connectedVob);
			ogame->GetWorld()->RemoveVob(ogame->GetWorld()->SearchVobByName("EDITOR_CAMERA_VOB"));
			

			//oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));
			oCNpc::player->GetModel();
			player->SetMovLock(TRUE);
			player->SetSleeping(TRUE);
			player->SetPhysicsEnabled(FALSE);
			player->ai_disabled = true;

			OnRemoveVob(player);
			player->RemoveVobFromWorld();
			player->Disable();
			
			//ogame->spawnman->DeleteNpc(player);

			auto world = ogame->GetWorld();

			if (zCAICamera::GetCurrent() && zCAICamera::GetCurrent()->camVob)
			{
				zCAICamera *aiCam = zCAICamera::GetCurrent();
				zCVob *aiCamVob = aiCam->camVob;


				OnRemoveVob(aiCamVob);
				aiCamVob->RemoveVobFromWorld();

			}

			if (zCAICamera::GetCurrent() && zCAICamera::GetCurrent()->camVob)
			{
				OnRemoveVob(zCAICamera::GetCurrent()->camVob);
				zCAICamera::GetCurrent()->camVob->RemoveVobFromWorld();
			}
				
			
			while (zCVob* nextCamera = world->SearchVobByName("ZCAICAMERA")) {

				OnRemoveVob(nextCamera);
				nextCamera->RemoveVobFromWorld();
			}

			while (zCVob* nextCamera = world->SearchVobByName("EDITOR_CAMERA_VOB")) {

				OnRemoveVob(nextCamera);
				nextCamera->RemoveVobFromWorld();
			}

			ogame->CamInit();

			if (ogame->GetCamera()->connectedVob)
			{
				print.PrintRed("Restore cam");
				//ogame->GetGameWorld()->AddVob(ogame->GetCamera()->connectedVob);
				ogame->GetCamera()->connectedVob->SetVobName("EDITOR_CAMERA_VOB");
				ogame->GetCamera()->connectedVob->SetAI(0);
				ogame->GetCamera()->connectedVob->dontWriteIntoArchive = true;
				ogame->GetCamera()->connectedVob->SetPositionWorld(safePos);
			}

			

			ShowCursor(TRUE);
			hideWindows = !hideWindows;

			hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
		}


	}
	*/

	/*
	void SpacerApp::PlayTheGame()
	{
		oCWorld*	world = 0;
		zCSession*	session = 0;

		world = dynamic_cast<oCWorld*>(ogame->GetWorld());

		session = ogame;

		if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
		if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

		theApp.SetSelectedVob(NULL);


		//if (!world || (!world->IsCompiled() && !world->IsCompiledEditorMode()) || !session) return;

		// [BENDLIN] Addon Patch2 - 'Play the game'-Flag erst setzen, wenn es auch passiert
		g_bIsPlayingGame = TRUE;

		oCNpc::SetNpcAIDisabled(FALSE);
		zCVob::SetShowHelperVisuals(FALSE);


		ogame->GetWorld()->showWaynet = 0;

		zCVobLight::renderLightVisuals = 0;	// Включает визуалы для источников света
		zCVob::SetShowHelperVisuals(zCVobLight::renderLightVisuals);          // Включает визуалы для тригерров, источников звука и прочего

		ogame->GetWorld()->bspTree.drawVobBBox3D = 0;

		//CSpacerView::view->RememberCamPos();
		dynamic_cast<oCGame*>(session)->GetSpawnManager()->SetSpawningEnabled(TRUE);



		hideWindows = !hideWindows;

		hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();

		while (ShowCursor(FALSE) >= 0);


		npclist = (world ? world->GetVobListNpcs() : 0);
		npcListCopy = zNEW(oTVobListNpcs);

		if (npclist)
		{
			for (int i = 0; i<npclist->GetNumInList(); i++)
			{
				if (npclist->Get(i))
				{
					oCNpc *nextNpc = npclist->Get(i);
					npcListCopy->Insert(nextNpc);
					nextNpc->AddRef();
					nextNpc->DeleteHumanAI();
					nextNpc->InitHumanAI();
				}
			};
		};

		if (zinput->KeyPressed(KEY_LSHIFT))
		{
			g_bInsertNPCs = TRUE;
		}
		else
		{
			g_bInsertNPCs = FALSE;
		}

		ogame->LoadGame(-2, "");

		camPos = ogame->GetCamera()->connectedVob->GetPositionWorld();

		if (ogame->GetCameraAI())
		{
			print.PrintRed("Camera");

			ogame->GetCameraAI()->SetMode(CamModNormal, 0);
		}
		else
		{
			print.PrintRed("Camera create");
			zCAICamera::Create();
		}


	}


	void SpacerApp::StopPlay()
	{

		oCWorld*	world = 0;
		zCSession*	session = 0;


		world = dynamic_cast<oCWorld*>(ogame->GetWorld());

		session = ogame;

		g_bInsertNPCs = FALSE;

		// und wieder alles so wie es war
		oCNpc::SetNpcAIDisabled(TRUE);
		dynamic_cast<oCGame*>(session)->GetSpawnManager()->SetSpawningEnabled(FALSE);

		if (npclist)
		{
			for (int i = 0; i<npclist->GetNumInList(); i++)
			{
				oCNpc *nextNpc = npclist->Get(i);
				if (nextNpc)
				{
					if (npcListCopy->IsInList(nextNpc))
					{
						// war vor "PLAY THE GAME" auch schon drin. nur die stand ai wieder einschalten, dann is gut
						nextNpc->UseStandAI();
						npcListCopy->Remove(nextNpc);
						zRELEASE(nextNpc);
					}
					else
					{
						// wurde in CGameManager::Run() neu eingefьgt. Kann also komplett aus der Welt verschwinden
						world->RemoveVob(nextNpc);
					}
				}
			};
		};

		zCVob::SetShowHelperVisuals(FALSE);

		if (zCAICamera::GetCurrent() && zCAICamera::GetCurrent()->camVob)
		{
			zCAICamera *aiCam = zCAICamera::GetCurrent();
			zCVob *aiCamVob = aiCam->camVob;
			aiCamVob->RemoveVobFromWorld();
		}

		//CSpacerView::view->ResetCameraAfterPlay();
		//CSpacerView::view->ResetCamPos();

		// ARGHH: irgendein zCAICamera Vob is noch in der Welt. Weg damit. Leaked zwar, aber darf auch nicht
		// drin bleiben. Der ganze Camerasetupkram ist eh fьrn Arsch
		//while (zCVob* nextCamera = world->SearchVobByName("ZCAICAMERA")) nextCamera->RemoveVobFromWorld();

		delete npcListCopy; npcListCopy = 0;
		zinput->ClearKeyBuffer();

		g_bIsPlayingGame = FALSE;

		ogame->CamInit();
		//ogame->EnvironmentInit();
		ogame->GetCamera()->connectedVob->SetVobName("EDITOR_CAMERA_VOB");
		ogame->GetCamera()->connectedVob->SetAI(0);
		//ogame->GetCamera()->connectedVob->bbox3D.mins = zVEC3(5, 5, 5);
		//ogame->GetCamera()->connectedVob->bbox3D.maxs = zVEC3(5, 5, 5);
		ogame->GetGameWorld()->AddVob(ogame->GetCamera()->connectedVob);

		ShowCursor(TRUE);
		hideWindows = !hideWindows;

		hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
	}
	*/
}