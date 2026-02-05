// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void SpacerApp::VobTypeHighLight_Visual_Loop()
	{
		zCWorld* world = ogame->GetWorld();
		zCVob* pickedVob = GetSelectedVob();
		zCVob* camVob = ogame->GetCamera()->connectedVob;
		zCCamera* cam = ogame->GetCamera();
		zVEC3 camPos = camVob->GetPositionWorld();

		static auto GetSearchRadius = (voidFuncPointer)GetProcAddress(theApp.module, "GetSearchRadius");

		if (vobListVisualStr.Length() == 0)
		{
			return;
		}

		// radius is x2
		int radius = GetSearchRadius() * 2;

		zTBBox3D box;

		box.maxs = camPos + zVEC3(radius, radius, radius);
		box.mins = camPos - zVEC3(radius, radius, radius);

		zCArray<zCVob*> resVobList;

		ogame->GetWorld()->CollectVobsInBBox3D(resVobList, box);


		std::string searchStrUpper = vobListVisualStr.ToChar();

		std::vector<zCVob*> finalList;

		finalList.reserve(resVobList.GetNumInList() / 2);

		std::unordered_map<zCVob*, std::string> vobVisualCache;
		vobVisualCache.reserve(resVobList.GetNumInList() / 2);

		for (int i = 0; i < resVobList.GetNumInList(); i++) 
		{
			auto pVob = resVobList.GetSafe(i);

			if (!pVob) continue;

			if (auto pVisual = pVob->GetVisual()) {
				std::string visualName = pVisual->GetVisualName().ToChar();
				// Приводим к верхнему регистру один раз при построении кэша
				//std::transform(visualName.begin(), visualName.end(), visualName.begin(), ::toupper);
				vobVisualCache[pVob] = visualName;
			}
		}

		vobVisualCache.erase(currentVobRender);
		vobVisualCache.erase(currenItemRender);
		vobVisualCache.erase(floorVob);
		vobVisualCache.erase(bboxMaxsVob);
		vobVisualCache.erase(bboxMinsVob);
		vobVisualCache.erase(pickedVob);
		vobVisualCache.erase(ogame->GetCameraVob());
		vobVisualCache.erase(pfxManager.testVob);
		vobVisualCache.erase(floorVob);
		vobVisualCache.erase(pfxEditorVob);
		vobVisualCache.erase(pLightDx11);

		for (auto& it : vobVisualCache)
		{
			if (it.second.find(searchStrUpper) != std::string::npos) 
			{
				finalList.push_back(it.first);
			}
		}


		for (auto& it : finalList)
		{

			zVEC3 vobPos = it->GetPositionWorld();
			zVEC3 toVob = vobPos - camPos;


			zVEC3 viewPos = cam->GetTransform(zTCamTrafoType::zCAM_TRAFO_VIEW) * vobPos;
			int x, y;


			cam->Project(&viewPos, x, y);

			if (viewPos[2] > cam->nearClipZ)
			{
				it->bbox3D.Draw(GFX_RED);
			}
		}

	}

	void SpacerApp::VobTypeHighLight_Loop()
	{
		if (vobListVisualActive)
		{
			VobTypeHighLight_Visual_Loop();
			return;
		}

		if (!theApp.options.GetIntVal("renderOnlySelectedVobType"))
		{
			return;
		}

		int type = theApp.vobListSelectedIndex;

		if (type == 0)
		{
			return;
		}

		zCWorld* world = ogame->GetWorld();
		zCVob* pickedVob = GetSelectedVob();
		zCVob* camVob = ogame->GetCamera()->connectedVob;
		zCCamera* cam = ogame->GetCamera();
		zVEC3 camPos = camVob->GetPositionWorld();

		static auto GetSearchRadius = (voidFuncPointer)GetProcAddress(theApp.module, "GetSearchRadius");

		// radius is x2
		int radius = GetSearchRadius() * 2;

		zTBBox3D box;

		box.maxs = camPos + zVEC3(radius, radius, radius);
		box.mins = camPos - zVEC3(radius, radius, radius);

		zCArray<zCVob*> resVobList;

		ogame->GetWorld()->CollectVobsInBBox3D(resVobList, box);

		std::vector<zCVob*> finalList;

		finalList.reserve(resVobList.GetNumInList() / 2);

		bool isFakeBbox = false;
		

		for (int i = 0; i < resVobList.GetNumInList(); i++)
		{
			auto pVob = resVobList.GetSafe(i);

			if (!pVob) continue;

			if (pVob == theApp.currentVobRender
				|| pVob == theApp.currenItemRender
				|| pVob == theApp.floorVob
				|| pVob == theApp.bboxMaxsVob
				|| pVob == theApp.bboxMinsVob
				|| IsSpacerVob(pVob)
				|| pVob == pickedVob
				)
			{
				continue;
			}

			// highlight by str visual
			if (vobListVisualActive)
			{
				cmd << "vobListVisualStr: " << vobListVisualStr << endl;

				if (auto pVisual = pVob->GetVisual())
				{
					if (pVisual->GetVisualName().Upper().Contains(vobListVisualStr))
					{
						finalList.push_back(pVob);
					}
				}

				continue;
			}

			// 
			if (type == 1 && pVob->_GetClassDef()->GetClassName_() == "zCVob")
			{
				if (pVob->GetVisual())
				{
					if (pVob->GetVisual()->GetVisualName().Upper().Search(".TGA", 1) == -1)
					{
						finalList.push_back(pVob);
					}
				}
				else
				{
					finalList.push_back(pVob);
				}
			}
			else if (type == 2 && dynamic_cast<oCItem*>(pVob))
			{
				finalList.push_back(pVob);
			}
			else if (type == 3 && dynamic_cast<zCVobSpot*>(pVob))
			{
				finalList.push_back(pVob);

				isFakeBbox = true;
			}
			else if (type == 4 && dynamic_cast<zCVobWaypoint*>(pVob))
			{
				finalList.push_back(pVob);
				isFakeBbox = true;
			}
			else if (type == 5 && dynamic_cast<zCVobSound*>(pVob))
			{
				finalList.push_back(pVob);
			}
			else if (type == 6 && pVob->GetVisual())
			{
				if (pVob->GetVisual()->GetVisualName().Upper().Search(".TGA", 1) != -1)
				{
					finalList.push_back(pVob);
				}

			}
			else if (type == 7 && dynamic_cast<zCTrigger*>(pVob))
			{
				finalList.push_back(pVob);
			}
			else if (type == 8 && dynamic_cast<oCMOB*>(pVob))
			{
				finalList.push_back(pVob);
			}
			else if (type == 9 && pVob && !pVob->GetShowVisual() && pVob->GetVisual())
			{
				finalList.push_back(pVob);
			}
			// PFX
			else if (type == 11 && pVob && pVob->GetVisual() && (pVob->GetVisual()->GetVisualName().EndWith(".pfx") || pVob->GetVisual()->GetVisualName().EndWith(".PFX")))
			{
				finalList.push_back(pVob);

				//cmd << "GetVobName: " << GetVobName(resVobList.GetSafe(i)) << endl;
			}
			// zones
			else if (type == 12 && pVob && pVob->CastTo<zCZone>())
			{
				finalList.push_back(pVob);

				//cmd << "GetVobName: " << GetVobName(resVobList.GetSafe(i)) << endl;
			}
			else if (type == 13 && pVob && (pVob->CastTo<oCZoneMusic>() && !pVob->CastTo<oCZoneMusicDefault>()))
			{
				finalList.push_back(pVob);

				//cmd << "GetVobName: " << GetVobName(resVobList.GetSafe(i)) << endl;
			}
		}

		//zCVobLight
		if (!vobListVisualActive && (type == 0 || type == 10))
		{
			auto pos = camVob->GetPositionWorld();
			zCArray<zCVob*> lightList;

			ogame->GetWorld()->SearchVobListByClass(zCVobLight::classDef, lightList, 0);

			for (int i = 0; i < lightList.GetNumInList(); i++)
			{
				if (auto pLight = lightList.GetSafe(i))
				{
					if (pLight->GetPositionWorld().Distance(pos) <= radius)
					{
						finalList.push_back(pLight);
					}
				}
			}
		}

		//print.PrintRed(Z (int) finalList.size());
		
		


		for (auto& it : finalList)
		{

			zVEC3 vobPos = it->GetPositionWorld();
			zVEC3 toVob = vobPos - camPos;


			zVEC3 viewPos = cam->GetTransform(zTCamTrafoType::zCAM_TRAFO_VIEW) * vobPos;
			int x, y;


			cam->Project(&viewPos, x, y);

			if (viewPos[2] > cam->nearClipZ) 
			{
				if (isFakeBbox)
				{

					zTBBox3D bboxFake;

					bboxFake.Init();
					bboxFake.AddPoint(zVEC3(-50, 0, -50) + vobPos);
					bboxFake.AddPoint(zVEC3(50, 215, 50) + vobPos);
					bboxFake.Draw(GFX_GREEN);
				}
				else
				{
					it->bbox3D.Draw(GFX_RED);
				}
				
			}
		}
	}
}