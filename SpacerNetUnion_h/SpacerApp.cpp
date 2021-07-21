

// Supported with union (c) 2020 Union team
// Union SOURCE file



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

		this->spcOpt.Init("spacer_net.ini", true);
	}

	zCVob* SpacerApp::GetSelectedVob()
	{
		return this->pickedVob;
	}

	void SpacerApp::SetSelectedVob(zCVob* vob, zSTRING funcName)
	{
		pickedVob = vob;


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

		AddVobToRender("", false);

		if (IsAWorldLoaded())
		{
			pfxManager.RemovePFX();
			if (ogame->GetWorld()) ogame->GetWorld()->DisposeWorld();
		}



		if (zmusic) zmusic->Stop();
		if (zsound) zsound->StopAllSounds();
		zinput->ClearKeyBuffer();

		isMerged = false;
		isMesh = false;
		levelReady = false;
		treeIsReady = false;
		firstTimeZenSaved = false;


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
			if (!isGrattControlActive)
			{
				ControllerEvents.CameraMoving = zCM_DISABLED;
				ControllerEvents.SelectedVobs.Clear();
				ControllerEvents.PickedVob = NULL;

				CameraMoving();
				VobMoving();
			}
			else
			{
				ControllerEvents.CameraMoving = zCM_ENABLED;

				UpdateGrattController();
			}

			VobKeys();
			

		}
	}

	inline bool CheckDx11() {
		HMODULE module = GetModuleHandle("ddraw.dll");

		return
			module != Null &&
			GetProcAddress(module, "GDX_AddPointLocator");
	}

	bool SpacerApp::IsDx11Active()
	{
		static bool isEnabled = CheckDx11();
		return isEnabled;
	}
	void SpacerApp::RenderSelectedVobBbox()
	{
		if (!IsDx11Active())
		{
			return;
		}

		static zCOLOR colUp = zCOLOR(255, 255, 255);
		static zCOLOR colAt = zCOLOR(255, 0, 0);
		static zCOLOR colRight = zCOLOR(0, 255, 0);


		zREAL size = 0.0f;

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
	}

	void SpacerApp::UpdateGrattController()
	{
		POINT  cur;
		_GetCursorPos(&cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		float rw = rect.right - rect.left;
		float rh = rect.bottom - rect.top;

		//print.PrintRed(zSTRING((float)cur.x, 6) + "/ " + zSTRING((float)cur.y, 6));

		float ax = (float)cur.x / rw * (float)zrenderer->vid_xdim;
		float ay = (float)cur.y / rh * (float)zrenderer->vid_ydim;

		ax = screen->anx(ax);
		ay = screen->any(ay);

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

		if (GameFocused())
		{
			if (isGrattControlActive)
			{
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
			}
			
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








	void _GetCursorPos(POINT* cur)
	{
		GetCursorPos(cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		cur->x -= rect.left;
		cur->y -= rect.top;
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
		POINT  cur;
		_GetCursorPos(&cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		float rw = rect.right - rect.left;
		float rh = rect.bottom - rect.top;

		//print.PrintRed(zSTRING((float)cur.x, 6) + "/ " + zSTRING((float)cur.y, 6));

		float ax = (float)cur.x / rw * (float)zrenderer->vid_xdim;
		float ay = (float)cur.y / rh * (float)zrenderer->vid_ydim;


		//print.PrintRed(zSTRING(ax, 6) + "// " + zSTRING(ay, 6));

		if (ay <= 0)
		{
			return;
		}

		mm.OnPick(ax, ay);
	}

	

	zVEC3 IsSphIntersect(zVEC3 originPos, zVEC3 sp_pos, zVEC3 radNorm, float radius_sphere)
	{
		zVEC3 diff = originPos - sp_pos;
		float A_Point = radNorm.Dot(radNorm);
		float B_Point = 2 * diff.Dot(radNorm);
		float C_Point = pow(diff.Length(), 2) - pow(radius_sphere, 2);

		float D = B_Point * B_Point - 4 * A_Point * C_Point;

		if (D < 0.0) return NULL;

		zVEC3 t1 = originPos + radNorm*(-B_Point - sqrt(D)) / (2 * A_Point);
		zVEC3 t2 = originPos + radNorm*(-B_Point + sqrt(D)) / (2 * A_Point);

		return t1.Length() <= t2.Length() ? t1 : t2;
	}

	void SpacerApp::PickVob()
	{
		//zCVob* CSpacerView::PickSingle()

		POINT  cur;
		_GetCursorPos(&cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		float rw = rect.right - rect.left;
		float rh = rect.bottom - rect.top;

		//print.PrintRed(zSTRING((float)cur.x, 6) + "/ " + zSTRING((float)cur.y, 6));

		float ax = (float)cur.x / rw * (float)zrenderer->vid_xdim;
		float ay = (float)cur.y / rh * (float)zrenderer->vid_ydim;


		//print.PrintRed(zSTRING(ax, 6) + "// " + zSTRING(ay, 6));

		if (ay <= 0)
		{
			return;
		}

		if (pickedVob && !isGrattControlActive)
		{
			return;
		}

		ogame->GetWorld()->PickScene(*ogame->GetCamera(), ax, ay, -1);

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
			cam->BackProject(ax, ay, p);				// p im camSpace
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

		auto addItem = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

		Stack_PushString(className);
		Stack_PushString(str);

		addItem();


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
		}
		else
		{
			auto addItem = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

			Stack_PushString("");
			Stack_PushString("");

			addItem();
		}



	}


	void SpacerApp::ApplyProps(CString str, CString nameCurrent, CString visualStr)
	{
		zSTRING propString = zSTRING(str);
		zSTRING name = zSTRING(nameCurrent);
		zSTRING visual = zSTRING(visualStr);

		//OutFile("ApplyProps: " + A name, true);

		if (current_object)
		{

			zCVob* oldVob = dynamic_cast<zCVob*>(current_object);

			zSTRING lastName = current_object->GetObjectName();
			zSTRING lastVisual = oldVob && oldVob->GetVisual() ? oldVob->GetVisual()->GetVisualName() : "";


			zCBuffer buf(propString.ToChar(), propString.Length());
			zCArchiver* arch = zarcFactory->CreateArchiverRead(&buf, 0);
			arch->SetStringEOL(zSTRING("\n"));



			arch->ReadObject(current_object);
			arch->Close();
			zRELEASE(arch);


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


				if (lastName != name)
				{
					RecalcWPBBox(vob);
					auto updateName = (onUpdateVobName)GetProcAddress(theApp.module, "UpdateVobName");
					Stack_PushString(GetVobName(vob));
					updateName((uint)vob);
				}

			}

		}


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