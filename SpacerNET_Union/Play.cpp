// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	RECT rcOldClip;
	bool winHide = false;
	bool fastMode = false;

	int IsKeyPressed(int a, int b) {
		if (KeyPress(a) || KeyPress(b)) {
			return 1;
		}
		return 0;
	}

	int lightRadiusSafe = 0;


	// game mod inside spacer
	void SpacerApp::GameLoop()
	{

		if (GetForegroundWindow() != hWndApp && GetForegroundWindow() != theApp.mainWin && GetForegroundWindow() != theApp.vobsWin)
		{
			zinput->ClearKeyBuffer();
			zinput->ClearLeftMouse();
		}


		if (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin)
		{

			if (keys.KeyPressed("GAME_MODE", true))
			{

				ToggleGame();
			}

			if (KeyPress(KEY_ESCAPE) || KeyPress(KEY_RETURN))
			{
				
				if (fastMode)
				{
					fastMode = false;
					ogame->aiCam->pathSearch->collisionEnabled = true;
					player->SetCollDet(TRUE);
					player->SetPhysicsEnabled(TRUE);
					player->SetSleeping(FALSE);

					zinput->ClearKeyBuffer();
					return;

				}

				ToggleGame();
				zinput->ClearKeyBuffer();
			}


			if (KeyPress(KEY_F1))
			{
				fastMode = !fastMode;

				if (fastMode)
				{
					ogame->aiCam->pathSearch->collisionEnabled = false;
					player->SetCollDet(FALSE);
					player->SetPhysicsEnabled(FALSE);
					player->SetSleeping(TRUE);
					player->human_ai->PC_Turnings(TRUE);
				}
				else
				{
					//print.PrintRed("FlyMode: F1 key pressed...");
					ogame->aiCam->pathSearch->collisionEnabled = true;
					player->SetCollDet(TRUE);
					player->SetPhysicsEnabled(TRUE);
					player->SetSleeping(FALSE);
				}

				zinput->ClearKeyBuffer();
			}

			if (fastMode)
			{
				static float noclipSpeed = 40;
				static float maxNoclipSpeed = 50;
				zCCamera*& pCamera = zCCamera::activeCam;


				player->human_ai->PC_Turnings(TRUE);

				if (IsKeyPressed(KEY_W, KEY_UP)) {
					zVEC3 pos1 = player->GetPositionWorld();
					float speed = noclipSpeed * 1.0 / 16.6;
					speed = speed * ztimer->frameTimeFloat;
					if (KeyPress(KEY_LSHIFT)) {
						speed *= 5;
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
					zVEC3 vt = pos1 + pCamera->connectedVob->trafoObjToWorld.GetAtVector() * speed;
					player->SetPositionWorld(vt);

					if (noclipSpeed >= maxNoclipSpeed) {
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
				}

				if (IsKeyPressed(KEY_S, KEY_DOWN)) {
					zVEC3 pos1 = player->GetPositionWorld();
					float speed = noclipSpeed * 1.0 / 16.6;
					speed = speed * ztimer->frameTimeFloat;
					if (KeyPress(KEY_LSHIFT)) {
						speed *= 5;
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
					zVEC3 vt = pos1 + pCamera->connectedVob->trafoObjToWorld.GetAtVector() * -speed;
					player->SetPositionWorld(vt);

					if (noclipSpeed >= maxNoclipSpeed) {
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
				}

				if (IsKeyPressed(KEY_A, KEY_LEFT)) {
					zVEC3 pos1 = player->GetPositionWorld();
					float speed = noclipSpeed * 1.0 / 16.6;
					speed = speed * ztimer->frameTimeFloat;
					if (KeyPress(KEY_LSHIFT)) {
						speed *= 5;
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
					zVEC3 vt = pos1 + ogame->GetCamera()->connectedVob->trafoObjToWorld.GetRightVector() * -speed;
					player->SetPositionWorld(vt);

					if (noclipSpeed >= maxNoclipSpeed) {
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
				}

				if (IsKeyPressed(KEY_D, KEY_RIGHT)) {
					zVEC3 pos1 = player->GetPositionWorld();
					float speed = noclipSpeed * 1.0 / 16.6;
					if (KeyPress(KEY_LSHIFT)) {
						speed *= 5;
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
					speed = speed * ztimer->frameTimeFloat;
					zVEC3 vt = pos1 + ogame->GetCamera()->connectedVob->trafoObjToWorld.GetRightVector() * speed;
					player->SetPositionWorld(vt);

					if (noclipSpeed >= maxNoclipSpeed) {
						ogame->GetCameraAI()->ReceiveMsg(zPLAYER_BEAMED);
						ogame->GetCameraAI()->moveTracker->Update();
					}
				}
			}

			

			if (!g_bIsPlayingGame) return;

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

			lightRadiusSafe = playerLightInt;

			GetClipCursor(&rcOldClip);

			auto call = (callIntFunc)GetProcAddress(theApp.module, "ToggleMainMenuInterface");
			call(0);

			auto safePosPlayer = ogame->GetCamera()->connectedVob->GetPositionWorld();

			g_bIsPlayingGame = true;
			RenderDX11_RemoveAmbientLight();

			SetSelectedVob(NULL);

		
			parser->ResetGlobalVars();
			parser->SetInstance("SELF", NULL);
			parser->SetInstance("OTHER", NULL);
			parser->SetInstance("VICTIM", NULL);
			parser->SetInstance("ITEM", NULL);

			ogame->spawnman->ClearList();
			
			delete ogame->infoman;
			ogame->infoman = zNEW(oCInfoManager)(parser);

			oCLogManager::GetLogManager().Clear();

			delete ogame->newsman;
			ogame->newsman = zNEW(oCNewsManager());

			if (zsound) zsound->StopAllSounds();

			oCVisualFX::PreSaveGameProcessing(TRUE);
			oCVisualFX::PostSaveGameProcessing();
	
			oCNpc::SetNpcAIDisabled(FALSE);
			ogame->GetSpawnManager()->SetSpawningEnabled(TRUE);

			oCVob::ClearDebugList();

			if (ogame->portalman) ogame->portalman->CleanUp();

			ogame->ClearObjectRoutineList();
			//ogame->CallScriptStartup();
			//ogame->CallScriptInit();


			if (auto sym = parser->GetSymbol("RX_IsSpacetNet"))
			{
				parser->SetScriptInt("RX_IsSpacetNet", 1);
			}

			

			if (!player)
			{
				oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));
			}

			player->showVisual = 1;
			player->variousFlags = 2;
			player->SetAttribute(NPC_ATR_HITPOINTSMAX, 100000);
			player->CompleteHeal();
			

			ogame->EnterWorld(oCNpc::player, TRUE, "EDITOR_CAMERA_VOB");


			player->dontWriteIntoArchive = true;
			ogame->InitNpcAttitudes();
			player->ai_disabled = false;
			
			player->human_ai->PC_Turnings(1);
			player->human_ai->StopTurnAnis();

			player->SetCollDet(FALSE);
			player->SetPositionWorld(safePosPlayer);
			player->SetCollDet(TRUE);

			player->GetModel()->SetRandAnisEnabled(FALSE);

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
			
		}
		else
		{

			if (fastMode)
			{
				fastMode = false;

				ogame->aiCam->pathSearch->collisionEnabled = true;
				player->SetCollDet(TRUE);
				player->SetPhysicsEnabled(TRUE);
				player->SetSleeping(FALSE);
			}

			zVEC3 camFuturePos = player->GetPositionWorld();

#if ENGINE >= Engine_G2
			ogame->m_bWorldEntered = false;
#endif

			parser->ResetGlobalVars();
			parser->SetInstance("SELF", NULL);
			parser->SetInstance("OTHER", NULL);
			parser->SetInstance("VICTIM", NULL);
			parser->SetInstance("ITEM", NULL);

			ogame->spawnman->ClearList();

			delete ogame->infoman;
			ogame->infoman = zNEW(oCInfoManager)(parser);
	
			oCLogManager::GetLogManager().Clear();

			delete ogame->newsman;
			ogame->newsman = zNEW(oCNewsManager());
	
			zsound->StopAllSounds();
	
			oCVisualFX::PreSaveGameProcessing(TRUE);
			oCVisualFX::PostSaveGameProcessing();
			

			oCVob::ClearDebugList();

			if (ogame->portalman) ogame->portalman->CleanUp();

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


			if (theApp.options.GetIntVal("bAddPlayerForPlugins"))
			{
				oCNpc::player = (oCNpc*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_NSC, parser->GetIndex("PC_HERO"));

				oCNpc::player->GetModel();
				player->dontWriteIntoArchive = true;
				player->showVisual = 0;
			}
			
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

			if (lightRadiusSafe > 0)
			{
				playerLightInt = lightRadiusSafe;

#if ENGINE >= Engine_G2
				if (zCSkyControler::GetActiveSkyControler())
				{
					zCSkyControler::GetActiveSkyControler()->SetLightDirty();
				}
#endif
				playerLightInt = lightRadiusSafe;
			}

			ShowCursor(TRUE);

			if (winHide && hideWindows)
			{
				hideWindows = !hideWindows;

				hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();


			}
			
			ClipCursor(&rcOldClip);

			auto call = (callIntFunc)GetProcAddress(theApp.module, "ToggleMainMenuInterface");
			call(1);

			
			
		}	


		
	}
}