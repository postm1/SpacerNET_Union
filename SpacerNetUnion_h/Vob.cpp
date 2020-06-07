// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void ScaleVob(zCVob* pVob, float scale)
	{

		if (pVob && pVob->GetVisual())
		{

			zTBBox3D bbox = pVob->GetVisual()->GetBBox3D();
			float length = (bbox.maxs - bbox.mins).Length();
			//float vobSizeMult = SafeDiv(1.0f, length) * 80.0f;
			//print.PrintRed(A vobSizeMult);

			zMAT4& trafo = pVob->trafoObjToWorld;
			zVEC3 scaleVec = zVEC3(0.5f, 0.5f, 0.5f);

			//trafo.MakeIdentity();
			trafo.PreScale(scaleVec);
			//	trafo = Alg_Scaling3D(scaleVec) * trafo;
			//trafo.MakeOrthonormal();
			pVob->bbox3D.Scale(scaleVec);
		}
	}




	void AddVobToRender(zSTRING visual, bool isItem = false)
	{
		if (!ogame || !ogame->GetCamera() || !ogame->GetCamera()->connectedVob)
		{
			return;
		}

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
			return;
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

			pVob->CalcLightSampleAtOrigin();
		}
		else
		{
			print.PrintRed("Add to Render fail: " + A visual);
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

	void RenderVobsLoop()
	{

		if (!ogame->GetCamera() || !ogame->GetWorld())
		{
			return;
		}

		zCVob* camVob = ogame->GetCamera()->connectedVob;



		if (theApp.currentVobRender)
		{
			theApp.currentVobRender->SetPositionWorld(camVob->GetPositionWorld()
				- camVob->GetAtVectorWorld().Cross(zVEC3(0, -1, 0) * 150)
				+ camVob->GetAtVectorWorld() * 200
				+ zVEC3(0, 25, 0)
			);


			if (theApp.currentVobRender->GetVobName().Search(".TGA", 1) == -1)
			{
				theApp.currentVobRender->RotateLocalY(0.1 * ztimer->frameTimeFloat);
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