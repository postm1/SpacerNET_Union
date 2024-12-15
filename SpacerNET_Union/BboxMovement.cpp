// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void SpacerApp::PrepareBboxVobs(zCVob* vob)
	{
		if (vob)
		{
			auto box = vob->GetBBox3DWorld();

			if (!bboxMinsVob)
			{
				bboxMinsVob = new zCVob();
				bboxMinsVob->SetVobName("SPACER_VOB_BBOX_MINS");
				bboxMinsVob->SetShowVisual(TRUE);
				bboxMinsVob->dontWriteIntoArchive = true;
				bboxMinsVob->SetCollDet(FALSE);
				bboxMinsVob->ignoredByTraceRay = true;
				
				bboxMinsVob->SetVisual("INVISIBLE_DRAGVOB.3DS");

				theApp.nextInsertBlocked = true;
				ogame->GetWorld()->AddVob(bboxMinsVob);
				theApp.nextInsertBlocked = false;
			}

			if (!bboxMaxsVob)
			{
				bboxMaxsVob = new zCVob();
				bboxMaxsVob->SetVobName("SPACER_VOB_BBOX_MAXS");
				bboxMaxsVob->SetShowVisual(TRUE);
				bboxMaxsVob->dontWriteIntoArchive = true;
				bboxMaxsVob->SetCollDet(FALSE);
				bboxMaxsVob->SetVisual("INVISIBLE_DRAGVOB.3DS");
				bboxMaxsVob->ignoredByTraceRay = true;


				theApp.nextInsertBlocked = true;
				ogame->GetWorld()->AddVob(bboxMaxsVob);
				theApp.nextInsertBlocked = false;
			}

			
			bboxMinsVob->SetPositionWorld(box.mins);
			bboxMaxsVob->SetPositionWorld(box.maxs);
			
		}
		else
		{
			if (bboxMinsVob)
			{
				bboxMinsVob->SetPositionWorld(zVEC3(0, 0, 0));
			}

			if (bboxMaxsVob)
			{
				bboxMaxsVob->SetPositionWorld(zVEC3(0, 0, 0));
			}
			
		}
	}

	void BoxMoving()
	{
		zCVob* pickedVob = theApp.GetSelectedVob();

		if (!pickedVob)
		{
			return;
		}

		auto pOcVob = pickedVob->CastTo<oCVob>();
		auto pWp = pickedVob->CastTo<zCVobWaypoint>();
		auto pSpot = pickedVob->CastTo<zCVobSpot>();



		if (keys.KeyPressed("TOOL_BBOX_CHANGE", true) && pickedVob)
		{

			if ((pickedVob->GetVisual() || pOcVob || pSpot || pWp))
			{
				print.PrintRed(GetLang("TOOL_BBOX_CANT_WORK"));
				SetSelectedTool(TM_TRANSLATE);
				theApp.isBboxChangeMod = 0;
				return;
			}

			if (GetSelectedTool() != TM_BBOXEDIT)
			{
				print.PrintRed(GetLang("TOOL_BBOX_CHANGE"));
				SetSelectedTool(TM_BBOXEDIT);
				theApp.isBboxChangeMod = 1;

				theApp.PrepareBboxVobs(pickedVob);
			}
			else
			{
				print.PrintRed(GetLang("TOOL_BBOX_CHANGE_LEAVE"));
				SetSelectedTool(TM_TRANSLATE);
				theApp.isBboxChangeMod = 0;
				theApp.PrepareBboxVobs(NULL);
			}
			
			
		}

		if (GetSelectedTool() != TM_BBOXEDIT)
		{
			return;
		}

		auto pickMode = theApp.GetPickMode();



		if (zKeyToggled(KEY_1)) {

			zinput->ClearKeyBuffer();
			print.PrintRed(GetLang("TOOL_BBOX_MAXS"));
			theApp.isBboxChangeMod = 1;
		}

		if (zKeyToggled(KEY_2)) {

			zinput->ClearKeyBuffer();
			print.PrintRed(GetLang("TOOL_BBOX_MINS"));
			theApp.isBboxChangeMod = 2;
		}




		if (pickedVob && theApp.isBboxChangeMod != 0)
		{
			zCVob* bboxCurrentVob = NULL;

			if (theApp.isBboxChangeMod == 1)
			{
				bboxCurrentVob = theApp.bboxMaxsVob;

			}
			else
			{
				bboxCurrentVob = theApp.bboxMinsVob;
			}

			if (bboxCurrentVob)
			{
				zVEC3 startPos = bboxCurrentVob->GetPositionWorld();



				zlineCache->Line3D(startPos, startPos + zVEC3(100, 0, 0), GFX_GREEN, TRUE);
				zlineCache->Line3D(startPos, startPos + zVEC3(0, 100, 0), GFX_YELLOW, TRUE);
				zlineCache->Line3D(startPos, startPos + zVEC3(0, 0, 100), GFX_BLUE, TRUE);
			}
		}

		if (pickedVob && !zinput->GetMouseButtonPressedRight() && theApp.isBboxChangeMod != 0)
		{

			auto box = pickedVob->GetBBox3DWorld();
			zCVob* bboxCurrentVob = NULL;

			if (theApp.isBboxChangeMod == 1)
			{
				bboxCurrentVob = theApp.bboxMaxsVob;

			}
			else
			{
				bboxCurrentVob = theApp.bboxMinsVob;
			}

			if (!bboxCurrentVob) return;




			float speedTranslation = ((float)(theApp.options.GetIntVal("vobTransSpeed")) / 100000);
			float rotSpeed = ((float)(theApp.options.GetIntVal("vobRotSpeed")) / 1000);

			//print.PrintRed(zSTRING(speedTranslation, 10));
			speedTranslation *= 7;
			float length = 0;

			if (keys.KeyPressed("VOB_SPEED_X10", false, true))
			{
				speedTranslation *= 10;
				rotSpeed *= 3;
			}


			if (keys.KeyPressed("VOB_SPEED_MINUS_10", false, true))
			{
				speedTranslation /= 10;
				rotSpeed /= 20;
			}

			

			// general manipulations, regardless of the selected tool
			if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
			{
				zVEC3 pos = bboxCurrentVob->GetPositionWorld();

				zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				int y = pos.n[1];


				auto scaleValue = ztimer->frameTimeFloat / 1000.0f;

				// BBOX SCALE MOD
				if (zKeyPressed(KEY_3)) {

					box.Scale(1.00f - scaleValue);

					theApp.bboxMinsVob->SetPositionWorld(box.mins);
					theApp.bboxMaxsVob->SetPositionWorld(box.maxs);
					pickedVob->SetBBox3DWorld(box);
					
					
					return;
				}

				if (zKeyPressed(KEY_4)) {

					box.Scale(1.00F + scaleValue);

					theApp.bboxMinsVob->SetPositionWorld(box.mins);
					theApp.bboxMaxsVob->SetPositionWorld(box.maxs);
					pickedVob->SetBBox3DWorld(box);
					return;
				}

				y = pos.n[1];
				zVEC3 unit = camUnit;
				unit[1] = 0;


				if (keys.KeyPressed("VOB_TRANS_UP", false, true))
				{


					pos.n[1] += speedTranslation * ztimer->frameTimeFloat;
					HandleVobTranslation(bboxCurrentVob, pos);


				}

				if (keys.KeyPressed("VOB_TRANS_DOWN", false, true))
				{

					pos.n[1] -= speedTranslation * ztimer->frameTimeFloat;
					HandleVobTranslation(bboxCurrentVob, pos);


				}

				if (keys.KeyPressed("VOB_TRANS_FORWARD", false, true))
				{
					pos += unit * speedTranslation * ztimer->frameTimeFloat;
					//pos.n[1] = y;
					HandleVobTranslation(bboxCurrentVob, pos);
				}

				if (keys.KeyPressed("VOB_TRANS_BACKWARD", false, true))
				{
					pos -= unit * speedTranslation * ztimer->frameTimeFloat;
					//pos.n[1] = y;
					HandleVobTranslation(bboxCurrentVob, pos);
				}

				if (keys.KeyPressed("VOB_TRANS_LEFT", false, true))
				{
					zVEC3 left = camUnit.Cross(zVEC3(0, 1, 0));
					pos += left * speedTranslation * ztimer->frameTimeFloat;
					//pos.n[1] = y;
					HandleVobTranslation(bboxCurrentVob, pos);
				}

				if (keys.KeyPressed("VOB_TRANS_RIGHT", false, true))
				{
					zVEC3 right = camUnit.Cross(zVEC3(0, -1, 0));
					pos += right * speedTranslation * ztimer->frameTimeFloat;
					//pos.n[1] = y;
					HandleVobTranslation(bboxCurrentVob, pos);
				}

				if (keys.KeyPressed("VOB_NEAR_CAM", true))
				{
					if (theApp.pickedVob)
					{
						if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
						{
							HandleVobTranslation(bboxCurrentVob, ogame->GetCamera()->connectedVob->GetPositionWorld() +

								ogame->GetCamera()->connectedVob->GetAtVectorWorld() * 200);

							print.PrintRed(GetLang("VOB_NEAR_CAMERA"));
						}
					}


				}



				zTBBox3D bbox;
				bbox.mins = theApp.bboxMinsVob->GetPositionWorld();
				bbox.maxs = theApp.bboxMaxsVob->GetPositionWorld();

				for (int d = 0; d < 3; d++)
				{
					if (bbox.mins[d]>bbox.maxs[d])
					{
						zREAL tmp = bbox.mins[d];
						bbox.mins[d] = bbox.maxs[d];
						bbox.maxs[d] = tmp;
					}
				}

				
				pickedVob->SetDrawBBox3D(FALSE);
				pickedVob->SetBBox3DWorld(bbox);
				pickedVob->SetDrawBBox3D(TRUE);

				HandleVobTranslation(pickedVob, bbox.GetCenter());



				pickedVob->BeginMovement();
				pickedVob->SetPositionWorld(pickedVob->GetPositionWorld());

#if ENGINE >= Engine_G2A
				pickedVob->EndMovement(FALSE);
#else
				pickedVob->EndMovement();
#endif
			}

		}
	}
}