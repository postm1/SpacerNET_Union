// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void CameraManager::Init()
	{
		cur_cam = NULL;
		tempCam = NULL;
		cur_vob = NULL;
		blockUpdateCamWindow = false;
		blockRename = false;
		cameraRun = false;

		hideVisualWhileActive = true;
		timeDurationOnSpacer = 5;


		textView = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
		textView->SetFont("FONT_OLD_10_WHITE.TGA");
		textView->SetFontColor(zCOLOR(255, 255, 255, 255));
		screen->InsertItem(textView);
	}


	void CameraManager::Reset()
	{
		cameraRun = false;
		blockUpdateCamWindow = false;
		blockRename = false;
		cur_cam = NULL;
		tempCam = NULL;
		cur_vob = NULL;
		hideVisualWhileActive = false;

		GetProcAddress(theApp.module, "OnCameraClear_Interface")();
	}

	void CameraManager::OnChangeKey(int key)
	{
		//cmd << "CameraManager::OnChangeKey: " << key << endl;

		if (key > 0)
		{
			if (!cur_cam)
			{
				cur_cam = tempCam;
			}
			
			if (!cur_cam)
			{
				return;
			}

			zCVob* v = NULL;

			if(cur_cam->camKeysFOR == zCCSCamera::zCAMTRJ_KEY_FOR_OBJECT) {
				// suche refCam, und setzt ihn
				//zSTRING refCamVobName = zSTRING(m_ref_cam.GetBuffer(0));
				//if (!refCamVobName.IsEmpty()) v = CSpacerDoc::doc->GetWorld()->SearchVobByName(refCamVobName);

				zCCSCamera_EventMsgActivate* msg_c = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETCAMREFERENCE);

				if (!v) msg_c->MD_SetVobParam(cur_cam);
				else	msg_c->MD_SetVobParam(v);

				cur_cam->OnMessage(msg_c, NULL);
			}

			if (cur_cam->targetKeysFOR == zCCSCamera::zCAMTRJ_KEY_FOR_OBJECT) {

				//zSTRING refTargetVobName = zSTRING(m_ref_target.GetBuffer(0));
				////if (!refTargetVobName.IsEmpty()) v = CSpacerDoc::doc->GetWorld()->SearchVobByName(refTargetVobName);
				//else v = 0;

				zCCSCamera_EventMsgActivate* msg_t = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETTARGETREFERENCE);
				if (!v) msg_t->MD_SetVobParam(cur_cam);
				else	msg_t->MD_SetVobParam(v);

				if (cur_cam->GetNumTargetKeys() == 0) lastPresetAutoAddKF = TRUE;
				else								  lastPresetAutoAddKF = FALSE;

				cur_cam->OnMessage(msg_t, NULL);

			}


			// hole EVManager
			// sende EV_Play
			zCCSCamera_EventMsg* msg = new zCCSCamera_EventMsg(zCCSCamera_EventMsg::EV_GOTO_KEY);
			msg->time = timeDurationOnSpacer;
			msg->key = key;
			msg->kfType = KF_CAM;
			cur_cam->OnMessage(msg, NULL);
			cur_cam->SetShowVisual(FALSE);

			cur_cam = NULL;
		}
	}

	void CameraManager::OnRun()
	{
		if (!cur_cam || cameraRun) return;

		zCVob* v = 0;

		if (cur_cam->autoCamFocusVobName.Length() > 0)
		{
			//v = ogame->GetWorld()->SearchVobByName(cur_cam->autoCamFocusVobName);
			//cmd << "vob found!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		}



		cur_cam->GetEM()->Clear();

		if (cur_cam->camKeysFOR == zCCSCamera::zCAMTRJ_KEY_FOR_OBJECT)
		{

			zCCSCamera_EventMsgActivate* msg_c = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETCAMREFERENCE);

			if (!v) msg_c->MD_SetVobParam(cur_cam);
			else	msg_c->MD_SetVobParam(v);

			cur_cam->OnMessage(msg_c, NULL);
		}

		if (cur_cam->targetKeysFOR == zCCSCamera::zCAMTRJ_KEY_FOR_OBJECT)
		{
			zCCSCamera_EventMsgActivate* msg_t = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETTARGETREFERENCE);
			if (!v) msg_t->MD_SetVobParam(cur_cam);
			else	msg_t->MD_SetVobParam(v);

			if (cur_cam->GetNumTargetKeys() == 0) lastPresetAutoAddKF = TRUE;
			else								  lastPresetAutoAddKF = FALSE;

			cur_cam->OnMessage(msg_t, NULL);

		}

		
		cur_cam->Refresh();
		
		// hole EVManager
		// sende EV_Play
		zCCSCamera_EventMsg* msg = new zCCSCamera_EventMsg(zCCSCamera_EventMsg::EV_PLAY);
		lastPresetTime = cur_cam->totalTime;

		//cmd << "Run: time " << timeDurationOnSpacer << " Hide: " << hideVisualWhileActive << endl;
		msg->time = timeDurationOnSpacer;

		cur_cam->OnMessage(msg, NULL);
		

		if (hideVisualWhileActive)
		{
			cur_cam->SetShowVisual(FALSE);

			hideHelpVisualsTemp = zCVob::s_showHelperVisuals;
			zCVob::SetShowHelperVisuals(FALSE);
		}


		tempCam = cur_cam;

		blockUpdateCamWindow = true;
		theApp.SetSelectedVob(NULL, "Cam OnRun");
		blockUpdateCamWindow = false;
		cameraRun = true;
	}

	void CameraManager::OnStop()
	{

		cur_cam = tempCam;

		if (cur_cam == 0) return;
		// hole EVManager
		// sende EV_Play
		zCCSCamera_EventMsgActivate* msg_t = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETTARGETREFERENCE);
		zCCSCamera_EventMsgActivate* msg_c = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETCAMREFERENCE);

		if (lastPresetAutoAddKF) msg_t->MD_SetVobParam(0);
		else					 msg_t->MD_SetVobParam(cur_cam);

		msg_c->MD_SetVobParam(cur_cam);

		if (cur_cam->GetNumTargetKeys()>0) cur_cam->OnMessage(msg_t, NULL);

		cur_cam->OnMessage(msg_c, NULL);
		cur_cam->OnMessage(msg_t, NULL);

		zCCSCamera_EventMsg* msg = new zCCSCamera_EventMsg(zCCSCamera_EventMsg::EV_STOP);
		cur_cam->OnMessage(msg, NULL);

		cur_cam->totalTime = lastPresetTime;

		cur_cam->SetShowVisual(TRUE);

		cameraRun = false;

		if (hideVisualWhileActive)
		{
			if (hideHelpVisualsTemp)
			{
				zCVob::SetShowHelperVisuals(TRUE);
			}
		}

		theApp.SetSelectedVob(cur_cam, "CamStop");
	}

	void CameraManager::Loop()
	{
		if (!cur_cam && textView)
		{
			textView->ClrPrintwin();
		}
		else if (cur_cam && textView)
		{
			auto pCamera = ogame->GetCamera();

			if (!pCamera) return;


			textView->ClrPrintwin();

			if (cameraRun) return;
			/*
			for (int i = 0; i < cur_cam->GetNumCamKeys() - 1; i++)
			{
				auto key1 = cur_cam->posKeys.GetSafe(i);
				auto key2 = cur_cam->posKeys.GetSafe(i+1);

				if (key1 && key2)
				{
					auto start = key1->GetPositionWorld();
					auto end = key2->GetPositionWorld();

					zlineCache->Line3D(start, end, GFX_RED, TRUE);
				}
			}
			*/

			bool endOrStartIndex = false;

			for (int i = 0; i < cur_cam->GetNumCamKeys(); i++)
			{
				auto key = cur_cam->posKeys.GetSafe(i);
				
				if (key)
				{
					auto start = key->GetPositionWorld();
					auto end = start + zVEC3(0, 50, 0);
					auto text = key->GetVobName();

					endOrStartIndex = false;

					if (i == 0)
					{
						text += " (START)";
						endOrStartIndex = true;
					}
					else if (i == cur_cam->GetNumCamKeys() - 1)
					{
						text += " (END)";
						endOrStartIndex = true;
					}

					zlineCache->Line3D(start, end, GFX_RED, TRUE);


					zVEC3 vWorldPos = end + zVEC3(0, 10, 0);

					// transform the world position to pixels on screen
					float fPixelPosX = FLT_MAX, fPixelPosY = FLT_MAX;
					if (vWorldPos[0] != FLT_MAX) {
						pCamera->Activate();
						zVEC3 vec = pCamera->camMatrix * vWorldPos;
						if (vec[2] > 0)
							pCamera->Project(&vec, fPixelPosX, fPixelPosY);
					}

					if (fPixelPosX != FLT_MAX) {
						fPixelPosX -= screen->nax(screen->FontSize(text) / 2);
						int x = screen->anx(fPixelPosX);
						int y = screen->any(fPixelPosY);

						zClamp(y, screen->FontY(), 8191 - screen->FontY());
						zClamp(x, 0, 8191 - screen->FontSize(text));

						if (endOrStartIndex)
						{
							textView->SetFontColor(GFX_YELLOW);
						}
						else
						{
							textView->SetFontColor(GFX_WHITE);
						}

						textView->Print(x, y, text);
					}

				}
			}

			for (int i = 0; i < cur_cam->GetNumTargetKeys(); i++)
			{
				auto key = cur_cam->targetKeys.GetSafe(i);

				if (key)
				{
					auto start = key->GetPositionWorld();
					auto end = start + zVEC3(0, 50, 0);
					auto text = key->GetVobName();


					endOrStartIndex = false;

					if (i == 0)
					{
						text += " (START)";
						endOrStartIndex = true;
					}
					else if (i == cur_cam->GetNumTargetKeys() - 1)
					{
						text += " (END)";
						endOrStartIndex = true;
					}

					zlineCache->Line3D(start, end, GFX_GREEN, TRUE);


					zVEC3 vWorldPos = end + zVEC3(0, 10, 0);

					// transform the world position to pixels on screen
					float fPixelPosX = FLT_MAX, fPixelPosY = FLT_MAX;
					if (vWorldPos[0] != FLT_MAX) {
						pCamera->Activate();
						zVEC3 vec = pCamera->camMatrix * vWorldPos;
						if (vec[2] > 0)
							pCamera->Project(&vec, fPixelPosX, fPixelPosY);
					}

					if (fPixelPosX != FLT_MAX) {
						fPixelPosX -= screen->nax(screen->FontSize(text) / 2);
						int x = screen->anx(fPixelPosX);
						int y = screen->any(fPixelPosY);

						zClamp(y, screen->FontY(), 8191 - screen->FontY());
						zClamp(x, 0, 8191 - screen->FontSize(text));

						if (endOrStartIndex)
						{
							textView->SetFontColor(GFX_GREEN);
						}
						else
						{
							textView->SetFontColor(GFX_WHITE);
						}
						textView->Print(x, y, text);
					}

				}
			}
		}
	}


	void CameraManager::OnRenameSplineKey(int index, CString name)
	{
		//cmd << "OnRenameSplineKey: " << name << " " << index << endl;

		Stack_PushString(name);
		Stack_PushInt(index);

		GetProcAddress(theApp.module, "OnRenameSplineKey_Interface")();
	}

	void CameraManager::OnRenameTargetKey(int index, CString name)
	{
		//cmd << "OnRenameTargetKey: " << name << " " << index << endl;

		Stack_PushString(name);
		Stack_PushInt(index);

		GetProcAddress(theApp.module, "OnRenameTargetKey_Interface")();
	}
	
	void CameraManager::OnSelectCameraVob()
	{
		
		cmd << "OnSelectCameraVob" << endl;


		if (blockRename)
		{
			cmd << "block rename" << endl;
			return;
		}
		if (blockUpdateCamWindow)
		{
			cmd << "blockUpdateCamWindow" << endl;
			return;
		}

		if (!cur_cam) return;

		cur_cam->SetDrawEnabled(TRUE);
		cur_cam->Refresh();

		auto tempCam = cur_cam;

		GetProcAddress(theApp.module, "OnCameraClear_Interface")();

		bool renameFound = false;

		for (int i = 0; i < cur_cam->GetNumCamKeys(); i++)
		{
			auto key = cur_cam->posKeys.GetSafe(i);

			if (key)
			{
				zSTRING name = key->GetVobName();

				if (name.Length() == 0)
				{
					cmd << "Found bad name POS: " + i << endl;

					name = "KEY_POS_" + Z(i + 1);
					key->SetVobName(name);

					renameFound = true;
					blockRename = true;
					auto updateName = (onUpdateVobName)GetProcAddress(theApp.module, "UpdateVobName");
					Stack_PushString(name);
					updateName((uint)key);
					
				}

				Stack_PushString(name);
				GetProcAddress(theApp.module, "OnInsertSplineKey_Interface")();

				
			}
		}

		for (int i = 0; i < cur_cam->GetNumTargetKeys(); i++)
		{
			auto key = cur_cam->targetKeys.GetSafe(i);

			if (key)
			{
				zSTRING name = key->GetVobName();

				if (name.Length() == 0)
				{
					cmd << "Found bad name TARGET: " + i << endl;

					name = "KEY_TARGET_" + Z(i + 1);
					key->SetVobName(name);
					auto updateName = (onUpdateVobName)GetProcAddress(theApp.module, "UpdateVobName");
					Stack_PushString(name);
					updateName((uint)key);
					renameFound = true;
					blockRename = true;
				}
				Stack_PushString(name);
				GetProcAddress(theApp.module, "OnInsertTargetKey_Interface")();


			}
		}

		blockRename = false;

		if (renameFound)
		{
			theApp.SetSelectedVob(NULL, "CameraRenameforce");
			GetProcAddress(theApp.module, "OnCameraClear_Interface")();
			print.PrintRed(GetLang("CAMERA_ITEMS_FORCE_RENAME"));

			theApp.SetSelectedVob(tempCam, "RenameSetCamAfter");

			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((int)tempCam);

			theApp.SelectObject(tempCam);
		}

		

		//Stack_PushInt(cur_cam->GetNumCamKeys);
		//GetProcAddress(theApp.module, "OnUpdateMaxKey_Interface")();
	}


	//Extern_InsertNewTargetKey

	void CameraManager::InsertNewTargetKey()
	{
		//cmd << "InsertNewTargetKey" << endl;
		int foundPos = -1;


		blockUpdateCamWindow = true;
		if (cur_cam)
		{
			zCVob* v = NULL;// ogame->GetWorld()->SearchVobByName(cur_cam->autoCamFocusVobName);

			if (cur_cam->targetKeysFOR == zCCSCamera::zTCamTrj_FOR::zCAMTRJ_KEY_FOR_OBJECT && !v)
			{

				print.PrintRed(GetLang("CANT_APPLY_CAMERA_NEWKEY"));
				return;
			}
			zCCamTrj_KeyFrame*	parentKeyframe = dynamic_cast<zCCamTrj_KeyFrame*>(cur_cam);

			parentKeyframe = NULL;

			if (parentKeyframe)
			{
				if (parentKeyframe->type == KF_TARGET)
				{
					foundPos = cur_cam->SearchTargetKey(parentKeyframe);
				}
				else if (parentKeyframe->type == KF_CAM)
				{
					foundPos = cur_cam->SearchCamKey(parentKeyframe);
				}
				else
				{
					return;
				}

			}


			zCCamTrj_KeyFrame* newkey = new zCCamTrj_KeyFrame();

			newkey->SetCollDetStat(FALSE);
			newkey->SetCollDetDyn(FALSE);
			newkey->type = KF_TARGET;
			zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
			zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

			newkey->SetPositionWorld(pos + dir * 200);

			newkey->SetVobName("KEY_TARGET_" + Z(cur_cam->GetNumTargetKeys() + 1));

			//cmd << "Set new target key: " << newkey->GetVobName() << endl;
			cur_cam->Refresh();
			InsertIntoWorld(newkey, cur_cam);
			cur_cam->Refresh();

			if (!cur_cam)
			{
				cmd << "NO CAM!!!!!!!!!! " << endl;
			}
			//cmd << "InsertIntoWorld target ok " << endl;

			if (cur_cam->GetNumTargetKeys() == 0)
			{
				//cmd << "InsertTargetKey " << endl;
				cur_cam->InsertTargetKey(newkey);
			}
			else
			{
				if (parentKeyframe) {
					cur_cam->InsertTargetKeyAtPos(newkey, foundPos + 1);
				}
				else
				{
					//cmd << "InsertTargetKeyAtPos " << cur_cam->GetNumTargetKeys() << endl;
					cur_cam->InsertTargetKeyAtPos(newkey, cur_cam->GetNumTargetKeys()); // Mitten drin einfuegen
				}
			}

			newkey->SetShowVisual(TRUE);
			newkey->Release();

			cur_cam->Refresh();
			//cmd << "Insert target ok.. .try to add into c# list" << endl;

			Stack_PushString(newkey->GetVobName());
			(callVoidFunc)GetProcAddress(theApp.module, "OnInsertTargetKey_Interface")();
		}

		blockUpdateCamWindow = false;
	}
	void CameraManager::InsertNewSplineKey()
	{
		//cmd << "InsertNewSplineKey" << endl;
		int foundPos = -1;
		

		blockUpdateCamWindow = true;
		if (cur_cam)
		{

			zCVob* v = NULL; // ogame->GetWorld()->SearchVobByName(cur_cam->autoCamFocusVobName);

			if (cur_cam->camKeysFOR == zCCSCamera::zTCamTrj_FOR::zCAMTRJ_KEY_FOR_OBJECT && !v)
			{

				print.PrintRed(GetLang("CANT_APPLY_CAMERA_NEWKEY"));
				return;
			}
			

			zCCamTrj_KeyFrame*	parentKeyframe = dynamic_cast<zCCamTrj_KeyFrame*>(cur_cam);

			parentKeyframe = NULL;

			if (parentKeyframe)
			{
				if (parentKeyframe->type == KF_TARGET)
				{
					foundPos = cur_cam->SearchTargetKey(parentKeyframe);
				}
				else if (parentKeyframe->type == KF_CAM)
				{
					foundPos = cur_cam->SearchCamKey(parentKeyframe);
				}
				else
				{
					return;
				}

			}

			
			zCCamTrj_KeyFrame* newkey = new zCCamTrj_KeyFrame();

			newkey->SetCollDetStat(FALSE);
			newkey->SetCollDetDyn(FALSE);

			zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
			zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

			newkey->SetPositionWorld(pos + dir * 200);

			newkey->SetVobName("KEY_POS_" + Z (cur_cam->GetNumCamKeys() + 1));

			//cmd << "Set new key: " << newkey->GetVobName() << endl;
			cur_cam->Refresh();
			InsertIntoWorld(newkey, cur_cam);
			cur_cam->Refresh();

			//cmd << "InsertIntoWorld ok " << endl;
			//newkey->type = KF_CAM;
			if (cur_cam->GetNumCamKeys() == 0)
			{
				//cmd << "InsertCamKey " << endl;
				cur_cam->InsertCamKey(newkey);
			}
			else
			{
				if (parentKeyframe) {
					cur_cam->InsertCamKeyAtPos(newkey, foundPos + 1);
				}
				else 
				{
					//cmd << "InsertCamKeyAtPos " << cur_cam->GetNumCamKeys()  << endl;
					cur_cam->InsertCamKeyAtPos(newkey, cur_cam->GetNumCamKeys()); // Mitten drin einfuegen
				}
			}

			//cmd << "keys insert ok.. " << endl;

			newkey->SetShowVisual(TRUE);
			newkey->Release();

			cur_cam->Refresh();
			//cmd << "Insert ok.. .try to add into c# list" << endl;

			Stack_PushString(newkey->GetVobName());
			(callVoidFunc)GetProcAddress(theApp.module, "OnInsertSplineKey_Interface")();
		}

		blockUpdateCamWindow = false;
	}
	
	void CameraManager::InsertPosKeyAtIndex(int index)
	{
		if (!cur_cam) return;

		auto pKey = cur_cam->posKeys[index];

		//cmd << "InsertPosKeyAtIndex: " << index << endl;
		if (pKey)
		{
			int foundPos = -1;


		
			if (cur_cam)
			{
				
				if (cur_cam->camKeysFOR == zCCSCamera::zTCamTrj_FOR::zCAMTRJ_KEY_FOR_OBJECT && !cur_vob)
				{

					print.PrintRed(GetLang("CANT_APPLY_CAMERA_NEWKEY"));
					return;
				}

				zCCamTrj_KeyFrame*	parentKeyframe = dynamic_cast<zCCamTrj_KeyFrame*>(cur_cam);

				parentKeyframe = NULL;

				if (parentKeyframe)
				{
					if (parentKeyframe->type == KF_TARGET)
					{
						foundPos = cur_cam->SearchTargetKey(parentKeyframe);
					}
					else if (parentKeyframe->type == KF_CAM)
					{
						foundPos = cur_cam->SearchCamKey(parentKeyframe);
					}
					else
					{
						return;
					}

				}

				blockUpdateCamWindow = true;

				zCCamTrj_KeyFrame* newkey = new zCCamTrj_KeyFrame();

				newkey->SetCollDetStat(FALSE);
				newkey->SetCollDetDyn(FALSE);

				zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
				zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				newkey->SetPositionWorld(pos + dir * 200);


				if (cur_cam->GetNumCamKeys() == 0 || index + 1 == cur_cam->GetNumCamKeys())
				{
					newkey->SetVobName("KEY_POS_" + Z(cur_cam->GetNumCamKeys() + 1));
				}
				else
				{
					//zSTRING name1 = cur_cam->posKeys[index]->GetVobName();
					//zSTRING name2 = cur_cam->posKeys[index+1]->GetVobName();
					//newkey->SetVobName("KEY_POS_" + name1 + "_" + Z name2);
					zSTRING name = cur_cam->posKeys[index]->GetVobName() + "_A";

					newkey->SetVobName(name);
				}

				//cmd << "Set new key: " << newkey->GetVobName() << endl;
				cur_cam->Refresh();
				InsertIntoWorld(newkey, cur_cam);
				cur_cam->Refresh();

				//cmd << "InsertIntoWorld ok " << endl;
				//newkey->type = KF_CAM;
				if (cur_cam->GetNumCamKeys() == 0)
				{
					//cmd << "InsertCamKey " << endl;
					cur_cam->InsertCamKey(newkey);
				}
				else
				{
					if (parentKeyframe) {
						cur_cam->InsertCamKeyAtPos(newkey, foundPos + 1);
					}
					else
					{
						//cmd << "InsertCamKeyAtPos " << cur_cam->GetNumCamKeys() << endl;
						cur_cam->InsertCamKeyAtPos(newkey, index+1); // Mitten drin einfuegen
					}
				}

				//cmd << "keys insert ok.. " << endl;

				newkey->SetShowVisual(TRUE);
				newkey->Release();

				cur_cam->Refresh();
				//cmd << "Insert ok.. .try to add into c# list" << endl;

				blockUpdateCamWindow = false;
				auto tempVob = cur_cam;

				theApp.SetSelectedVob(NULL, "InsertPosKeyAtIndex");
				GetProcAddress(theApp.module, "OnCameraClear_Interface")();

				theApp.SetSelectedVob(tempVob, "InsertPosKeyAtIndex");

				auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
				onSelect((int)tempVob);

				theApp.SelectObject(tempVob);


				cur_cam = tempVob;
				//Stack_PushString(newkey->GetVobName());
				//(callVoidFunc)GetProcAddress(theApp.module, "OnInsertSplineKey_Interface")();
			}

			
		}
	}
	void CameraManager::InsertTargetKeyAtIndex(int index)
	{
		if (!cur_cam) return;

		auto pKey = cur_cam->targetKeys[index];

		//cmd << "InsertTargetKeyAtIndex: " << index << endl;

		if (pKey)
		{
			int foundPos = -1;



			if (cur_cam)
			{

				if (cur_cam->targetKeysFOR == zCCSCamera::zTCamTrj_FOR::zCAMTRJ_KEY_FOR_OBJECT && !cur_vob)
				{

					print.PrintRed(GetLang("CANT_APPLY_CAMERA_NEWKEY"));
					return;
				}

				zCCamTrj_KeyFrame*	parentKeyframe = dynamic_cast<zCCamTrj_KeyFrame*>(cur_cam);

				parentKeyframe = NULL;

				if (parentKeyframe)
				{
					if (parentKeyframe->type == KF_TARGET)
					{
						foundPos = cur_cam->SearchTargetKey(parentKeyframe);
					}
					else if (parentKeyframe->type == KF_CAM)
					{
						foundPos = cur_cam->SearchCamKey(parentKeyframe);
					}
					else
					{
						return;
					}

				}

				blockUpdateCamWindow = true;

				zCCamTrj_KeyFrame* newkey = new zCCamTrj_KeyFrame();

				newkey->SetCollDetStat(FALSE);
				newkey->SetCollDetDyn(FALSE);

				zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
				zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				newkey->SetPositionWorld(pos + dir * 200);


				if (cur_cam->GetNumTargetKeys() == 0 || index + 1 == cur_cam->GetNumTargetKeys())
				{
					newkey->SetVobName("KEY_TARGET_" + Z(cur_cam->GetNumTargetKeys() + 1));
				}
				else
				{
					//zSTRING name1 = cur_cam->posKeys[index]->GetVobName();
					//zSTRING name2 = cur_cam->posKeys[index+1]->GetVobName();
					//newkey->SetVobName("KEY_TARGET_" + name1 + "_" + Z name2);
					//newkey->SetVobName("KEY_TARGET_" + Z(index+1) + "_" + Z(index + 2));

					zSTRING name = cur_cam->targetKeys[index]->GetVobName() + "_A";
					newkey->SetVobName(name);
				}

				//cmd << "Set new key: " << newkey->GetVobName() << endl;
				cur_cam->Refresh();
				InsertIntoWorld(newkey, cur_cam);
				cur_cam->Refresh();

				//cmd << "InsertIntoWorld ok " << endl;
				//newkey->type = KF_CAM;
				if (cur_cam->GetNumTargetKeys() == 0)
				{
					//cmd << "InsertTargetKey " << endl;
					cur_cam->InsertTargetKey(newkey);
				}
				else
				{
					if (parentKeyframe) {
						cur_cam->InsertTargetKeyAtPos(newkey, foundPos + 1);
					}
					else
					{
						//cmd << "InsertCamKeyAtPos " << cur_cam->GetNumTargetKeys() << endl;
						cur_cam->InsertTargetKeyAtPos(newkey, index + 1); // Mitten drin einfuegen
					}
				}

				//cmd << "keys insert ok.. " << endl;

				newkey->SetShowVisual(TRUE);
				newkey->Release();

				cur_cam->Refresh();
				//cmd << "Insert ok.. .try to add into c# list" << endl;

				blockUpdateCamWindow = false;
				auto tempVob = cur_cam;

				theApp.SetSelectedVob(NULL, "InsertTargetKeyAtIndex");
				GetProcAddress(theApp.module, "OnCameraClear_Interface")();

				theApp.SetSelectedVob(tempVob, "InsertTargetKeyAtIndex");

				auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
				onSelect((int)tempVob);

				theApp.SelectObject(tempVob);


				cur_cam = tempVob;
			}
		}
	}
	
	void CameraManager::SelectSplineKeyByIndex(int index)
	{
		if (!cur_cam) return;

		auto pKey = cur_cam->posKeys[index];

		if (pKey)
		{
			theApp.SetSelectedVob(pKey, "SelectSplineKeyByIndex");


			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((int)pKey);

			theApp.SelectObject(pKey);


		}
	}
	void CameraManager::SelectTargetKeyByIndex(int index)
	{
		if (!cur_cam) return;

		auto pKey = cur_cam->targetKeys[index];

		if (pKey)
		{
			theApp.SetSelectedVob(pKey, "SelectTargetKeyByIndex");

			
			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((int)pKey);
	
			theApp.SelectObject(pKey);
			
			
		}
	}

	void CameraManager::RemoveSplineKeyByIndex(int index)
	{
		if (!cur_cam) return;

		auto pKey = cur_cam->posKeys[index];

		if (pKey)
		{
			auto tempObj = cur_cam;

			cur_cam->RemoveCamKey(pKey);

			//cmd << "try to remove cam key by index: " << index << endl;
			theApp.RemoveVob(pKey);


			cur_cam = tempObj;

			theApp.SetSelectedVob(cur_cam, "RemoveSplineKeyByIndex");

			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((int)cur_cam);

			theApp.SelectObject(cur_cam);

		}
	}

	void CameraManager::RemoveTargetKeyByIndex(int index)
	{
		if (!cur_cam) return;

		auto pKey = cur_cam->targetKeys[index];

		if (pKey)
		{
			auto tempObj = cur_cam;

			cur_cam->RemoveTargetKey(pKey);

			//cmd << "try to remove target key by index: " << index << endl;
			theApp.RemoveVob(pKey);


			cur_cam = tempObj;

			theApp.SetSelectedVob(cur_cam, "RemoveTargetKeyByIndex");

			auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectVob");
			onSelect((int)cur_cam);
	
			theApp.SelectObject(cur_cam);

		}
	}

	void CameraManager::RemoveSplineKey()
	{
		//cmd << "RemoveSplineKey" << endl;
	}
	
	void CameraManager::InsertCam(CString name)
	{
		//cmd << "try to create a new camera: " << name << endl;


		auto parent = theApp.GetSelectedVob();

		if (theApp.globalParent) parent = theApp.globalParent;

		if (parent && parent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;
		};

		if (parent && parent->CastTo<zCCSCamera>())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_CAMERA"));

			return;
		};


		zCCSCamera* newcam = new zCCSCamera();
		newcam->SetVobName(name);

		newcam->SetPhysicsEnabled(FALSE);
		newcam->SetSleepingMode(zVOB_AWAKE_DOAI_ONLY);
		newcam->SetDrawEnabled(true);
		newcam->SetCollDetStat(FALSE);
		newcam->SetCollDetDyn(FALSE);
		newcam->adaptToSurroundings = FALSE;
		newcam->targetKeysFOR = zCCSCamera::zTCamTrj_FOR::zCAMTRJ_KEY_FOR_WORLD;
		newcam->camKeysFOR = zCCSCamera::zTCamTrj_FOR::zCAMTRJ_KEY_FOR_WORLD;

		zCCSCamera_EventMsgActivate* msg_c = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETCAMREFERENCE);
		zCCSCamera_EventMsgActivate* msg_t = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETTARGETREFERENCE);

		msg_c->referenceVob = newcam;
		msg_t->referenceVob = newcam;

		newcam->OnMessage(msg_c, NULL);
		newcam->OnMessage(msg_t, NULL);

		zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


		newcam->SetPositionWorld(pos + dir * 200);
		InsertIntoWorld(newcam, parent);

		newcam->Refresh();

		newcam->SetDrawEnabled(TRUE);
		newcam->Release();


		theApp.SetSelectedVob(newcam, "InsertCam");
		cur_cam = newcam;

	}
}