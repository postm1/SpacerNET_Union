

namespace GOTHIC_ENGINE {


	
	// Add your code here . . .
	SpacerApp::SpacerApp()
	{
		
		this->pickedVob = NULL;
		this->pickedVobBBoxMode = false;
		this->dynLightCompile = false;
		this->levelReady = false;
		this->dataFilled = false;
		this->pickedWaypoint = NULL;
		this->pickedWaypoint2nd = NULL;
		this->current_object = NULL;
		this->selectedWaypointForNet = NULL;
		this->mouseClicksBlockedTime = 0;
		this->isMerged = false;
		this->isMesh = false;
		this->hideWindows = false;
		this->hideWindowsForce = false;
		this->treeToCopy = NULL;
		this->moverVob = NULL;
		this->m_kf_pos = 0;
		this->event_sourcevob = NULL;
		this->isExit = false;
		this->selectedWpForCreateIsBlocked = false;
		this->g_bIsPlayingGame = false;
		this->g_bInsertNPCs = false;
		this->nextInsertBlocked = false;
		this->currentVobRender = NULL;
		this->selectNextVob = false;
		this->turnWpMode = TurnWpMode::NONE;
		this->isVobParentChange = false;
		this->firstZenLoad = false;
		this->currenItemRender = NULL;
		this->search.cur_vob = NULL;
		this->search.cur_vob_convert = NULL;
		this->vobListSelectedIndex = 0;
		this->firstTimeZenSaved = false;
		this->pluginsChecked = false;
		this->isGrattControlActive = false;
		this->isNextCopyVobInsertNear = false;
		this->showRespawnOnVobs = false;
		this->nextInsertionIsTempPfx = false;
		this->bDebugSpacerLoadMesh = false;
		this->globalParent = NULL;

		this->s_pLightSphereMesh = NULL;
		this->vobLightSelected = NULL;

		this->spcOpt.Init("spacer_net.ini", true);
		
		this->restorator.Init();
	}

	zCVob* SpacerApp::GetSelectedVob()
	{
		return this->pickedVob;
	}

	void SpacerApp::SetSelectedVob(zCVob* vob, zSTRING funcName)
	{
		pickedVob = vob;


		if (viewInfo) viewInfo->ClrPrintwin();

		if (isGrattControlActive)
		{
			if (vob)
			{

				ControllerEvents.SelectedVobs.InsertEnd(vob);
				ControllerEvents.MotionMode = zMM_MOVE;
				ControllerEvents.PickMode = zPM_VOBS;

			}
			else
			{
				ControllerEvents.SelectedVobs.Clear();
				ControllerEvents.PickedVob = NULL;
			}
		}
		
	

		if (!selectedWpForCreateIsBlocked)
		{
			selectedWaypointForNet = dynamic_cast<zCVobWaypoint*>(vob);
		}

		auto light = dynamic_cast<zCVobLight*>(pickedVob);

		if (!pickedVob || light)
		{
			
				if (!s_pLightSphereMesh)
				{
					s_pLightSphereMesh = zCMesh::Load("SPHERE.3DS", TRUE);
				}

				if (theApp.options.GetIntVal("showLightRadiusVob"))
				{
					vobLightSelected = light;
				}
			
		}

		if (!light || !pickedVob)
		{
			vobLightSelected = NULL;
		}

		moverVob = dynamic_cast<zCMover*>(vob);

		SetMover();

	}



	bool SpacerApp::GameFocused()
	{
		if (GetForegroundWindow() == hWndApp || GetForegroundWindow() == theApp.mainWin)
		{
			return true;
		}

		return false;
	}

