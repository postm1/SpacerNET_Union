// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	void BoxMoving()
	{
		zCVob* pickedVob = theApp.GetSelectedVob();

		



		if (keys.KeyPressed("TOOL_BBOX_CHANGE", true) && pickedVob)
		{

			if (GetSelectedTool() != 3)
			{
				print.PrintRed(GetLang("TOOL_BBOX_CHANGE"));
				SetSelectedTool(3);
				theApp.isBboxChangeMod = 1;

				if (theApp.bboxMaxsVob)
				{
					theApp.bboxMaxsVob->SetPositionWorld(pickedVob->GetBBox3DWorld().maxs);

				}

				if (theApp.bboxMinsVob)
				{
					theApp.bboxMinsVob->SetPositionWorld(pickedVob->GetBBox3DWorld().mins);

				}
			}
			else
			{
				print.PrintRed(GetLang("TOOL_BBOX_CHANGE_LEAVE"));
				SetSelectedTool(1);
				theApp.isBboxChangeMod = 0;
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

			zVEC3 startPos = bboxCurrentVob->GetPositionWorld();

			

			zlineCache->Line3D(startPos, startPos + zVEC3(100, 0, 0), GFX_GREEN, TRUE);
			zlineCache->Line3D(startPos, startPos + zVEC3(0, 100, 0), GFX_YELLOW, TRUE);
			zlineCache->Line3D(startPos, startPos + zVEC3(0, 0, 100), GFX_BLUE, TRUE);


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