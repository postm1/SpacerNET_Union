// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	struct CameraMovement
	{
		// camera rotation angles
		float yaw;
		float pitch;
		// mouse coordinates on the last frame
		POINT mousePrev;
		// спр€тан ли курсор
		bool hideCursor;

		float lastFrameTime;

		int vid_x;
		int vid_y;

		CameraMovement::CameraMovement()
		{
			yaw = 90.0f;
			pitch = 0.0f;
			hideCursor = false;
			lastFrameTime = 0.0f;
			vid_x = -1;
			vid_y = -1;
		}

	} camMov;


	// The mouse may go beyond the УborderФ of the screen, so we place it in the center if necessary
	bool IsSetMouseCenter(POINT& mouse)
	{
		bool flag = false;

		int x = camMov.vid_x;
		int y = camMov.vid_y;

		if (mouse.x > x * 75 / 100)
		{
			mouse.x = x / 2;
			flag = true;
		}

		if (mouse.x < 200)
		{
			mouse.x = x / 2;
			flag = true;
		}

		if (mouse.y < 200)
		{
			mouse.y = y / 2;
			flag = true;
		}

		if (mouse.y > y * 75 / 100)
		{

			mouse.y = y / 2;
			flag = true;
		}

		return flag;
	}


	float		now;
	float		time_corr;


	void CameraMoving()
	{
		if (!ogame->GetCamera() || theApp.camMan.cameraRun)
		{
			return;
		}

		zCVob* movvob = ogame->GetCamera()->connectedVob;




		POINT mouse;
		POINT delta;

		GetCursorPos(&mouse);


		if (zinput->GetMouseButtonPressedRight())
		{

			//	CRect rc;
			//CMainFrame::mainframe->GetWindowRect(rc);

			if (camMov.vid_x == -1)
			{
				camMov.vid_x = zoptions->ReadInt("VIDEO", "zVidResFullscreenX", 1920);
				camMov.vid_y = zoptions->ReadInt("VIDEO", "zVidResFullscreenY", 1080);
			}

			if (IsSetMouseCenter(mouse))
			{
				SetCursorPos(mouse.x, mouse.y);
				camMov.mousePrev = mouse;
			}


			if (!camMov.hideCursor)
			{
				camMov.hideCursor = true;
				camMov.mousePrev = mouse;
				while (ShowCursor(FALSE) >= 0);


				if (theApp.options.GetIntVal("hideCamWindows") && !theApp.hideWindows)
				{
					(voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")();
					theApp.hideWindows = true;
				}



			}
		}
		else
		{
			if (camMov.hideCursor)
			{
				camMov.hideCursor = false;
				ShowCursor(TRUE);

				if (theApp.hideWindows && !theApp.hideWindowsForce)
				{
					theApp.hideWindows = false;
					(voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
				}

			}

			return;
		}

		zVEC3 unit = movvob->GetAtVectorWorld();
		zVEC3 pos = movvob->GetPositionWorld();
		zVEC3 newVec = zVEC3(0, 0, 0);


		/*************************** TRANSLATION MOVEMENT *****************************/
		float speedTranslate = ((float)(theApp.options.GetIntVal("camTransSpeed")) / 100) * ztimer->frameTimeFloat;


		if (keys.KeyPressed("CAMERA_TRANS_FORWARD", false, true))
		{
			newVec += unit.Normalize() * speedTranslate;
		}


		if (keys.KeyPressed("CAMERA_TRANS_BACKWARD", false, true))
		{
			newVec += (unit.Normalize() * speedTranslate).Minus();
		}

		if (keys.KeyPressed("CAMERA_TRANS_LEFT", false, true))
		{
			newVec += unit.Normalize().Cross(zVEC3(0, 1, 0) * speedTranslate);
		}

		if (keys.KeyPressed("CAMERA_TRANS_RIGHT", false, true))
		{
			newVec += unit.Normalize().Cross(zVEC3(0, -1, 0) * speedTranslate);
		}


		if (keys.KeyPressed("CAMERA_TRANS_UP", false, true))
		{

			newVec += zVEC3(0, 1, 0) * speedTranslate;
		}

		if (keys.KeyPressed("CAMERA_TRANS_DOWN", false, true))
		{
			newVec += zVEC3(0, -1, 0) * speedTranslate;
		}


		// accelerate n-times with left shift pressed
		if (keys.KeyPressed("CAM_SPEED_X10", false, true))
		{
			newVec *= 10;
		}

		// slow down n-times with left control pressed
		if (keys.KeyPressed("CAM_SPEED_MINUS_10", false, true))
		{
			newVec *= 0.2;
		}


#if ENGINE == Engine_G1



		if (theApp.mouseWheelKeyCodeG1 == zMOUSE_SCROLL_UP)
		{
			newVec = unit * 2000;
		}

		if (theApp.mouseWheelKeyCodeG1 == zMOUSE_SCROLL_DOWN)
		{
			newVec = unit.Minus() * 2000;
		}


		theApp.mouseWheelKeyCodeG1 = 0;

#else

// very fast movement on the map in the direction of view
if (KeyClick(MOUSE_WHEELUP))
{
	newVec = unit * 2000;
}

if (KeyClick(MOUSE_WHEELDOWN))
{
	newVec = unit.Minus() * 2000;
}
#endif
		

		movvob->SetPositionWorld(pos + newVec);


		/*************************** ROTATION MOVEMENT*****************************/

		delta.x = (mouse.x - camMov.mousePrev.x);
		delta.y = (camMov.mousePrev.y - mouse.y);

		

		camMov.mousePrev = mouse;


		float rotSpeed = (float)(theApp.options.GetIntVal("camRotSpeed")) / 100; // *currentFrameTime;
		// углы поворота
		// += 1.0f * time_corr - friction_rot * rotSpeed * time_corr;


		



		camMov.yaw -= delta.x * rotSpeed;
		camMov.pitch += delta.y * rotSpeed;

		// we limit the maximum angle of the camera so that it does not rotate 360 degrees vertically and the world does not turn upside down
		if (camMov.pitch > 89.0f)
			camMov.pitch = 89.0f;
		if (camMov.pitch < -89.0f)
			camMov.pitch = -89.0f;

		zVEC3 rot;

		rot.n[0] = cos(Radian(camMov.yaw)) * cos(Radian(camMov.pitch));
		rot.n[1] = sin(Radian(camMov.pitch));
		rot.n[2] = sin(Radian(camMov.yaw)) * cos(Radian(camMov.pitch));

		int slerpValue = theApp.options.GetIntVal("slerpRot");


		if (slerpValue == 0)// || ztimer->frameTimeFloat >= 30.0f)
		{
			zVEC3 newDir = rot.Normalize();
			movvob->SetHeadingAtWorld(newDir);
		}
		else if (slerpValue > 0)
		{
			float slerpVal = 1.0f - (float)slerpValue / 100.0;

			//print.PrintRed(zSTRING(slerpVal, 6));

			zVEC3 oldDir = movvob->GetAtVectorWorld();
			zVEC3 newDir = Slerp(oldDir, rot.Normalize(), slerpVal);
			movvob->SetHeadingAtWorld(newDir);
		}
		
		

		camMov.lastFrameTime = ztimer->frameTimeFloat;
	}
}