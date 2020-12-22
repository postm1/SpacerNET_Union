

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

		this->spcOpt.Init("spacer_net.ini", true);
	}

	zCVob* SpacerApp::GetSelectedVob()
	{
		return this->pickedVob;
	}

	void SpacerApp::SetSelectedVob(zCVob* vob, zSTRING funcName)
	{
		//std::cout << "Union: select vob " << GetVobName(vob) << " in " << funcName.ToChar() << std::endl;

		OutFile("SetSelectedVob: (" + A GetVobName(vob) + ") in " + A funcName, false);

		pickedVob = vob;


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
		if (ogame && ogame->GetWorld() && ogame->GetCamera() && !isExit && !g_bIsPlayingGame)
		{
			CameraMoving();
			VobMoving();

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
			if (zinput->GetMouseButtonPressedLeft())
			{
				int pickMode = theApp.options.GetIntVal("bTogglePickMaterial") ? 1 : 0;

				if (pickMode == 0)
				{
					theApp.PickVob();
				}
				else if (pickMode == 1)
				{
					theApp.PickMaterial();
				}
				


				zinput->ClearKey(MOUSE_LEFT);
				ClearLMB();

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

		if (!foundVob && zinput->KeyPressed(KEY_LCONTROL))
		{
			/*
			zVEC3 camPos = ogame->GetCameraVob()->GetPositionWorld();
			zVEC3 camAt = ogame->GetCameraVob()->GetAtVectorWorld();

			// надо было mouse3 поставить, ей удобней будет выделять

			BOOL result = ogame->GetWorld()->TraceRayNearestHit(camPos, camAt * 100000, ogame->GetCameraVob(), zTRACERAY_VOB_BBOX);
			if (result && ogame->GetWorld()->traceRayReport.foundVob)
			{
			ogame->GetWorld()->traceRayReport.foundVob->SetDrawBBox3D(TRUE);
			foundVob = ogame->GetWorld()->traceRayReport.foundVob;
			}


			// прицел, правда кривоватый
			zTBSphere3D s;
			s.radius = 1.0f;
			s.center = camPos + camAt * 200;
			s.Draw(zCOLOR(255, 255, 0));

			*/
			/*

			zCVob* camVob = ogame->GetCamera()->connectedVob;
			zCCamera* cam = ogame->GetCamera();

			cmd << Col16(CMD_YELLOW) << "Try TraceRay" << endl;


			zVEC3 ray00, ray, p;
			cam->camMatrixInv.GetTranslation(ray00);
			p.n[VZ] = 1;
			cam->BackProject(ax, ay, p);				// p im camSpace
			p = cam->camMatrixInv * p;					// p im world(obj)Space
			ray = (p - ray00).Normalize();


			ogame->GetWorld()->TraceRayFirstHit(camVob->GetPositionWorld(),
			ray * 3000, (zCVob*)camVob, zTRACERAY_VOB_BBOX);

			print.PrintRed(zSTRING(ray.n[0], 6) + ", " + zSTRING(ray.n[1], 6) + ", " + zSTRING(ray.n[2], 6));

			foundVob = ogame->GetWorld()->traceRayReport.foundVob;

			if (foundVob)
			{
			//foundVob->SetDrawBBox3D(TRUE);
			cmd << Col16(CMD_RED) << "Vob Found: " << GetVobName(foundVob) << endl;
			}



			/*
			zCArray<zCVob*>resVobList;

			zCClassDef* lightClassdef = zCVobAnimate::classDef;

			ogame->GetWorld()->SearchVobListByClass(lightClassdef, resVobList, 0);
			for (int i = 0; i < resVobList.GetNumInList(); i++)
			{
			zCVob* nextVob = resVobList.GetSafe(i);

			if (nextVob)
			{
			nextVob->ignoredByTraceRay = 0;
			}
			}

			cmd << Col16(CMD_YELLOW) << "Try trace" << endl;
			foundVob = ogame->GetWorld()->traceRayReport.foundVob;

			for (int i = 0; i < resVobList.GetNumInList(); i++)
			{
			zCVob* nextVob = resVobList.GetSafe(i);

			if (nextVob)
			{
			nextVob->ignoredByTraceRay = 1;
			}
			}
			*/


			zTBBox3D box;

			zCVob* camVob = ogame->GetCamera()->connectedVob;
			zCCamera* cam = ogame->GetCamera();
			zVEC3 currentPos = camVob->GetPositionWorld();
			zCArray<zCVob*> baseVobList;
			zCArray<zCVob*> resVobList;
			zCArray<zCVob*> resultVobList;
			int bBoxSize = 2000;
			zCVob* tryVob = NULL;
			int dist = 0;
			int stepDist = 25;
			bool flagExit = false;


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

			//print.PrintRed(zSTRING(ray.n[0], 6) + ", " + zSTRING(ray.n[1], 6) + ", " + zSTRING(ray.n[2], 6));
			//print.PrintRed(zSTRING(p.n[0], 6) + ", " + zSTRING(p.n[1], 6) + ", " + zSTRING(p.n[2], 6));

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
					resVobList.Insert(vob);
				}
			}

			currentPos += ray * stepDist;

			while (dist <= (bBoxSize + 200) && !flagExit)
			{
				for (int i = 0; i<resVobList.GetNumInList(); i++) {

					zCVob* vob = resVobList[i];
					zTBBox3D bbox = vob->GetVisual()->GetBBox3D();
					float length = 0;

					length = (float)((int)(box.maxs.n[VX] - box.mins.n[VX]));
					//case 1: result = (float)((int)(box.maxs.n[VY] - box.mins.n[VY])); break;
					//case 2: result = (float)((int)(box.maxs.n[VZ] - box.mins.n[VZ])); break;
					float angle = GetAngleBetweenVobs(camVob, vob, camVob->GetAtVectorWorld());


					if (currentPos.Distance(vob->GetPositionWorld()) <= 150
						&& angle <= 55)
					{
						//print.PrintGreen(zSTRING(angle, 6));
						//print.PrintRed(A length);
						//cmd << Col16(CMD_YELLOW) << "Vob picked: " << GetVobName(resVobList[i]) << endl;

						resultVobList.Insert(vob);
						flagExit = true;
						break;
					}

				}

				currentPos += (ray * stepDist);
				dist += stepDist;
			}


			int minDist = 10000;

			if (resultVobList.GetNumInList() > 0)
			{
				if (resultVobList.GetNumInList() > 1)
				{
					for (int i = 0; i < resultVobList.GetNumInList(); i++)
					{
						int dist = Dist(camVob, resultVobList[i]);

						if (dist < minDist)
						{
							tryVob = resultVobList[i];
							minDist = dist;
						}
					}
					foundVob = tryVob;
				}
				else
				{
					foundVob = resultVobList[0];
				}
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