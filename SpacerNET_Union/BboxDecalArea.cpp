// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	enum BBoxDecalSide
	{
		BBOX_SIDE_LEFT = 0,
		BBOX_SIDE_RIGHT,
		BBOX_SIDE_FRONT,
		BBOX_SIDE_BACK,
		BBOX_SIDE_BOTTOM,
		BBOX_SIDE_TOP
	};


	void SpacerApp::SetBBoxDecalAABB(
		zCVob* side,
		const zTBBox3D& bbox,
		int sideType
	)
	{
		if (!side)
			return;

		auto visual = side->GetVisual();

		if (!visual)
		{
			side->SetShowVisual(FALSE);
			return;
		}

		auto decal = visual->CastTo<zCDecal>();

		if (!decal)
		{
			side->SetShowVisual(FALSE);
			return;
		}

		const zVEC3& mins = bbox.mins;
		const zVEC3& maxs = bbox.maxs;

		const float sizeX = maxs[VX] - mins[VX];
		const float sizeY = maxs[VY] - mins[VY];
		const float sizeZ = maxs[VZ] - mins[VZ];

		const float centerX = (mins[VX] + maxs[VX]) * 0.5f;
		const float centerY = (mins[VY] + maxs[VY]) * 0.5f;
		const float centerZ = (mins[VZ] + maxs[VZ]) * 0.5f;

		side->ResetRotationsWorld();

		switch (sideType)
		{
			// YZ
		case BBOX_SIDE_LEFT:
		{
			side->SetPositionWorld(
				zVEC3(mins[VX], centerY, centerZ)
			);

			decal->SetDecalDim(
				sizeZ * 0.5f,
				sizeY * 0.5f
			);

			// decal -> YZ plane
			side->RotateWorldY(90.0f);

			break;
		}

		// YZ
		case BBOX_SIDE_RIGHT:
		{
			side->SetPositionWorld(
				zVEC3(maxs[VX], centerY, centerZ)
			);

			decal->SetDecalDim(
				sizeZ * 0.5f,
				sizeY * 0.5f
			);

			side->RotateWorldY(90.0f);

			break;
		}

		// XY
		case BBOX_SIDE_FRONT:
		{
			side->SetPositionWorld(
				zVEC3(centerX, centerY, mins[VZ])
			);

			decal->SetDecalDim(
				sizeX * 0.5f,
				sizeY * 0.5f
			);

			// default decal plane
			break;
		}

		// XY
		case BBOX_SIDE_BACK:
		{
			side->SetPositionWorld(
				zVEC3(centerX, centerY, maxs[VZ])
			);

			decal->SetDecalDim(
				sizeX * 0.5f,
				sizeY * 0.5f
			);

			// decal2Sided == true,
			// so no 180 rotation
			break;
		}

		// XZ
		case BBOX_SIDE_BOTTOM:
		{
			side->SetPositionWorld(
				zVEC3(centerX, mins[VY], centerZ)
			);

			decal->SetDecalDim(
				sizeX * 0.5f,
				sizeZ * 0.5f
			);

			// XY -> XZ
			side->RotateWorldX(90.0f);

			break;
		}

		// XZ
		case BBOX_SIDE_TOP:
		{
			side->SetPositionWorld(
				zVEC3(centerX, maxs[VY], centerZ)
			);

			decal->SetDecalDim(
				sizeX * 0.5f,
				sizeZ * 0.5f
			);

			side->RotateWorldX(90.0f);

			break;
		}
		}

		side->SetShowVisual(TRUE);
	}


	void SpacerApp::ClearBboxDecal()
	{

		if (!sidesInit)
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
				if (pVob->visual)
				{
					zCDecal* dec = ((zCDecal*)pVob->GetVisual()->CastTo<zCDecal>());

					if (dec)
					{
						dec->SetDecalDim(10, 10);
					}
				}
				
			}
		}
	}

	void SpacerApp::ClearBboxDecalReset()
	{

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

		if (sidesInit)
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
		decalMat->color.alpha = 115;
		decalMat->AddRef();
		//decalMat->SetUsage(zCMaterial::zMAT_USAGE_LEVEL);
		

		for (int i = 0; i < 6; i++)
		{
			auto side = new zCVob();
			side->SetVisual(textureName);
			side->SetVobName("HELPER_SIDE_" + Z i);


			if (side->visual)
			{
				if (auto decal = side->visual->CastTo<zCDecal>())
				{
					decal->SetDecalDim(0, 0);
					decal->decal2Sided = true;
					decal->decalMaterial = decalMat;
				}
			}
			

			side->SetShowVisual(FALSE);
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
			return;

		if (!pickedVob)
			return;

		if (pickedVob->visual)
			return;

		if (!options.GetIntVal("bShowBboxModel"))
			return;

		auto pZone = pickedVob->CastTo<zCZone>();
		auto pTrigger = pickedVob->CastTo<zCTriggerBase>();
		auto pDamage = pickedVob->CastTo<zCTouchDamage>();
		auto pTouchAni = pickedVob->CastTo<zCTouchAnimate>();

		auto pFog = pickedVob->CastTo<zCZoneZFog>();

		if (pFog)
			return;

		if (!(pTrigger || pZone || pDamage || pTouchAni))
			return;

		const zTBBox3D& bbox = pickedVob->bbox3D;

		if (bbox.maxs.Distance(bbox.mins) < 5.0f)
			return;

		SetBBoxDecalAABB(sides[0], bbox, BBOX_SIDE_LEFT);
		SetBBoxDecalAABB(sides[1], bbox, BBOX_SIDE_RIGHT);
		SetBBoxDecalAABB(sides[2], bbox, BBOX_SIDE_FRONT);
		SetBBoxDecalAABB(sides[3], bbox, BBOX_SIDE_BACK);
		SetBBoxDecalAABB(sides[4], bbox, BBOX_SIDE_BOTTOM);
		SetBBoxDecalAABB(sides[5], bbox, BBOX_SIDE_TOP);
	}

}