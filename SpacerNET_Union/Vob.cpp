// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void AddVobToRender(zSTRING visual, bool isItem = false)
	{
		if (!ogame || !ogame->GetCamera() || !ogame->GetCamera()->connectedVob)
		{
			return;
		}

		//print.PrintRed(visual + " " + Z isItem);

		zCVob* camVob = ogame->GetCamera()->connectedVob;

		if (theApp.currentVobRender)
		{
			ogame->GetWorld()->RemoveVob(theApp.currentVobRender);
			zRELEASE(theApp.currentVobRender);
			//print.PrintRed("—ущ. воб удален!");
		}

		if (visual.Length() == 0)
		{
			theApp.currentVobRender = NULL;
			theApp.currenItemRender = NULL;
			return;
		}


		if (!isItem)
		{
			CString visualFileCheckName = visual;

			if (visualFileCheckName.EndWith(".ASC"))
			{
				visualFileCheckName = visualFileCheckName.Replace(".ASC", ".MDL");
			}
			else if (visualFileCheckName.EndWith(".MDS"))
			{
				visualFileCheckName = visualFileCheckName.Replace(".MDS", ".MSB");
			}
			else if (visualFileCheckName.EndWith(".MMS"))
			{
				visualFileCheckName = visualFileCheckName.Replace(".MMS", ".MMB");
			}
			else
			{
				visualFileCheckName = visualFileCheckName.Replace(".3DS", ".MRM");
			}

			//prevent spacer crash if no file exists
			if (!Union_FileExists(visualFileCheckName))
			{
				return;
			}
		}
		

		zCVob *pVob = NULL;

		if (isItem)
		{
			zSTRING itemName = zSTRING(visual).Upper();

			int index = parser->GetIndex(itemName);

			theApp.currenItemRender = dynamic_cast<oCItem*>((zCVob*)ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, index));
			pVob = theApp.currenItemRender;
		}
		else
		{
			theApp.currenItemRender = NULL;
			pVob = zNEW(zCVob);
			pVob->SetVisual(visual);
			pVob->SetShowVisual(TRUE);
			pVob->SetVobName(ToStr "VobPreview_" + visual);

			pVob->SetCollDetStat(FALSE);
			pVob->SetCollDetDyn(FALSE);
		}


		if (pVob)
		{
			pVob->SetSleeping(FALSE);
			pVob->SetDrawBBox3D(FALSE);
			pVob->dontWriteIntoArchive = true;

			pVob->ignoredByTraceRay = TRUE;
			theApp.nextInsertBlocked = true;
			ogame->GetWorld()->AddVob(pVob);

			//pVob->CalcLightSampleAtOrigin();
		}
		else
		{
			print.PrintRed("Add to Render fail: " + A visual);

			theApp.nextInsertBlocked = false;
			return;
		}



		if (!pVob->GetVisual())
		{
			ogame->GetWorld()->RemoveVob(pVob);
			pVob->Release();
			theApp.currentVobRender = NULL;
			theApp.nextInsertBlocked = false;
			return;
		}

		zTBBox3D bbox = pVob->GetVisual()->GetBBox3D();
		float length = (bbox.maxs - bbox.mins).Length();
		float vobSizeMult = SafeDiv(1.0f, length) * 80.0f;

		zMAT4& trafo = pVob->GetNewTrafoObjToWorld();

		//print.PrintRed(zSTRING(vobSizeMult));

		if (!theApp.currenItemRender)
		{
			trafo.MakeIdentity();
			trafo.PreScale(vobSizeMult);
		}




		//pVob->Release();

		if (visual.Search("SWORD", 1) != -1)
		{
			//pVob->RotateLocalX(90);
		}

		theApp.currentVobRender = pVob;

		theApp.nextInsertBlocked = false;

	}


	extern bool GetFloorPositionForVobHelper(zCVob* vob, zVEC3& centerPos, zCPolygon*& polyIntersect, bool& foundVob);

	void HideFloorVob()
	{
		zCVob* floorVob = theApp.floorVob;

		if (floorVob)
		{
			floorVob->showVisual = 0;
			floorVob->SetVisual(0);
			floorVob->SetPositionWorld(zVEC3(0, 0, 0));
		}
		
	}

	void RenderVobsLoop()
	{

		if (!ogame->GetCamera() || !ogame->GetWorld())
		{
			return;
		}

		zCVob* camVob = ogame->GetCamera()->connectedVob;

		zCVob* floorVob = theApp.floorVob;

		if (theApp.treeIsReady)
		{
			if (theApp.options.GetIntVal("checkBoxShowVobTraceFloor"))
			{
				if (!floorVob)
				{
					floorVob = new zCVob();
					floorVob->SetVobName("SPACER_VOB_HELPER_FLOOR");

					floorVob->SetCollDet(FALSE);
					floorVob->dontWriteIntoArchive = TRUE;
					floorVob->ignoredByTraceRay = TRUE;
					floorVob->SetPhysicsEnabled(FALSE);
					floorVob->SetVisual("SPACER_CIRCLE_ITEM.TGA");





					zCDecal* dec = ((zCDecal*)floorVob->GetVisual());

					if (dec)
					{
						dec->decal2Sided = true;
						dec->SetDecalDim(100, 100);
						dec->decalMaterial->rndAlphaBlendFunc = zRND_ALPHA_FUNC_ADD;
					}

					floorVob->RotateLocalX(90);
					floorVob->showVisual = 0;

					theApp.nextInsertBlocked = true;
					ogame->GetWorld()->AddVob(floorVob);

					theApp.floorVob = floorVob;
					theApp.nextInsertBlocked = false;
				}

				// if a vob is selected
				if (auto pVob = theApp.GetSelectedVob())
				{

					if (!theApp.floorVob->GetVisual() || !theApp.floorVob->GetVisual()->objectName.contains(".TGA"))
					{
						floorVob->SetVisual("SPACER_CIRCLE_ITEM.TGA");

						zCDecal* dec = ((zCDecal*)floorVob->GetVisual());

						if (dec)
						{
							dec->decal2Sided = true;
							dec->SetDecalDim(100, 100);
							dec->decalMaterial->rndAlphaBlendFunc = zRND_ALPHA_FUNC_ADD;
						}

						floorVob->RotateLocalX(90);

					}
					zVEC3 floorPos = pVob->GetPositionWorld();
					zCPolygon* polyIntersect = NULL;
					bool foundVob = false;

					if (GetFloorPositionForVobHelper(pVob, floorPos, polyIntersect, foundVob))
					{

						if (!foundVob)
						{
							floorVob->SetPositionWorld(floorPos + zVEC3(0, 5, 0));
						}
						else
						{
							floorVob->SetPositionWorld(floorPos);
						}


						floorVob->showVisual = 1;




						zCDecal* dec = ((zCDecal*)floorVob->GetVisual());

						if (dec)
						{

							zREAL diff = pVob->bbox3D.GetMaxExtent() / 1.75f;


							if (diff == 0)
							{
								diff = 35;
							}
							//cmd << "diff: " << diff << endl;
							dec->SetDecalDim(diff, diff);

							if (polyIntersect)
							{
								//cmd << "poly: " << diff << endl;
								floorVob->SetHeadingAtWorld(polyIntersect->GetNormal());
							}
							else
							{
								floorVob->SetHeadingAtWorld(zVEC3(0, 1, 0));
							}

						}

						//floorVob->RotateWorldY(ztimer->frameTimeFloat / 8);
					}
					else
					{
						HideFloorVob();
					}
				}
				else
				{
					HideFloorVob();
				}
			}
			else
			{
				HideFloorVob();
			}

			
		}

		



		if (theApp.currentVobRender)
		{
			theApp.currentVobRender->SetPositionWorld(camVob->GetPositionWorld()
				- camVob->GetAtVectorWorld().Cross(zVEC3(0, -1, 0) * 150)
				+ camVob->GetAtVectorWorld() * 200
				+ zVEC3(0, 25, 0)
			);


			float speedRot = (float)theApp.options.GetIntVal("previewSpeed") / 70 * ztimer->frameTimeFloat;

			if (theApp.currentVobRender->GetVobName().Search(".TGA", 1) == -1)
			{
				theApp.currentVobRender->RotateLocalY(speedRot);
				//currentVob->RotateLocalZ(0.04 * ztimer->frameTimeFloat);
			}
			else
			{
				theApp.currentVobRender->SetHeadingWorld(camVob);
				theApp.currentVobRender->RotateLocalY(180);
				theApp.currentVobRender->ResetXZRotationsWorld();
			}

		}
	}
}