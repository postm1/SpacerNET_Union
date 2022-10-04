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
				bboxMinsVob->dontWriteIntoArchive = true;
				bboxMinsVob->SetCollDet(FALSE);
				//bboxMinsVob->SetVisual("ITMI_STONEFIRE_ENCH.3DS");
				bboxMinsVob->ignoredByTraceRay = true;
				bboxMinsVob->SetShowVisual(FALSE);

				theApp.nextInsertBlocked = true;
				ogame->GetWorld()->AddVob(bboxMinsVob);
				theApp.nextInsertBlocked = false;
			}

			if (!bboxMaxsVob)
			{
				bboxMaxsVob = new zCVob();
				bboxMaxsVob->SetVobName("SPACER_VOB_BBOX_MAXS");
				bboxMaxsVob->dontWriteIntoArchive = true;
				bboxMaxsVob->SetCollDet(FALSE);
				//bboxMaxsVob->SetVisual("ITMI_STONEFIRE_ENCH.3DS");
				
				bboxMaxsVob->ignoredByTraceRay = true;

				bboxMaxsVob->SetShowVisual(FALSE);

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
				SetSelectedTool(1);
				theApp.isBboxChangeMod = 0;
				return;
			}

			if (GetSelectedTool() != 3)
			{
				
				SetSelectedTool(3);
				theApp.isBboxChangeMod = 1;

				theApp.PrepareBboxVobs(pickedVob);
			}
			else
			{
				print.PrintRed(GetLang("TOOL_BBOX_CHANGE_LEAVE"));
				SetSelectedTool(1);
				theApp.isBboxChangeMod = 0;
				theApp.PrepareBboxVobs(NULL);
			}
			
			
		}

		if (GetSelectedTool() != 3)
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



			// общие манипуляции, вне зависимости от выбранного инструмента
			if (ogame->GetCamera() && ogame->GetCamera()->connectedVob)
			{
				zVEC3 pos = bboxCurrentVob->GetPositionWorld();

				zVEC3 camUnit = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

				int y = pos.n[1];




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
				// Korrektur	
				for (int d = 0; d<3; d++)
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

				pickedVob->EndMovement(FALSE);
			}

		}
	}
}