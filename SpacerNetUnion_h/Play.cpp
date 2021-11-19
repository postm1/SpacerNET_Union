// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	typedef zCListSort<zCVob>		oTVobList;
	typedef zCListSort<oCNpc>		oTVobListNpcs;
	typedef zCListSort<oCItem>		oTVobListItems;

	// Add your code here . . .
	oTVobListNpcs* npclist = NULL;
	oTVobListNpcs* npcListCopy = NULL;

	bool heroSpawned = false;
	zVEC3 camPos;
	zSTRING CamModNormal("CAMMODNORMAL");


	void SpacerApp::GameLoop()
	{

		if (KeyPress(KEY_ESCAPE))
		{
			ToggleGame();
			zinput->ClearKeyBuffer();
		}

	}

	void SpacerApp::ToggleGame()
	{
		return;

		static zCCamera* cam = NULL;

		if (!g_bIsPlayingGame)
		{
			print.PrintRed("Start the game...");

			g_bIsPlayingGame = true;

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


			/*
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
			*/

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