	void SpacerApp::ToggleWindowsVisible()
	{
		hideWindows = !hideWindows;
		hideWindowsForce = hideWindows;
		hideWindows ? (voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")() : (voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
	}

	void SpacerApp::SetHoldTime(int enabled)
	{
		int& holdTime = *(int*)0xAB0888;
		holdTime = enabled;
	}

	zCWorld* SpacerApp::GetWorld(zBOOL getEmptyWorldToo)
	{
		if (!ogame)				return 0;

		zCWorld* world = (zCWorld*)(ogame->GetWorld());

		if (getEmptyWorldToo || world->globalVobTree.GetFirstChild() != 0) return world;

		return 0;
	}

	bool SpacerApp::IsAWorldLoaded()
	{
		zCWorld* world = GetWorld(TRUE);
		return (world && world->globalVobTree.GetFirstChild() != 0);
	}

	void SpacerApp::Reset()
	{
		
		if (g_bIsPlayingGame)
		{
			theApp.ToggleGame();
			
		}

		pickedVob = NULL;
		vobToCopy = NULL;
		globalParent = NULL;
		pickedWaypoint2nd = NULL;

		pickedWaypoint = NULL;
		selectedWaypointForNet = NULL;
		currentVobRender = NULL;
		currenItemRender = NULL;

		current_object = NULL;
		treeToCopy = NULL;

		AddVobToRender("", false);

		if (IsAWorldLoaded())
		{
			pfxManager.RemovePFX();
			if (ogame->GetWorld()) ogame->GetWorld()->DisposeWorld();
		}


		if (viewInfo)
		{
			viewInfo->ClrPrintwin();
		}

		if (zmusic) zmusic->Stop();
		if (zsound) zsound->StopAllSounds();
		zinput->ClearKeyBuffer();

		isMerged = false;
		isMesh = false;
		levelReady = false;
		treeIsReady = false;
		firstTimeZenSaved = false;
		theApp.ClearRespList();
		globalParent = NULL;
		itemsLocator.Reset();
		restorator.Reset();

		if (ogame->GetWorld() && ogame->GetWorld()->GetBspTree())
		{
			//ogame->GetWorld()->GetBspTree()->m_bRenderedFirstTime = TRUE;
			//MessageBox(0, "First", 0, 0);
		}
	}


	void SpacerApp::Loop()
	{
		isGrattControlActive = theApp.options.GetIntVal("bToggleNewController");
		
		if (ogame && ogame->GetWorld() && ogame->GetCamera() && !isExit && !g_bIsPlayingGame)
		{
			if (GameFocused())
			{
				if (isGrattControlActive)
				{
					ControllerEvents.CameraMoving = zCM_ENABLED;
					UpdateGrattController();

					ManagerGrattController();
				}
				else
				{
					if (zinput->GetMouseButtonPressedLeft())
					{
						auto pickMode = theApp.GetPickMode();

						if (pickMode == SWM_VOBS)
						{
							theApp.PickVob();
							zinput->ClearKey(MOUSE_LEFT);
							ClearLMB();
						}
						else if (pickMode == SWM_MATERIALS)
						{
							theApp.PickMaterial();
							zinput->ClearKey(MOUSE_LEFT);
							ClearLMB();
						}
					}

					ControllerEvents.CameraMoving = zCM_DISABLED;
					ControllerEvents.SelectedVobs.Clear();
					ControllerEvents.PickedVob = NULL;

					CameraMoving();
					VobMoving();
				}

			}

			VobKeys();
			

		}
	}

	inline bool CheckDx11() {

		HMODULE ddraw = GetModuleHandle("ddraw.dll");
		HMODULE g2a = GetModuleHandle("g2a.dll");
		HMODULE g2a_avx2 = GetModuleHandle("g2a_avx2.dll");


		bool result =

			(ddraw != Null && GetProcAddress(ddraw, "GDX_AddPointLocator"))
			|| (g2a != Null && GetProcAddress(g2a, "GDX_AddPointLocator"))
			|| (g2a_avx2 != Null && GetProcAddress(g2a_avx2, "GDX_AddPointLocator"))

			;

		return result;

	}

	bool SpacerApp::IsDx11Active()
	{
		static bool isEnabled = CheckDx11();
		return isEnabled;
	}

	void SpacerApp::RenderDx11_Bbox(zCVob* vob)
	{
		static zCOLOR colUp = zCOLOR(229, 223, 45);
		static zCOLOR colUpFP = zCOLOR(0, 57, 229);
		static zCOLOR colAt = zCOLOR(255, 0, 0);
		static zCOLOR colRight = zCOLOR(0, 255, 0);
		zREAL size = 0.0f;

		if (!vob) return;

		zCOLOR color = GFX_RED;

		zPOINT3 wsPoint1, csPoint1;
		zPOINT2 ssPoint1;

		viewInfo->ClrPrintwin();

		screen->RemoveItem(viewInfo);

		if (dynamic_cast<zCVobWaypoint*>(vob) || dynamic_cast<zCWaypoint*>(vob))
		{
			/*

			zTBBox3D BBox;
			BBox.mins = zVEC3(-100, -100, -100);
			BBox.maxs = zVEC3(+100, +100, +100);
			BBox.Translate(vob->GetPositionWorld());
			vob->SetBBox3DWorld(BBox);
			*/
			size = 50;
			zVEC3 pos = vob->GetPositionWorld() + zVEC3(0, 0, 0);

			zlineCache->Line3D(pos, pos + vob->GetUpVectorWorld()	* size, colUp, 1);
			zlineCache->Line3D(pos, pos + vob->GetAtVectorWorld()	* size, colAt, 1);
			zlineCache->Line3D(pos, pos + vob->GetRightVectorWorld()* size, colRight, 1);
		}
		else if (auto spot = dynamic_cast<zCVobSpot*>(vob))
		{
			/*
			const zREAL FPBOX_DIMENSION = 50;
			zTBBox3D fpBox;

			fpBox.maxs = fpBox.mins = spot->GetPositionWorld();
			fpBox.maxs[0] += FPBOX_DIMENSION; fpBox.maxs[1] += FPBOX_DIMENSION * 2; fpBox.maxs[2] += FPBOX_DIMENSION;
			fpBox.mins[0] -= FPBOX_DIMENSION; fpBox.mins[1] -= FPBOX_DIMENSION * 2; fpBox.mins[2] -= FPBOX_DIMENSION;
			fpBox.Translate(spot->GetPositionWorld());
			spot->SetBBox3DWorld(fpBox);
			*/
			/*
			zTBBox3D BBox;
			BBox.mins = zVEC3(-100, -100, -100);
			BBox.maxs = zVEC3(+100, +100, +100);
			BBox.Translate(vob->GetPositionWorld());
			vob->SetBBox3DWorld(BBox);
			*/

			size = 70;

			zVEC3 pos = vob->GetPositionWorld() + vob->GetAtVectorWorld() * 10 + vob->GetUpVectorWorld() * 15;

			zlineCache->Line3D(pos, pos + vob->GetUpVectorWorld()	* size, colUp, 1);
			zlineCache->Line3D(pos, pos + vob->GetAtVectorWorld()	* size, colAt, 1);
			zlineCache->Line3D(pos, pos + vob->GetRightVectorWorld()* size, colRight, 1);


			zlineCache->Line3D(pos + zVEC3(1, 0, 0), pos + vob->GetUpVectorWorld()	* size, colUpFP, 1);
			zlineCache->Line3D(pos + zVEC3(0, 1, 0), pos + vob->GetAtVectorWorld()	* size, colAt, 1);
			zlineCache->Line3D(pos + zVEC3(0, 1, 0), pos + vob->GetRightVectorWorld()* size, colRight, 1);



			zVEC3 textPos = spot->GetPositionWorld() + vob->GetAtVectorWorld() * size + zVEC3(0, 10, 0);
			zCCamera* cam = ogame->GetCamera();
			zVEC3 viewPos = cam->GetTransform(zTCamTrafoType::zCAM_TRAFO_VIEW) * textPos;
			int x, y;
			cam->Project(&viewPos, x, y);

			if (viewPos[2] > cam->nearClipZ) {
				int px = viewInfo->anx(x);
				int py = viewInfo->any(y);

				viewInfo->SetFontColor(zCOLOR(229, 135, 27));
				viewInfo->Print(px, py, spot->GetVobName());
				viewInfo->Blit();
				screen->InsertItem(viewInfo);
			}
		}

		vob->bbox3D.Draw(color);
	}

	void SpacerApp::RenderSelectedVobBbox()
	{
		if (!IsDx11Active())
		{
			return;
		}

		
		auto vob = GetSelectedVob();

		RenderDx11_Bbox(vob);

		

		return;
		

		
		/*
		if (this->pickedVob)
		{

			auto vob = this->pickedVob;

			auto spot = dynamic_cast<zCVobSpot*>(vob);

			auto wp = dynamic_cast<zCVobWaypoint*>(vob);

			auto zone = dynamic_cast<zCZone*>(vob);

			if (spot)
			{
				size = 50;

				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetUpVectorWorld()	* size, colUp, 1);
				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetAtVectorWorld()	* size, colAt, 1);
				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetRightVectorWorld()* size, colRight, 1);
			}
			else if (wp)
			{
				size = 50;

				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetUpVectorWorld()	* size, colUp, 1);
				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetAtVectorWorld()	* size, colAt, 1);
				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetRightVectorWorld()* size, colRight, 1);
			}
			else if (zone)
			{
				vob->bbox3D.Draw(zCOLOR(255, 0, 0));
			}
			else
			{
				static	zREAL wave;

				zTBBox3D bbox = vob->bbox3D;


				if (vob->GetVisual())
				{
					bbox = vob->GetVisual()->GetBBox3D();
				}

				//if (!vob->sleepingMode)	wave = sin(ztimer->totalTimeFloat*0.005F);
				//else						wave = 1.0F;

				zREAL	size = (float)((int)(bbox.maxs.n[VX] - bbox.mins.n[VX]));

				if (size >= 150)
				{
					size *= 0.8;
				}
				else  if (size >= 200)
				{
					size *= 0.75;
				}
				else  if (size >= 400)
				{
					size *= 0.6;
				}

				if (size < 35) size = 35;

				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetUpVectorWorld()	* size, colUp, 1);
				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetAtVectorWorld()	* size, colAt, 1);

				zlineCache->Line3D(vob->GetPositionWorld(), vob->GetPositionWorld() + vob->GetRightVectorWorld()* size, colRight, 1); // зеленый


				//vob->bbox3D.Draw(zCOLOR(255, 0, 0));
			}


		}
		*/
	}

	void SpacerApp::UpdateGrattController()
	{
		theApp.TryPickMouse();

		float ax = screen->anx(pickTryEntry.ax);
		float ay = screen->any(pickTryEntry.ay);

		//print.PrintRed(Z ax + " " + Z ay);

		ControllerEvents.CursorPosition = zVEC2(ax, ay);
		ControllerEvents.PickedVob = pickedVob;


	}

	SpacerWorkMode SpacerApp::GetPickMode()
	{
		return (SpacerWorkMode)theApp.options.GetIntVal("bToggleWorkMode");
	}

	void SpacerApp::ManagerGrattController()
	{
		static uint lastClickTime = Invalid;
		static bool leftMouse = false;

		bool doubleClick = false;

		if (zinput->GetMouseButtonToggledLeft()) {
			uint clickTime = Timer::GetTime();

			if (clickTime - lastClickTime < 250)
				doubleClick = true;

			lastClickTime = clickTime;
		}

		bool leftMousePressed = zinput->GetMouseButtonPressedLeft();


		if (!pickedVob)
		{
			if (leftMousePressed)
			{
				auto pickMode = theApp.GetPickMode();

				if (pickMode == SWM_VOBS)
				{
					theApp.PickVob();
				}
				else if (pickMode == SWM_MATERIALS)
				{
					theApp.PickMaterial();
				}

				zinput->ClearKey(MOUSE_LEFT);
				ClearLMB();

			}
		}
		else
		{
			if (leftMousePressed)
			{
				if (!leftMouse && zinput->KeyPressed(KEY_LSHIFT))
				{
					if (!dynamic_cast<zCVobLevelCompo*>(theApp.pickedVob))
					{
						theApp.vobToCopy = theApp.pickedVob;
						theApp.isVobParentChange = false;
						
			
						SetSelectedVob(NULL);

						isNextCopyVobInsertNear = true;
						HandleInsertVobCopy(NULL);
						isNextCopyVobInsertNear = false;
						leftMouse = true;
					}

				}
			}
			else
			{
				leftMouse = false;
			}

			if (doubleClick)
			{
				doubleClick = false;

				if (theApp.pickedVob) theApp.pickedVob->drawBBox3D = false;

				SetSelectedVob(NULL);
				zinput->ClearKey(MOUSE_LEFT);
				ClearLMB();

			}
		}
	}

	void SpacerApp::PreRender()
	{
		if (g_bIsPlayingGame)
		{
			return;
		}


		if (pickedWaypoint2nd)
		{
			//print.PrintRed("pickedWaypoint2nd");
			static bool		blinkDrawIt = true;
			static zDWORD	lastBlinkToggle = 0;
			zREAL	timeValue = ztimer->totalTimeFloat;
			zDWORD	now = int(timeValue / 250);
			if (now != lastBlinkToggle)
			{
				blinkDrawIt = !blinkDrawIt;
				lastBlinkToggle = now;
			}
			pickedWaypoint2nd->SetDrawBBox3D(blinkDrawIt);
		}


		pickedVobBBoxMode = pickedVob && pickedVob->drawBBox3D;

		if (pickedVob)
		{
			if (!pickedVob->GetEM(0)->GetCutsceneMode())
			{
				pickedVob->SetDrawBBox3D(TRUE);
			}
		}

		RenderSelectedVobBbox();

	}

	void SpacerApp::AfterRender()
	{
		if (g_bIsPlayingGame)
		{
			return;
		}

		if (pickedVob) pickedVob->SetDrawBBox3D(pickedVobBBoxMode);
		if (pickedWaypoint2nd) pickedWaypoint2nd->SetDrawBBox3D(FALSE);


		zCVob*			refVob = 0;
		zCList<zCVob>*	vobList = 0;
		// Targets
		vobList = &targetVobList;
		refVob = pickedVob;
		if (vobList && refVob)
		{
			for (int i = 0; i<vobList->GetNumInList(); i++) DrawLine(ogame->GetCamera(), refVob, vobList->Get(i), GFX_BLUE);
		}
		// Sources
		vobList = &sourceVobList;
		refVob = pickedVob;
		if (vobList && refVob)
		{
			for (int i = 0; i<vobList->GetNumInList(); i++) DrawLine(ogame->GetCamera(), refVob, vobList->Get(i), GFX_RED);
		}

	}










	/*

	if (player && ogame && ogame->GetWorld() && zlineCache)
	{
	// трассировка мира сильно вниз от центра ГГ
	BOOL result = ogame->GetWorld()->TraceRayNearestHit(player->GetPositionWorld(), zVEC3(0, -10000, 0), (zCVob*)player, zTRACERAY_VOB_BBOX);

	// зелёная(свободная) линия трассировки
	zCOLOR col = zCOLOR(0, 255, 0);

	// если трассировка прошла успешно и найден воб
	if (result && ogame->GetWorld()->traceRayReport.foundVob)
	{
	// линия трассировки красная(занята)
	col = zCOLOR(255, 0, 0);

	// включаем боунд бокс найденного воба
	ogame->GetWorld()->traceRayReport.foundVob->SetDrawBBox3D(TRUE);

	// если у воба есть визуал
	if (screen && ogame->GetWorld()->traceRayReport.foundVob->visual)
	{
	// выводим на экран название
	screen->Print(10, 2000, ogame->GetWorld()->traceRayReport.foundVob->visual->GetVisualName());
	}
	}

	// тестовая линия, показывающая успешность трассировки
	zlineCache->Line3D(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(0, -10000, 0), col, 0);
	}


	*/



	void SpacerApp::PickMaterial()
	{
		if (!theApp.TryPickMouse())
		{
			return;
		}

		mm.OnPick(pickTryEntry.ax, pickTryEntry.ay);
	}

	zVEC2 SpacerApp::GetMousePosVirt()
	{
		POINT  cur;
		_GetCursorPos(&cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		float rw = rect.right - rect.left;
		float rh = rect.bottom - rect.top;


		float ax = (float)cur.x / rw * (float)zrenderer->vid_xdim;
		float ay = (float)cur.y / rh * (float)zrenderer->vid_ydim;


		ax = screen->anx(ax);
		ay = screen->any(ay);

		zVEC2 result;


		result.n[0] = ax;
		result.n[1] = ay;

		//print.PrintRed(Z ax + " " + Z ay);

		return result;
	}

	// попадает ли мышь по экрану, или же задевает меню
	bool SpacerApp::TryPickMouse()
	{
		POINT  cur;
		_GetCursorPos(&cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		float rw = rect.right - rect.left;
		float rh = rect.bottom - rect.top;


		float ax = (float)cur.x / rw * (float)zrenderer->vid_xdim;
		float ay = (float)cur.y / rh * (float)zrenderer->vid_ydim;

		pickTryEntry.allowed = true;
		pickTryEntry.ax = ax;
		pickTryEntry.ay = ay;

		if (pickTryEntry.ay <= 0) pickTryEntry.allowed = false;

		//print.PrintRed(Z ax + " " + Z ay + " " + Z pickTryEntry.allowed);

		return pickTryEntry.allowed;
	}
	
	struct PickVobStruct
	{
		zCVob* vob;
		zVEC3 interPos;
	};
	zBOOL SpacerApp::PickSceneNew(zCCamera& cam, int xscr, int yscr, zREAL rayLength)
	{
		zCWorld* wld = ogame->GetWorld();

		if (!wld) return FALSE;

		const zVALUE RAY_DIST = 600000.0F;

		cam.Activate();	

		zPOINT3 ray00, ray, p;
		// create ray00, ray by backprojection
		// ray00, ray sind im world(obj)Space
		// 'ray00	= cam.camMatrixInv * zPOINT3(0,0,0);'  =
		cam.camMatrixInv.GetTranslation(ray00);
		p.n[VZ] = RAY_DIST;
		cam.BackProject(xscr, yscr, p);				// p im camSpace
		p = cam.camMatrixInv * p;					// p im world(obj)Space  
		ray = p - ray00;

		// wuenscht der Benutzer eine bestimmte Ray-Laenge?
		if (rayLength>0)
		{
			ray.Normalize();
			ray *= rayLength;
		};

		wld->traceRayReport.Clear();
		wld->traceRayIgnoreVobFlag = TRUE;

		int	traceFlags = zTRACERAY_STAT_POLY |
			zTRACERAY_POLY_TEST_WATER;

		if (zCVob::GetShowHelperVisuals())
			traceFlags |= zTRACERAY_VOB_TEST_HELPER_VISUALS;

		zBOOL hit = wld->TraceRayNearestHit(ray00, ray, (zCVob*)0, traceFlags);
		wld->traceRayIgnoreVobFlag = FALSE;




		zCArray <PickVobStruct*> searchList;

		int bBoxSize = 3000;
		int radius_sp = 700;
		zCArray<zCVob*> baseVobList;
		zTBBox3D box;
		zVEC3 posToPlace = ogame->GetCameraVob()->GetPositionWorld();
		auto camVob = ogame->GetCamera()->GetVob();

		box.maxs = posToPlace + zVEC3(bBoxSize, bBoxSize, bBoxSize);
		box.mins = posToPlace - zVEC3(bBoxSize, bBoxSize, bBoxSize);
		ogame->GetWorld()->CollectVobsInBBox3D(baseVobList, box);

		int showHelpVobs = theApp.options.GetIntVal("showHelpVobs");


		for (int i = 0; i < baseVobList.GetNumInList(); i++) {

			zCVob* vob = baseVobList[i];

			if (vob &&
				(		vob != ogame->GetCameraVob()
					&& !dynamic_cast<zCVobLight*>(vob)
					&& ! dynamic_cast<zCVobLevelCompo*>(vob)
					&& ! dynamic_cast<zCZone*>(vob)
					&& ! (vob->GetVisual() && (vob->GetVisual()->GetVisualName().Contains(".pfx") || vob->GetVisual()->GetVisualName().Contains(".PFX")))
					&& vob != theApp.currentVobRender
					&& vob != pfxManager.testVob
				)
			)
			{

				if (!showHelpVobs && (dynamic_cast<zCVobSpot*>(vob) || dynamic_cast<zCVobWaypoint*>(vob)))
				{
					continue;
				}

				zVEC3 sphInt = IsSphIntersect(camVob->GetPositionWorld(), vob->GetPositionWorld(), ray, radius_sp);

				//if (sphInt != NULL)
				{
					auto entry = new PickVobStruct();

					entry->vob = vob;
					entry->interPos = sphInt;

					searchList.InsertEnd(entry);
				}

				
			}
		}



		
		if (hit)
		{
			cmd << "Inter point found" << endl;
		}
		else
		{
			cmd << "Inter point NOT FOUND" << endl;
		}

		int i_num = 0;
		int dist = 10e9;

		if (hit)
		{
			for (int i = 0; i < searchList.GetNumInList(); i++)
			{
				auto entry = searchList[i];
				auto vob = entry->vob;

				/*
				if (vob->GetVisual())
				{
				cmd << vob->GetVobName() << " Visual: " << vob->GetVisual()->GetVisualName() << endl;
				}
				else
				{
				cmd << vob->GetVobName() << " No visual:" << endl;
				}
				*/

				int cur_dist = vob->GetPositionWorld().Distance(wld->traceRayReport.foundIntersection);

				if (cur_dist < dist)
				{
					dist = cur_dist;
					i_num = i;
				}


			}
		}

		cmd << "Distance: " << dist << endl;

		wld->traceRayReport.Clear();

		if (searchList.GetNumInList() > 0)
		{
			wld->traceRayReport.foundVob = searchList[i_num]->vob;
		}

		
		searchList.DeleteListDatas();

		return true;
	}
	void SpacerApp::PickVob()
	{
		if (!theApp.TryPickMouse())
		{
			return;
		}


		ogame->GetWorld()->PickScene(*ogame->GetCamera(), pickTryEntry.ax, pickTryEntry.ay, -1);

		zCVob* foundVob = ogame->GetWorld()->traceRayReport.foundVob;



		if ((theApp.pickedVob == foundVob && theApp.pickedVob != NULL))
		{
			return;
		}


		oCVisualFX* pVisualVob = dynamic_cast<oCVisualFX*>(foundVob);
		

		if (pVisualVob)
		{
			
			oCItem* pItem = dynamic_cast<oCItem*>(pVisualVob->origin);

			if (pItem)
			{
				foundVob = pVisualVob->origin;
			}

		}

		if (foundVob && (foundVob == pfxManager.testVob || foundVob == currentVobRender || foundVob == currenItemRender))
		{
			foundVob = NULL;
		}

		// если расширенный режим выбор и мы выделили pfx, то убираем такой воб
		if (foundVob && zinput->KeyPressed(KEY_LCONTROL))
		{
			zCVisual* visual = foundVob->GetVisual();

			if (visual && (visual->GetVisualName().Search(".PFX", 1) == -1 || visual->GetVisualName().Search(".pfx", 1) == -1))
			{
				foundVob = NULL;
			}
		}
		
		if (!foundVob && isGrattControlActive)
		{
			SetSelectedVob(NULL);
		}


		if (!foundVob && zinput->KeyPressed(KEY_LCONTROL))
		{
			zTBBox3D box;

			zCVob* camVob = ogame->GetCamera()->connectedVob;
			zCCamera* cam = ogame->GetCamera();
			zVEC3 currentPos = camVob->GetPositionWorld();
			zCArray<zCVob*> baseVobList;
			zCArray<zCVob*> resVobList;
			zCArray<zCVob*> resultVobList;
			int bBoxSize = 2000;
			int radius_sp = 220;
			zCVob* tryVob = NULL;
			int dist = 10e9;
			int stepDist = 25;
			bool flagExit = false;
			int i_num = 0;

			box.maxs = camVob->GetPositionWorld() + zVEC3(bBoxSize, bBoxSize, bBoxSize);
			box.mins = camVob->GetPositionWorld() - zVEC3(bBoxSize, bBoxSize, bBoxSize);
			ogame->GetWorld()->CollectVobsInBBox3D(baseVobList, box);



			zVEC3 ray00, ray, p;
			cam->camMatrixInv.GetTranslation(ray00);
			p.n[VZ] = 1;
			cam->BackProject(pickTryEntry.ax, pickTryEntry.ay, p);				// p im camSpace
			p = cam->camMatrixInv * p;					// p im world(obj)Space  
			ray = p - ray00;

			ray = ray.Normalize();
			// Собираем список допустимых вобов
			for (int i = 0; i < baseVobList.GetNumInList(); i++) {


				zCVob* vob = baseVobList[i];

				if (vob
					&& vob != camVob
					&& !dynamic_cast<zCVobWaypoint*>(vob)
					&& !dynamic_cast<zCVobSpot*>(vob)
					&& !dynamic_cast<zCVobLight*>(vob)
					&& !dynamic_cast<zCVobLevelCompo*>(vob)
					&& !dynamic_cast<zCZone*>(vob)
					&& vob->GetVisual()
					&& vob->GetVisual()->GetVisualName().Search(".PFX", 1) == -1
					&& vob->GetVisual()->GetVisualName().Search(".pfx", 1) == -1
					&& vob != currentVobRender
					&& vob != pfxManager.testVob
					)
				{
					zVEC3 sphInt = IsSphIntersect(camVob->GetPositionWorld(), vob->GetPositionWorld(), ray, radius_sp);

					if (sphInt != NULL)
					{
						resVobList.Insert(vob);
					}
					
				}
			}

			//std::cout << "Intersect count: " << resVobList.GetNumInList() << std::endl;

			for (int i = 0; i < resVobList.GetNumInList(); i++) 
			{
				int cur_dist = Dist(camVob, resVobList[i]);

				if (cur_dist < dist)
				{
					dist = cur_dist;
					i_num = i;
				}

				
			}

			if (resVobList.GetNumInList() > 0)
			{
				foundVob = resVobList[i_num];
			}
		}



		if (zinput->KeyPressed(KEY_LSHIFT))
		{
			//print.PrintRed("Selected second waypoint");
			pickedWaypoint2nd = dynamic_cast<zCVobWaypoint*>(foundVob);
		}
		else
		{

			if ((theApp.pickedVob != foundVob && theApp.pickedVob != NULL))
			{
				theApp.pickedVob->drawBBox3D = false;
			}

			theApp.SetSelectedVob(foundVob, "PickSingle");
			pickedWaypoint = dynamic_cast<zCVobWaypoint*>(theApp.pickedVob);

			if (theApp.pickedVob)
			{


				std::cout << "Union: OnSelectVob " << GetVobName(theApp.pickedVob) << " Parent: " << theApp.pickedVob->GetParent() << std::endl;

				auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
				onSelect((int)theApp.pickedVob);
			}
			SelectObject(theApp.pickedVob);
		}


	



	}




	void SpacerApp::SetProperties(zSTRING& propString, zSTRING classNameVob)
	{
		char* str = propString.ToChar();
		char* className = classNameVob.ToChar();


		OutFile("SetProperties: " + A classNameVob, false);

		auto AddProps = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

		Stack_PushString(className);
		Stack_PushString(str);

		AddProps();


	}

	void SpacerApp::SelectObject(zCObject* object)
	{
		OutFile("SelectObject: object " + AHEX32((uint)object), true);

		theApp.CollectTargetListTrigger();
		

		if (object)
		{
			if (current_object)
			{
				zRELEASE(current_object);
			}


			current_object = object;
			if (current_object) current_object->AddRef();


			OutFile("SelectObject: current_object " + AHEX32((uint)current_object), true);

			zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, FALSE, 0);

			arch->SetStringEOL(zSTRING("\n"));
			arch->WriteObject(object);
			zSTRING arcString;
			arch->GetBufferString(arcString);

			zCVob* vob = (zCVob*)(object);
			zSTRING className = vob->_GetClassDef()->className;

			//std::cout << "Get Props: " << arcString << std::endl;

			SetProperties(arcString, className);



			arch->Close();
			zRELEASE(arch);

			/*
			vob->BeginMovement();
			vob->SetPositionWorld(vob->GetPositionWorld());
			vob->EndMovement(1);
			*/
			
		}
		else
		{
			auto AddProps = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

			Stack_PushString("");
			Stack_PushString("");

			AddProps();
		}

		zinput->ClearLeftMouse();
		zinput->ClearKeyBuffer();
	}


	void SpacerApp::ApplyProps(CString str, CString nameCurrent, CString visualStr)
	{
		zSTRING propString = zSTRING(str);
		zSTRING name = zSTRING(nameCurrent);
		zSTRING visual = zSTRING(visualStr);


		
		//cmd << "Player: " << AHEX32((uint)player) << endl;

		if (player)
		{
			//cmd << "Player: world " << AHEX32((uint)player->homeWorld) << endl;
		}
		//Message::Box("Apply 1");
		//OutFile("ApplyProps: " + A name, true);
		
		auto pickMode = theApp.GetPickMode();

		if (pickMode == SWM_MATERIALS && mm.IsMaterialSelected())
		{
			
			auto selPoly = mm.GetCurrentSelection();

			zCMaterial* currentMat = selPoly->GetMaterial();

			if (selPoly && currentMat)
			{
				cmd << "ApplyProps for material " << AHEX32((uint)currentMat) << endl;

				zCBuffer buf(propString.ToChar(), propString.Length());
				zCArchiver* arch = zarcFactory->CreateArchiverRead(&buf, 0);
				arch->SetStringEOL(zSTRING("\n"));



				arch->ReadObject(currentMat);
				arch->Close();
				zRELEASE(arch);
			}
		}
		else if (current_object)
		{
			cmd << "ApplyProps for vob " << AHEX32((uint)current_object) << endl;
			zCVob* oldVob = dynamic_cast<zCVob*>(current_object);

			zSTRING lastName = current_object->GetObjectName();
			zSTRING lastVisual = oldVob && oldVob->GetVisual() ? oldVob->GetVisual()->GetVisualName() : "";


			zCBuffer buf(propString.ToChar(), propString.Length());
			zCArchiver* arch = zarcFactory->CreateArchiverRead(&buf, 0);
			arch->SetStringEOL(zSTRING("\n"));



			arch->ReadObject(current_object);
			arch->Close();
			zRELEASE(arch);

			//cmd << "ApplyProps for vob 2 " << AHEX32((uint)current_object) << endl;

			//Message::Box("Apply 2");
			zCVob* vob = dynamic_cast<zCVob*>(current_object);

			if (vob)
			{
				if (vob->GetVisual())
				{

					if (GetFileExtension(visual) != GetFileExtension(lastVisual))
					{
						//print.PrintRed(GetFileExtension(visual) + "/" + GetFileExtension(vob->GetVisual()->GetVisualName()));
						//print.PrintRed("Clean wind: " + A visual + "/" + A vob->GetVisual()->GetVisualName());
						(callVoidFunc)GetProcAddress(theApp.module, "CleanPropWindow")();
						SelectObject(current_object);
					}
				}

				//Message::Box("Apply 3");

				//cmd << "ApplyProps for vob 3 " << AHEX32((uint)current_object) << endl;

				if (lastName != name)
				{
					RecalcWPBBox(vob);

					if (auto wp = dynamic_cast<zCVobWaypoint*>(vob))
					{
						wp->SetVobName(name);
						auto wpObj = ogame->GetWorld()->wayNet->GetWaypoint(lastName);

						if (wpObj)
						{
							wpObj->SetName(name);
						}
					}

					auto updateName = (onUpdateVobName)GetProcAddress(theApp.module, "UpdateVobName");
					Stack_PushString(GetVobName(vob));
					updateName((uint)vob);
				}
				//Message::Box("Apply 4");

				//cmd << "ApplyProps for vob 4 " << AHEX32((uint)current_object) << endl;
			}

			//vob->SetPositionWorld(vob->GetPositionWorld());

		}
		//Message::Box("Apply 5");
		//cmd << "ApplyProps for vob 5 " << AHEX32((uint)current_object) << endl;
	}


	void SpacerApp::BlockMouseClick(int time)
	{
		mouseClicksBlockedTime = time;
	}

	bool SpacerApp::IsClicksBlocked()
	{
		return mouseClicksBlockedTime > 0;
	}


}