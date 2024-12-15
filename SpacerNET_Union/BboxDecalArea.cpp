// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	//set decal for bbox position and direction
	void SpacerApp::SetBBoxDecalPosAndSize(zCVob* sides[6], zVEC3 points[8], int index, int a, int b, int c, int d)
	{
		sides[index]->SetPositionWorld((points[a] + points[b] + points[c] + points[d]) / 4);

		zCDecal* dec = ((zCDecal*)sides[index]->GetVisual());

		if (dec)
		{
			dec->SetDecalDim(abs((points[a] - points[b]).Length() / 2), abs((points[a] - points[c]).Length() / 2));

		}

		auto dir = (points[a] - points[b]).Normalize();

		sides[index]->ResetRotationsWorld();
		sides[index]->SetHeadingAtWorld(dir);
		sides[index]->RotateWorldY(90.0f);
		sides[index]->SetShowVisual(TRUE);
	}



	void SpacerApp::ClearBboxDecal()
	{

		if (!sidesInit)
		{
			return;
		}

		if (!options.GetIntVal("bShowBboxModel"))
		{
			return;
		}


		//cmd << "ClearBboxDecal" << endl;

		for (int i = 0; i < 6; i++)
		{
			if (auto pVob = sides[i])
			{

				pVob->SetPositionWorld(zVEC3(0, 0, 0));

				pVob->SetShowVisual(FALSE);

				//cmd << "ClearBboxDecal: " << GetVobNameSafe(pVob) << endl;

				zCDecal* dec = ((zCDecal*)pVob->GetVisual());

				if (dec)
				{
					dec->SetDecalDim(10, 10);
				}
			}
		}
	}

	void SpacerApp::ClearBboxDecalReset()
	{
		if (!options.GetIntVal("bShowBboxModel"))
		{
			return;
		}

		if (!sidesInit)
		{
			return;
		}

		//cmd << "ClearBboxDecalReset" << endl;

		for (int i = 0; i < 6; i++)
		{
			if (auto pVob = sides[i])
			{
				pVob->SetShowVisual(FALSE);
				pVob->RemoveVobFromWorld();
				zRELEASE(pVob);
			}
		}

		if (decalMat)
		{
			zRELEASE(decalMat);
		}

	}

	void SpacerApp::BBoxDecal_OnLevelLoaded()
	{
		if (!options.GetIntVal("bShowBboxModel"))
		{
			return;
		}

		//cmd << "BBoxDecal_OnLevelLoaded" << endl;

		zSTRING textureName = "RED.TGA";

		// dx11 has its own texture due to no supporting vob material transparency (alpha)
		if (IsDx11Active())
		{
			textureName = "SPACER_RED_DX11.TGA";
		}

		decalMat = new zCMaterial();
		decalMat->SetName("MAT_SPACER_HELPER_RED");
		decalMat->SetTexture(textureName);
		decalMat->rndAlphaBlendFunc = zRND_ALPHA_FUNC_ADD;
		decalMat->color.alpha = 50;
		decalMat->AddRef();
		//decalMat->SetUsage(zCMaterial::zMAT_USAGE_LEVEL);
		

		for (int i = 0; i < 6; i++)
		{
			auto side = new zCVob();
			side->SetVisual(textureName);
			side->SetVobName("HELPER_SIDE_" + Z i);

			if (auto decal = side->visual->CastTo<zCDecal>())
			{
				decal->SetDecalDim(0, 0);
				decal->decal2Sided = true;
				decal->decalMaterial = decalMat;

			}

			side->SetShowVisual(TRUE);
			side->dontWriteIntoArchive = true;
			side->SetSleeping(FALSE);
			side->SetDrawBBox3D(FALSE);
			theApp.nextInsertBlocked = true;
			ogame->GetWorld()->AddVob(side);
			side->SetCollDet(FALSE);
			side->ignoredByTraceRay = true;
#if ENGINE >= Engine_G2
			side->m_fVobFarClipZScale = 2.0f;
#endif
			sides[i] = side;
		}

		sidesInit = TRUE;
	}


	void SpacerApp::BBoxDecal_Render()
	{
		if (!sidesInit)
		{
			return;
		}

		if (pickedVob)
		{
			if (pickedVob->visual)
			{
				return;
			}

			if (!options.GetIntVal("bShowBboxModel"))
			{
				return;
			}

			auto pZone = pickedVob->CastTo<zCZone>();
			auto pTrigger = pickedVob->CastTo<zCTriggerBase>();
			auto pDamage = pickedVob->CastTo<zCTouchDamage>();
			auto pTouchAni = pickedVob->CastTo<zCTouchAnimate>();

			auto pFog = pickedVob->CastTo<zCZoneZFog>();

			if (pFog)
			{
				return;
			}

			if (pTrigger || pZone || pDamage || pTouchAni)
			{
				//check bbox size
				if (pickedVob->bbox3D.maxs.Distance(pickedVob->bbox3D.mins) >= 10)
				{

					pickedVob->bbox3D.GetCornerPoints(points);

					for (int i = 0; i < 8; i++)
					{
						//debug.AddLine(points[i], points[i] + zVEC3(0, 100, 0), GFX_GREEN, 20, false, Z i, Z i);
					}

					SetBBoxDecalPosAndSize(sides, points, 0, 0, 5, 3, 7);
					SetBBoxDecalPosAndSize(sides, points, 1, 2, 6, 4, 1);
					SetBBoxDecalPosAndSize(sides, points, 2, 0, 2, 3, 4);
					SetBBoxDecalPosAndSize(sides, points, 3, 5, 6, 7, 1);
				}
			}


		}
	}

}