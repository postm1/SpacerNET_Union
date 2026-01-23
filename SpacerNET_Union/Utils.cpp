// Supported with union (c) 2020 Union team
// Union SOURCE file



namespace GOTHIC_ENGINE {


	std::chrono::steady_clock::time_point begin_time[70];
	double perf[50];

	void RX_Begin(int index) {
		begin_time[index] = std::chrono::steady_clock::now();
	}

	void RX_End(int index) {
		perf[index] = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time[index]).count());
	}


	zSTRING RX_PerfString(int index) {
		return  zSTRING(perf[index] / 1000, 10) + zSTRING(" ms");
	}

	// Add your code here . . .
	//RX_FIX

#if ENGINE == Engine_G1
	int& playerLightInt = *(int*)0x0083AB90;
#elif ENGINE == Engine_G1A
	int& playerLightInt = *(int*)0x0088079C;
#elif ENGINE == Engine_G2
	int& playerLightInt = *(int*)0x00890970;
#elif ENGINE == Engine_G2A
	int& playerLightInt = *(int*)0x0089EBB4;
#endif

	
	void PatchSecuredMem(LPCVOID pDst, const void* pSrc, size_t Size, BYTE* originalBytes = nullptr)
	{
		BOOL bProtect;
		DWORD flOldProtect;
		_MEMORY_BASIC_INFORMATION Buffer;

		VirtualQuery(pDst, &Buffer, sizeof(Buffer));

		bProtect = VirtualProtect(Buffer.BaseAddress, Buffer.RegionSize, PAGE_READWRITE, &flOldProtect);

		if (bProtect)
		{
			// saving original data
			if (originalBytes != nullptr)
			{
				memcpy(originalBytes, pDst, Size);
			}

			memcpy((void*)pDst, pSrc, Size);

			VirtualProtect(Buffer.BaseAddress, Buffer.RegionSize, flOldProtect, &flOldProtect);
		}
	}

	void SetCollisionsFixForRayCast(bool toggle)
	{
		static BYTE patchRayCast[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		static BYTE original[6];
		static bool saveOriginalData = true;

#if ENGINE == Engine_G1
		static uint addrPatch = 0x005A6327;
#elif ENGINE == Engine_G1A
		static uint addrPatch = 0x005C2C21;
#else
		static uint addrPatch = 0x005C8237;
#endif
		if (toggle)
		{
			PatchSecuredMem((void*)addrPatch, patchRayCast, sizeof(patchRayCast), saveOriginalData ? original : NULL);
			saveOriginalData = false;
		}
		else
		{
			PatchSecuredMem((void*)addrPatch, original, sizeof(original), NULL);
		}

	}

	double GetAngleBetweenVobs(zCVob* vob, zCVob* target, zVEC3 realUnit)
	{
		zVEC3 targetRel = (target->GetPositionWorld() - vob->GetPositionWorld()).Normalize();

		double angle = acos((realUnit.n[0] * targetRel.n[0] + realUnit.n[2] * targetRel.n[2])
			/ (targetRel.Length() * realUnit.Length())) * 180 / PI;


		return angle;
	}

	float GetAngleBetweenVectors(zVEC3 a, zVEC3 b)
	{

		float angle = 2.0f * atan((a - b).Length() / (a + b).Length());

		angle = angle * 180 / PI;

		return angle;
	}

	void ClearLMB()
	{
#if ENGINE == Engine_G1
		*(int*)0x0086CCB8 = 0;
#elif ENGINE == Engine_G1A
		* (int*)0x008B27B4 = 0;
#elif ENGINE == Engine_G2
		* (int*)0x008C3010 = 0;
#elif ENGINE == Engine_G2A
		*(int*)0x008D1668 = 0;
#endif
	}

	void __cdecl PlaySoundGame(class zSTRING &)
	{
#if ENGINE == Engine_G1
		XCALL(0x00641350);
#elif ENGINE == Engine_G1A
		XCALL(0x006684F0);
#elif ENGINE == Engine_G2
		XCALL(0x0066F250);
#elif ENGINE == Engine_G2A
		XCALL(0x006CBFD0);
#endif

		
	}



	int GetFPS(void)
	{
#if ENGINE == Engine_G1
		XCALL(0x004EF790);
#elif ENGINE == Engine_G1A
		XCALL(0x00502030);
#elif ENGINE == Engine_G2
		XCALL(0x004FB050);
#elif ENGINE == Engine_G2A
		XCALL(0x004FDCD0);
#endif
		
	}


	void __cdecl sysEvent()
	{
#if ENGINE == Engine_G1
		XCALL(0x004F6AC0);
#elif ENGINE == Engine_G1A
		XCALL(0x00509530);
#elif ENGINE == Engine_G2
		XCALL(0x005026F0);
#elif ENGINE == Engine_G2A
		XCALL(0x005053E0);
#endif

	}

	void SetTime(int hour, int min)
	{
		int day, hour_, min_;

		ogame->GetTime(day, hour_, min_);
		ogame->SetTime(day, hour, min);
	}

	int printDebugCount = 0;

	void PrintDebug(zSTRING str) {

		screen->Print(F(1), F(1) + (F(3) * printDebugCount++), str);
	}

	void ResetPrintDebug()
	{
		printDebugCount = 0;
	}



	void OutFile(zSTRING str, bool close)
	{
		static std::ofstream fout;
		static bool enable = false;

		if (enable)
		{
			if (!fout.is_open())
			{
				fout.open("./system/_SpacerNetErrors.txt", std::ios_base::app);
			}

			fout << "Union: " << str.ToChar() << std::endl;

			if (close)
			{
				fout.close();
			}
		}

		std::cout << "Union: " << str.ToChar() << std::endl;

	}


	


	int childrenCountVob = 0;

	void GetChildrenCount(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		childrenCountVob++;
		node = node->GetFirstChild();
		while (node != NULL)
		{
			GetChildrenCount(node);
			node = node->GetNextChild();

		}
	}


	zSTRING GetVobNameSafe(zCVob* vob)
	{
		zSTRING result = "";

		if (vob)
		{
			if (!vob->GetVobName().IsEmpty())
			{
				result = vob->GetVobName();
			}
			else if (vob->GetVisual())
			{
				result = vob->GetVisual()->GetVisualName();
			}
			else
			{
				result = "vob1";
			}

			result = result + " (" + vob->_GetClassDef()->GetClassName_() + ")";
		}

		return result;
	}


	

	zSTRING GetVobClassName(zCVob* vob)
	{
		zSTRING str = "";

		if (vob && vob->_GetClassDef())
		{
			str = "(" + vob->_GetClassDef()->className + ")";
		}

		return str;
	}

	zSTRING GetCleanVobName(zCVob* vob)
	{
		zSTRING str = "";

		if (vob)
		{
			str = vob->GetVobName();
		}

		return str;
	}

	zSTRING GetVobName(zCVob* vob)
	{
		zSTRING str = "";

		if (vob)
		{
			
			if (dynamic_cast<zCVobLevelCompo*>(vob))
			{
				if (vob->GetVisual())
				{

					str = vob->GetVisual()->GetVisualName().ToChar();
				}
				else
				{
					str = vob->GetVobName().ToChar();
				}

				/*str = str + " [Children Count: ";

				childrenCountVob = 0;
				GetChildrenCount(vob->globalVobTreeNode);

				str = str + ToStr(childrenCountVob - 1);
				str = str + "]";
				*/
			}
			else
			
			{
				// generate the name of the item	
				if (!vob->GetVobName().IsEmpty())
				{
					str = vob->GetVobName();
				}
				else if (vob->GetVisual())
				{
					str = vob->GetVisual()->GetVisualName();
				}
				else
				{
					str = "vob1";
				}

				str = str + " (";
				str = str + vob->_GetClassDef()->GetClassName_();
				str = str + ")";
			}




		}

		return str;
	}

	// Draw a line with a specific color
	void DrawLine(zCCamera *cam, zCVob* v1, zCVob* v2, zCOLOR col)
	{
		if (!cam || !v1 || !v2) return;

		zVEC3 wsPoint1, wsPoint2, csPoint1, csPoint2;
		zVEC3 ssPoint1, ssPoint2;

		wsPoint1 = v1->GetPositionWorld();
		wsPoint2 = v2->GetPositionWorld();
		csPoint1 = cam->Transform(wsPoint1);
		csPoint2 = cam->Transform(wsPoint2);
		Alg_ClipAtZ0(csPoint1, csPoint2);

		if (csPoint1[VZ] >= 0)
		{
			cam->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
		}
		else return;

		if (csPoint2[VZ] >= 0)
		{
			cam->Project(&csPoint2, ssPoint2[VX], ssPoint2[VY]);
		}
		else return;

		float camSpaceInf1 = (csPoint1[VZ] != 0 ? 1 / csPoint1[VZ] : 0);
		float camSpaceInf2 = (csPoint1[VZ] != 0 ? 1 / csPoint2[VZ] : 0);

		int x1 = (int)ssPoint1[VX];
		int y1 = (int)ssPoint1[VY];
		int x2 = (int)ssPoint2[VX];
		int y2 = (int)ssPoint2[VY];
		if (!screen->ClipLine(x1, y1, x2, y2)) return;

		if (zrenderer) zrenderer->DrawLineZ((float)x1, (float)y1, camSpaceInf1, (float)x2, (float)y2, camSpaceInf2, col);

		if (csPoint1[VZ]<1000)
		{
			screen->Print(screen->anx(x1), screen->any(y1), v1->GetVobName());
		}
		if (csPoint2[VZ]<1000)
		{
			screen->Print(screen->anx(x2), screen->any(x2), v2->GetVobName());
		}
	}

	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	void CreateFileList()
	{
		char** fileList = nullptr;

		uint numInList = Vdfs32::vdf_filelist_virtual(fileList);


		for (uint i = 0; i < numInList; i++)
		{
			if (fileList[i])
			{
				theApp.modelsStrVDF.InsertEnd(zSTRING(fileList[i]));
			}
			
		}

		//cmd << "FileList: " << numInList << endl;

		if (fileList)
		{
			for (long i = 0; i < numInList; i++)
			{
				delete[] fileList[i];
			}

			delete[] fileList;
		}


		char** fileListWork = nullptr;

		numInList = Vdfs32::vdf_filelist_physical(fileListWork);

		//cmd << "fileListWork: " << numInList << endl;

		for (uint i = 0; i < numInList; i++)
		{
			if (fileListWork[i])
			{
				theApp.modelsStrWORK.InsertEnd(zSTRING(fileListWork[i]));
			}
		}

		if (fileListWork)
		{
			for (uint i = 0; i < numInList; i++)
			{
				delete[] fileListWork[i];
			}
			delete[] fileListWork;
		}

	}


	void SetBBoxValue(int x, int y, int z)
	{
		zCVob* vob = theApp.GetSelectedVob();

		if (vob)
		{
			vob->AddRef();

			zTBBox3D box;
			box = vob->GetBBox3DWorld();

			zVEC3 centerPt = vob->GetPositionWorld();

			box.maxs.n[VX] = centerPt.n[VX] + int(x / 2);
			box.mins.n[VX] = centerPt.n[VX] - int(x / 2);

			box.maxs.n[VY] = centerPt.n[VY] + int(y / 2);
			box.mins.n[VY] = centerPt.n[VY] - int(y / 2);

			box.maxs.n[VZ] = centerPt.n[VZ] + int(z / 2);
			box.mins.n[VZ] = centerPt.n[VZ] - int(z / 2);

			vob->SetBBox3DWorld(box);


			zRELEASE(vob);
		}
		theApp.SelectObject(theApp.pickedVob);
		//theApp.SetSelectedVob(NULL, "SetBBoxValue");
	}

	bool IsSpacerVob(zCVob* pVob)
	{
		if (theApp.bboxMaxsVob == pVob
			|| theApp.bboxMinsVob == pVob
			|| ogame->GetCameraVob() == pVob
			|| theApp.currenItemRender == pVob
			|| theApp.currentVobRender == pVob

			|| pfxManager.testVob == pVob
			|| pfxEditorVob == pVob
			|| theApp.floorVob == pVob
			|| theApp.mobInterSlotsVobs.IsInList(pVob)
			|| theApp.pLightDx11 == pVob
			//|| pVob->GetVobName() == "VOB_SPACER_CAMERA_START" // this vob can be selected, so remove it from here
			)
		{
			return true;
		}

		if (theApp.sidesInit)
		{
			for (int i = 0; i < 6; i++)
			{
				if (theApp.sides[i] == pVob)
				{
					return true;
				}
			}
		}

		return false;
	}

	float GetBboxValue(int coord)
	{
		zCVob* vob = theApp.GetSelectedVob();

		float result = 0;

		if (vob)
		{
			vob->AddRef();

			zVEC3 centerPt = vob->GetPositionWorld();
			zTBBox3D box;
			box = vob->GetBBox3DWorld();

			switch (coord)
			{
			case 0: result = (float)((int)(box.maxs.n[VX] - box.mins.n[VX])); break;
			case 1: result = (float)((int)(box.maxs.n[VY] - box.mins.n[VY])); break;
			case 2: result = (float)((int)(box.maxs.n[VZ] - box.mins.n[VZ])); break;
			}

			zRELEASE(vob);
		}

		return result;
	}

	int CheckUniqNameExistResult(CString name)
	{
		int result = 0;

		zSTRING vobName = zSTRING(name).Upper();

		if (theApp.pickedVob)
		{
			if (/*dynamic_cast<zCMover*>(theApp.pickedVob)*/
				dynamic_cast<zCVobSpot*>(theApp.pickedVob)
				|| dynamic_cast<zCVobWaypoint*>(theApp.pickedVob))
			{


				if (vobName.Length() == 0)
				{
					result = 2;
				}

				if (vobName.Length() > 0)
				{

					zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

					if (waynet)
					{
						zCWaypoint* found = waynet->GetWaypoint(vobName);

						if (found)
						{
							result = 1;
						}
					}

					zCArray<zCVob*> resultSearch;

					ogame->GetWorld()->SearchVobListByName(vobName, resultSearch);

					if (resultSearch.GetNumInList() > 0)
					{
						for (int i = 0; i < resultSearch.GetNumInList(); i++)
						{
							if (resultSearch.GetSafe(i) != theApp.pickedVob)
							{
								result = 1;
								break;
							}
						}
					}
				}
				
				
			}
		}
		else if (mm.matSelectedInTree)
		{
			zSTRING matName = vobName.Upper();

			zCClassDef* classDef = zCMaterial::classDef;

			zCMaterial*	mat = 0;
			int numOfMats = classDef->objectList.GetNum();


			for (int i = 0; i < numOfMats; i++)
			{
				mat = dynamic_cast<zCMaterial*>(classDef->objectList[i]);

				if (mat && mat->GetName() == matName && mm.matSelectedInTree != mat)
				{
					result = 1;
					break;
				}

			}
		}


		return result;
	}

	bool CheckVobNameExist(CString name, bool isWaypoint)
	{
		bool result = false;


		//std::cout << "CheckVobNameExist: looking for " << name << std::endl;


		zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

		if (waynet)
		{
			zCWaypoint* found = waynet->GetWaypoint(zSTRING(name).Upper());

			if (found)
			{
				//std::cout << "CheckVobNameExist: found waypoint " << name << std::endl;
				result = true;
			}
		}


		zCVob* vob = ogame->GetWorld()->SearchVobByName(zSTRING(name).Upper());

		if (vob)
		{
			//std::cout << "CheckVobNameExist: found vob " << name << std::endl;
			result = true;
		}

		return result;
	}


	void SetRangeVobs()
	{
		float drawDist = ((float)theApp.options.GetIntVal("rangeVobs") / 1000);

		ogame->GetWorld()->SetVobFarClipZScalability(drawDist);
	}

	void SetRangeWorld()
	{
		float drawDist = ((float)theApp.options.GetIntVal("rangeWorld") / 1000);

		if (zCSkyControler::s_activeSkyControler)
		{
			zCSkyControler::s_activeSkyControler->SetFarZ(drawDist * 10000);
			zCSkyControler::s_activeSkyControler->SetFarZScalability(drawDist);
		}

	}


	void FindNearestVobs()
	{
		zCWorld* world = ogame->GetWorld();
		zCVob* pickedVob = theApp.GetSelectedVob();


		zTBBox3D box;

		zCVob* camVob = ogame->GetCamera()->connectedVob;

		zVEC3 normCamera = camVob->GetAtVectorWorld().Normalize();

		int type = theApp.vobListSelectedIndex;
		zCArray<zCVob*>resVobList;
		zCArray<zCVob*>finalResultList;


		theApp.exports.toggleUIElement(UIElementType::UI_WIN_VOBLIST, FALSE);
		(callVoidFunc)GetProcAddress(theApp.module, "ClearVobList")();

		static auto GetSearchRadius = (voidFuncPointer)GetProcAddress(theApp.module, "GetSearchRadius");


		int radius = GetSearchRadius();

		box.maxs = camVob->GetPositionWorld() + zVEC3(radius, radius, radius);
		box.mins = camVob->GetPositionWorld() - zVEC3(radius, radius, radius);

		

		ogame->GetWorld()->CollectVobsInBBox3D(resVobList, box);


		
		//cmd << "SearchType: " << type << endl;

		for (int i = 0; i < resVobList.GetNumInList(); i++)
		{
			auto pVob = resVobList.GetSafe(i);

			if (pVob == theApp.currentVobRender 
				|| pVob == theApp.currenItemRender 
				|| pVob == theApp.floorVob 
				|| pVob == theApp.bboxMaxsVob
				|| pVob == theApp.bboxMinsVob
				|| IsSpacerVob(pVob)
				)
			{
				continue;
			}

			if (type == 0)
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 1 && resVobList.GetSafe(i)->_GetClassDef()->GetClassName_() == "zCVob")
			{
				if (resVobList.GetSafe(i)->GetVisual())
				{
					if (resVobList.GetSafe(i)->GetVisual()->GetVisualName().Upper().Search(".TGA", 1) == -1)
					{
						finalResultList.Insert(resVobList.GetSafe(i));
					}
				}
				else
				{
					finalResultList.Insert(resVobList.GetSafe(i));
				}
					
					
				
			}
			else if (type == 2 && dynamic_cast<oCItem*>(resVobList.GetSafe(i)))
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 3 && dynamic_cast<zCVobSpot*>(resVobList.GetSafe(i)))
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 4 && dynamic_cast<zCVobWaypoint*>(resVobList.GetSafe(i)))
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 5 && dynamic_cast<zCVobSound*>(resVobList.GetSafe(i)))
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 6 && resVobList.GetSafe(i)->GetVisual())
			{
				if (resVobList.GetSafe(i)->GetVisual()->GetVisualName().Upper().Search(".TGA", 1) != -1 )
				{
					finalResultList.Insert(resVobList.GetSafe(i));
				}
				
			}
			else if (type == 7 && dynamic_cast<zCTrigger*>(resVobList.GetSafe(i)))
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 8 && dynamic_cast<oCMOB*>(resVobList.GetSafe(i)))
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			else if (type == 9 && pVob && !pVob->GetShowVisual() && pVob->GetVisual())
			{
				finalResultList.Insert(resVobList.GetSafe(i));
			}
			// PFX
			else if (type == 11 && pVob && pVob->GetVisual() && (pVob->GetVisual()->GetVisualName().EndWith(".pfx") || pVob->GetVisual()->GetVisualName().EndWith(".PFX")))
			{
				finalResultList.Insert(resVobList.GetSafe(i));

				//cmd << "GetVobName: " << GetVobName(resVobList.GetSafe(i)) << endl;
			}
			// zones
			else if (type == 12 && pVob && pVob->CastTo<zCZone>())
			{
				finalResultList.Insert(resVobList.GetSafe(i));

				//cmd << "GetVobName: " << GetVobName(resVobList.GetSafe(i)) << endl;
			}
		}

		//zCVobLight
		if (type == 0 || type == 10)
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
						finalResultList.Insert(pLight);
					}
				}
			}
		}


		//ogame->GetWorld()->SearchVobListByBaseClass(lightClassdef, resVobList, 0);

		// sorting by names
		std::vector<std::pair<std::string, zCVob*>> vobsWithNames;
		vobsWithNames.reserve(finalResultList.GetNumInList());

		for (int i = 0; i < finalResultList.GetNumInList(); i++)
		{
			if (auto pVob = finalResultList.GetSafe(i))
			{
				zSTRING name = GetVobName(pVob);
				vobsWithNames.emplace_back(name, pVob);
			}

		}

		finalResultList.DeleteList();

		std::sort(vobsWithNames.begin(), vobsWithNames.end(),
			[](const auto& a, const auto& b) { return a.first < b.first; });


		for (size_t i = 0; i < vobsWithNames.size(); ++i) {
			finalResultList.InsertEnd(vobsWithNames[i].second);
		}

		for (int i = 0; i < finalResultList.GetNumInList(); i++)
		{
			zCVob* nextVob = finalResultList.GetSafe(i);


			if (nextVob && (nextVob->GetDistanceToVob(*ogame->GetCamera()->connectedVob) <= radius * 2) 
				&& nextVob != ogame->GetCamera()->connectedVob
				&& nextVob != theApp.currentVobRender 
				&& nextVob != theApp.currenItemRender
				&& !IsSpacerVob(nextVob)
				)
			{
				

				static addToVobList addEntry = (addToVobList)GetProcAddress(theApp.module, "AddToVobList");
				Stack_PushString(GetVobName(nextVob));
				addEntry((uint)nextVob);
				//vc->AddVob(nextVob);
			}
		}

		theApp.exports.toggleUIElement(UIElementType::UI_WIN_VOBLIST, TRUE);
	}

	void WriteLine(char* message)
	{
		std::cout << (std::string("Union: ").append(message)).c_str() << std::endl;
	}


	int GetRandVal(int min, int max)
	{
		return (rand() % (max - min + 1) + min);
	}


	int GetRandValNegative(int min, int max)
	{
		return min + rand() % (max - min + 1);
	}


	void GetDesktopResolution(int& horizontal, int& vertical)
	{
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);

		horizontal = desktop.right;
		vertical = desktop.bottom;
	}


	void ToggleMusic(bool mode)
	{

		if (mode)
		{
			zoptions->WriteInt("SOUND", "musicEnabled", 1, 0);
			zCMusicSystem::DisableMusicSystem(FALSE);
			zCZoneMusic::SetAutochange(TRUE);
		}
		else
		{
			zoptions->WriteInt("SOUND", "musicEnabled", 0, 0);
			zCZoneMusic::SetAutochange(FALSE);
			zCMusicSystem::DisableMusicSystem(TRUE);
		}
	}

	zSTRING GetFileExtension(zSTRING name)
	{
		std::string filename(name);
		std::string::size_type idx;

		idx = filename.rfind('.');

		if (idx != std::string::npos)
		{
			std::string extension = filename.substr(idx + 1);

			return zSTRING(extension.c_str()).Upper();
		}
		else
		{
			return "";
		}
	}

	uint vTableArr[] = { 0x0082E6F0, 0x0082E6FC, 0x0082E75C, 0x0082E7BC, 0x0082E81C, 0x0082E89C, 0x0082E8DC, 0x0082E904, 0x0082E970, 0x0082E9B4, 0x0082EA4C, 0x0082EB5C, 0x0082EBC4, 0x0082EC2C, 0x0082EC94, 0x0082ECF8, 0x0082ED00, 0x0082ED14, 0x0082ED78, 0x0082ED8C, 0x0082EE60, 0x0082EE8C, 0x0082EF20, 0x0082EF44, 0x0082EF9C, 0x0082EFD8, 0x0082EFEC, 0x0082F00C, 0x0082F044, 0x0082F09C, 0x0082F0EC, 0x0082F0F8, 0x0082F104, 0x0082F148, 0x0082F1EC, 0x0082F278, 0x0082F2D8, 0x0082F2EC, 0x0082F300, 0x0082F384, 0x0082F404, 0x0082F484, 0x0082F520, 0x0082F52C, 0x0082F594, 0x0082F5F0, 0x0082F61C, 0x0082F70C, 0x0082F80C, 0x0082F814, 0x0082F924, 0x0082F92C, 0x0082FA38, 0x0082FA80, 0x0082FA88, 0x0082FACC, 0x0082FB54, 0x0082FB70, 0x0082FBA8, 0x0082FBB0, 0x0082FBB8, 0x0082FBEC, 0x0082FC30, 0x0082FC50, 0x0082FC6C, 0x0082FCA0, 0x0082FCAC, 0x0082FED0, 0x0082FEDC, 0x0082FF24, 0x0082FF2C, 0x0082FF78, 0x0082FF84, 0x0082FFDC, 0x008300B4, 0x00830104, 0x0083016C, 0x008301BC, 0x00830208, 0x00830210, 0x00830218, 0x00830260, 0x0083029C, 0x008303DC, 0x00830450, 0x00830484, 0x00830534, 0x00830690, 0x00830874, 0x008308EC, 0x0083096C, 0x00830A04, 0x00830A64, 0x00830AD4, 0x00830B64, 0x00830C3C, 0x00830CA4, 0x00830CAC, 0x00830D34, 0x00830D3C, 0x00830DC4, 0x00830DCC, 0x00830E54, 0x00830E5C, 0x00830EE8, 0x00830EF4, 0x00830F7C, 0x00830F84, 0x00831010, 0x0083101C, 0x008310C4, 0x008310D0, 0x008310E4, 0x0083113C, 0x0083115C, 0x00831200, 0x0083120C, 0x00831294, 0x0083131C, 0x0083137C, 0x00831404, 0x0083140C, 0x0083151C, 0x008316A8, 0x008316D4, 0x0083184C, 0x008319DC, 0x00831BA4, 0x00831BB4, 0x00831D44, 0x00831D4C, 0x00831D78, 0x00831DAC, 0x00831DCC, 0x00831DEC, 0x00831E0C, 0x00831E2C, 0x00831E4C, 0x00831E6C, 0x00831EA0, 0x00831ED4, 0x00831F54, 0x00831F78, 0x00831FA4, 0x00831FC8, 0x00832084, 0x00832100, 0x00832128, 0x008321E4, 0x00832214, 0x00832228, 0x0083225C, 0x00832290, 0x008322BC, 0x0083234C, 0x008323BC, 0x00832438, 0x00832498, 0x008324B0, 0x008324D4, 0x00832544, 0x008325C4, 0x00832620, 0x008326B4, 0x0083271C, 0x00832964, 0x008329CC, 0x00832A3C, 0x00832A64, 0x00832BC8, 0x00832BE8, 0x00832C0C, 0x00832C4C, 0x00832CBC, 0x00832D54, 0x00832DEC, 0x00832E84, 0x00832F54, 0x00833024, 0x0083305C, 0x00833078, 0x008330A8, 0x008330D8, 0x0083311C, 0x0083314C, 0x0083317C, 0x008331B4, 0x008331C8, 0x008331F0, 0x00833200, 0x00833210, 0x0083988C, 0x008398EC, 0x00839904, 0x0083997C, 0x00839A04, 0x00839A74, 0x00839B4C, 0x00839BC4, 0x00839C3C, 0x00839CB4, 0x00839D2C, 0x00839DA4, 0x00839E1C, 0x00839E94, 0x00839F14, 0x00839F94, 0x0083A00C, 0x0083A084, 0x0083A0FC, 0x0083A174, 0x0083A1EC, 0x0083A264, 0x0083A2DC, 0x0083A33C, 0x0083A39C, 0x0083A3FC, 0x0083A47C, 0x0083A4FC, 0x0083A5A4, 0x0083A630, 0x0083A6A4, 0x0083A72C, 0x0083A784, 0x0083A7E4, 0x0083A7FC, 0x0083A880, 0x0083A89C, 0x0083A924, 0x0083A9AC, 0x0083AA34, 0x0083AABC, 0x0083AB44, 0x0083ABCC, 0x0083AC54, 0x0083ACDC, 0x0083ACEC, 0x0083AD8C, 0x0083AE1C, 0x0083AEBC, 0x0083AF34, 0x0083B0C4, 0x0083B264, 0x0083B404, 0x0083B454, 0x0083B484, 0x0083B4CC, 0x0083B544, 0x0083B5A8, 0x0083B69C, 0x0083B74C, 0x0083B754, 0x0083B77C, 0x0083B7D0, 0x0083B7D8, 0x0083B804, 0x0083B858, 0x0083B860, 0x0083B88C, 0x0083B8E4, 0x0083B8EC, 0x0083B914, 0x0083B970, 0x0083B978, 0x0083B9A4, 0x0083BA08, 0x0083BA10, 0x0083BA3C, 0x0083BA9C, 0x0083BAA4, 0x0083BACC, 0x0083BB48, 0x0083BB84, 0x0083BBAC, 0x0083BBEC, 0x0083BC08, 0x0083BC10, 0x0083BC3C, 0x0083BC90, 0x0083BCB8, 0x0083BCEC, 0x0083BD14, 0x0083BD48, 0x0083BD70, 0x0083BD98, 0x0083BDC0, 0x0083BE2C, 0x0083BE7C, 0x0083BEB0, 0x0083BF00, 0x0083BF38, 0x0083BF6C, 0x0083BFA4, 0x0083BFDC, 0x0083C010, 0x0083C064, 0x0083C10C, 0x0083C1C8, 0x0083C218, 0x0083C220, 0x0083C28C, 0x0083C364, 0x0083C36C, 0x0083C43C, 0x0083C44C, 0x0083C4AC, 0x0083C574, 0x0083C644, 0x0083C714, 0x0083C804, 0x0083C8E0, 0x0083C8EC, 0x0083C914, 0x0083CA3C, 0x0083CB64, 0x0083CCBC, 0x0083CE04, 0x0083CF2C, 0x0083D054, 0x0083D19C, 0x0083D2CC, 0x0083D3FC, 0x0083D474, 0x0083D4D4, 0x0083D58C, 0x0083D6D0, 0x0083D6DC, 0x0083D710, 0x0083D724, 0x0083DA8C, 0x0083DAFC, 0x0083DB6C, 0x0083DBDC, 0x0083DC4C, 0x0083DCBC, 0x0083DD2C, 0x0083DD9C, 0x0083DE0C, 0x0083DE7C, 0x0083DF08, 0x0083DF80, 0x0083DFE4, 0x0083E08C, 0x0083E120, 0x0083E138, 0x0083E174, 0x0083E220, 0x0083E23C, 0x0083E2FC, 0x0083E304, 0x0083E30C, 0x0083E344, 0x0083E364, 0x0083E3D8, 0x0083E3EC, 0x0083E410, 0x0083E428, 0x0083E434, 0x0083E4AC, 0x00842368, 0x008423FC, 0x00842584, 0x00842598, 0x00842AF8, 0x008437D8 };

	bool IsValidZObject(void* ptr)
	{
		return true; //no check, extended classes does not work
		if (shi_msize(ptr) != -1)
		{
			uint vt_address = *(uint*)ptr;

			int count = (sizeof(vTableArr) / sizeof(uint));

			for (int i = 0; i < count; i++)
			{
				if (vTableArr[i] == vt_address)
				{
					return true;
				}
			}

		}

		return false;
	}


	Common::Map<CString, CString> wordsMap;

	void ClearLangStrings()
	{
		wordsMap.Clear();
	}

	CString GetLang(CString key)
	{
		//cmd << "Key: " << key << endl;

		auto& foundPair = wordsMap[key];

		CString result = "";

		if (!foundPair.IsNull())
		{
			result =  foundPair.GetValue();
		}
		else
		{
			Stack_PushString(key);

			int codePage = 1251;
			static auto caller = (voidFuncPointer)GetProcAddress(theApp.module, "GetLangString");
			codePage = caller();
			result =  Stack_PeekStringW().WToA(codePage);
			wordsMap.Insert(key, result);
			//cmd << "Get new string from c#: " << result << " cp: " << codePage << endl;
		}

		return result;
		
	}

	int GetZenLevelCompoCount()
	{
		zCArray<zCVob*> resultList;
		zCWorld* world = ogame->GetWorld();

		world->SearchVobListByClass(zCVobLevelCompo::classDef, resultList, 0);

		zCVob* parentVob = NULL;

		return resultList.GetNumInList();
	}

	template <class T>
	inline T zLerp(const float t, const T& a, const T& b) {
		return a + t*(b - a);
	}

	template <class T>
	inline void zClamp(T& x, const T min, const T max) {
		if (x<min) x = min; else
			if (x>max) x = max;
	}

	zVEC3 Slerp(zVEC3 start, zVEC3 end, float percent)
	{
		float dot = start.Dot(end);

		zClamp(dot, -1.0f, 1.0f);

		float theta = acos(dot)*percent;
		zVEC3 RelativeVec = end - start*dot;
		RelativeVec.Normalize();

		return ((start*cos(theta)) + (RelativeVec*sin(theta)));
	}


	bool IsVirtualFile(CString path)
	{
		path = path.Replace("/", "\\");

		Array<string> pathes = (A path).Split("\\");

		int workPos = path.Search("\\_WORK\\", 0, true);


		path = "_WORK\\" + path.Copy(workPos, path.Length() - workPos);

		//cmd << "path =  " << path << endl;

		CString fileName = pathes[pathes.GetNum() - 1];

		//cmd << "Checking for " << fileName << endl;


		char matches[2048];
		vdf_searchfile(fileName.ToChar(), matches);
		Array<string> names = (A matches).Split("|");

		for (uint i = 0; i < names.GetNum(); i++) {

			//cmd << names[i] << endl;

			bool_t isVirtual = vdf_fexists(names[i].ToChar(), VDF_VIRTUAL);
			bool_t isPhysical = vdf_fexists(names[i].ToChar(), VDF_PHYSICAL);

			
			//cmd << isVirtual << endl;
			//cmd << isPhysical << endl;
			

			if (isVirtual && names[i] == path)
			{
				//cmd << "WARNING VIRTUAL!" << endl;
				return true;
			}
			
			
			
		}

		return false;
	}


	void _GetCursorPos(POINT* cur)
	{
		GetCursorPos(cur);

		RECT rect;
		GetWindowRect(hWndApp, &rect);

		cur->x -= rect.left;
		cur->y -= rect.top;
	}

	zVEC3 IsSphIntersect(zVEC3 originPos, zVEC3 sp_pos, zVEC3 radNorm, float radius_sphere)
	{
		zVEC3 diff = originPos - sp_pos;
		float A_Point = radNorm.Dot(radNorm);
		float B_Point = 2 * diff.Dot(radNorm);
		float C_Point = pow(diff.Length(), 2) - pow(radius_sphere, 2);

		float D = B_Point * B_Point - 4 * A_Point * C_Point;

		if (D < 0.0) return NULL;

		zVEC3 t1 = originPos + radNorm*(-B_Point - sqrt(D)) / (2 * A_Point);
		zVEC3 t2 = originPos + radNorm*(-B_Point + sqrt(D)) / (2 * A_Point);

		return t1.Length() <= t2.Length() ? t1 : t2;
	}


	void DrawPolygon(zCPolygon* poly, zCOLOR& color)
	{
		// protect ptr
		if (!poly)
			return;

		// run through all points of the polygon
		for (int i = 0; i < poly->polyNumVert; i++)
		{
			// get vertex coordinates
			zVEC3 vtx1 = poly->vertex[i]->position;

			// choice of next vertex index, taking into account the last
			int next = (i + 1 >= poly->polyNumVert) ? 0 : i + 1;

			// get next vertex coordinates
			zVEC3 vtx2 = poly->vertex[next]->position;

			// draw a line with two points
			zlineCache->Line3D(vtx1, vtx2, color, 0);
		}
	};


	inline float CalcAlphaFromDistToPlane(const zTPlane &camPlane, const zVEC3& pos)
	{
		static float lastZ = -1;
		static float DETAIL_MAX_DIST = -1;
		static float ONE_OVER_DETAIL_MAX_DIST = -1;

		if (zCCamera::activeCam->farClipZ - 1000 != DETAIL_MAX_DIST)
		{
			DETAIL_MAX_DIST = zCCamera::activeCam->farClipZ - 1000;
			if (DETAIL_MAX_DIST<1000) DETAIL_MAX_DIST = 1000;
			ONE_OVER_DETAIL_MAX_DIST = 1.0f / DETAIL_MAX_DIST;
		};

		zREAL dist = camPlane.GetDistanceToPlane(pos);
		if (dist<DETAIL_MAX_DIST)
		{
			dist = dist * 2 - DETAIL_MAX_DIST;
			float alpha = (dist <= 0) ? 1 : zREAL(1.0F) - (dist * ONE_OVER_DETAIL_MAX_DIST);
			return alpha;
		}
		else
		{
			return 0;
		};
	};


	inline float zMin(const float a, const float b) {
		if (a<b) return a;
		else	 return b;
	}

	inline float zMax(const float a, const float b) {
		if (a>b) return a;
		else	 return b;
	}

	inline int zMin(const int a, const int b) {
		if (a<b) return a;
		else	 return b;
	}

	inline int zMax(const int a, const int b) {
		if (a>b) return a;
		else	 return b;
	}

	float zTBBox3D::GetMaxExtent()  { 
		return zREAL(0.5F) * zMax(zMax(maxs[0] - mins[0], maxs[1] - mins[1]), maxs[2] - mins[2]); 
	};


	int Union_FileExists(CString searchName)
	{
		bool foundVirtual = false;

		auto result = vdf_fexists(searchName.ToChar(), VDF_DEFAULT);

		if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
		{
			foundVirtual = true;
			return 2;
		}


		result = vdf_fexists(searchName.ToChar(), VDF_PHYSICAL);

		if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
		{
			return 1;
		}

		if (result == 0 && !foundVirtual)
		{
			return 0;

		}

		return 0;
	
	}


	// packing RGBA colors in ulong (4 bytes)
	unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
	{
		return ((iA * 256 + iR) * 256 + iG) * 256 + iB;
	}



	// decomposition of 4 bytes into RGBA components
	RGBA DWORD2RGBA(unsigned long dwColor)
	{
		RGBA tmp;

		tmp.B = dwColor % 256; dwColor /= 256;
		tmp.G = dwColor % 256; dwColor /= 256;
		tmp.R = dwColor % 256; dwColor /= 256;
		tmp.Alpha = dwColor % 256;

		return tmp;
	}


	/*
	unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
	{        
	  return (((((iA << 8) + iR) << 8) + iG) << 8) + iB;
	}

	RGBA DWORD2RGBA(unsigned long dwColor)
	{        
	  RGBA tmp;

	  tmp.B = dwColor & 0xFF; dwColor >>= 8;
	  tmp.G = dwColor & 0xFF; dwColor >>= 8;
	  tmp.R = dwColor & 0xFF; dwColor >>= 8;
	  tmp.Alpha = dwColor & 0xFF;
	  return tmp;
	}
	*/

	
	void merge(void* arr, size_t left, size_t mid, size_t right, size_t size, int(*compare)(const void*, const void*)) {
		char* left_arr = (char*)arr + left * size;
		char* right_arr = (char*)arr + mid * size;
		char* temp = new char[(right - left) * size];

		size_t i = 0, j = 0, k = 0;
		while (i < mid - left && j < right - mid) {
			if (compare(left_arr + i * size, right_arr + j * size) < 0) {
				memcpy(temp + k * size, left_arr + i * size, size);
				i++;
			}
			else {
				memcpy(temp + k * size, right_arr + j * size, size);
				j++;
			}
			k++;
		}

		while (i < mid - left) {
			memcpy(temp + k * size, left_arr + i * size, size);
			i++;
			k++;
		}

		while (j < right - mid) {
			memcpy(temp + k * size, right_arr + j * size, size);
			j++;
			k++;
		}

		memcpy((char*)arr + left * size, temp, (right - left) * size);
		delete[] temp;
	}

	void merge_sort(void* arr, size_t left, size_t right, size_t size, int(*compare)(const void*, const void*)) {
		if (left + 1 < right) {
			size_t mid = (left + right) / 2;
			merge_sort(arr, left, mid, size, compare);
			merge_sort(arr, mid, right, size, compare);
			merge(arr, left, mid, right, size, compare);
		}
	}

	void sortData(void* data, size_t num, size_t size, int(*compare)(const void*, const void*)) {
		merge_sort(data, 0, num, size, compare);
	}

	void zCParser::SetScriptInt(zSTRING name, int value, int index) {
		zCPar_Symbol* sym = GetSymbol(name);

		if (sym) {

			sym->SetValue(value, index);
		}
	}

	zSTRING zCParser::GetScriptString(zSTRING const& str, int index) {
		zSTRING string;
		zCPar_Symbol* ps = GetSymbol(str);
		if (ps)
			ps->GetValue(string, index);
		if (string.Length())
			return string;
		else
			return " ";
		return " ";
	};


	void GetChildrenVobMesh(zCTree<zCVob>* node, zCVob* pickedVob, zCMesh* pMesh)
	{
		zCVob* vobChild = node->GetData();

		if (vobChild)
		{
			if (vobChild->visual)
			{
				
#if ENGINE < Engine_G2
				zCProgMeshProto* pProgMeshChild = vobChild->visual->CastTo<zCProgMeshProto>();
				if (pProgMeshChild)
				{
					zCMesh* pMeshChild = pProgMeshChild->GetMesh_G1(0);

					if (pMeshChild)
					{
						cmd << "Merge: " << vobChild->GetObjectName() << endl;

						pMesh->MergeMesh(pMeshChild, vobChild->trafoObjToWorld);
					}

				}
				
#else
				// ďűňŕĺěń˙ ďđĺîáđŕçîâŕňü âčçóŕë â "ďđîă ěĺř ďđîňî"
				zCProgMeshProto* pProgMeshChild = vobChild->visual->CastTo<zCProgMeshProto>();
				if (pProgMeshChild)
				{
					zCMesh* pMeshChild = pProgMeshChild->GetMesh(0);

					if (pMeshChild)
					{
						cmd << "Merge: " << vobChild->GetObjectName() << endl;

						pMesh->MergeMesh(pMeshChild, vobChild->trafoObjToWorld);
					}

				}
#endif
			}

		}

		node = node->GetFirstChild();

		while (node != NULL)
		{
			GetChildrenVobMesh(node, pickedVob, pMesh);
			node = node->GetNextChild();
		}
	}

	void SaveProgMeshToFile(zCVob* pVob, bool includeChildren, CString name)
	{

		cmd << "SaveProgMeshToFile: " << pVob->GetVobName() 
			<< " includeChildren: " << (int)includeChildren 
			<< " name: " << name
			<< endl;

		if (!pVob || !pVob->visual || pVob->CastTo<zCVobLevelCompo>())
		{
			print.PrintRed(GetLang("ACTION_FORBIDDEN"));
			return;
		}



		// FIXME_G1 SUPPORT GetMesh
		//print.PrintRed("NO G1 SUPPORT!");
		//return;


		zCProgMeshProto* pProgMesh = pVob->visual->CastTo<zCProgMeshProto>();

		if (!pProgMesh)
		{
			print.PrintRed(GetLang("VOB_VISUAL_SAVE_FAIL"));
			return;
		}
			

#if ENGINE == Engine_G1
		zCMesh* pMesh = pProgMesh->GetMesh_G1(0);
#else
		zCMesh* pMesh = pProgMesh->GetMesh(0);
#endif // DEBUG


		if (!pMesh || name.Length() == 0)
		{
			print.PrintRed(GetLang("VOB_VISUAL_SAVE_FAIL"));
			return;
		}
		

		pMesh->TransformAllVerts(pVob->trafoObjToWorld, FALSE);

		if (includeChildren)
		{
			if (pVob->globalVobTreeNode)
			{
				GetChildrenVobMesh(pVob->globalVobTreeNode, pVob, pMesh);
			}
		}

		zoptions->ChangeDir(DIR_MESHES);

		cmd << "try to save" << endl;


		zSTRING fileName = name;

		fileName.Replace(".3DS", ".MSH");

		if (!fileName.Contains(".MSH"))
		{
			fileName = fileName + ".MSH";
		}


		pMesh->SaveMSH(fileName);

		print.PrintGreen(GetLang("VOB_VISUAL_SAVE_SUCCESS"));

		cmd << "Try to remove..." << endl;

		pMesh->DeleteAll();
		zRELEASE(pMesh);


	}

	void Clipboard(const char* output)
	{
		const size_t len = strlen(output) + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		memcpy(GlobalLock(hMem), output, len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}

	void PrintMobName(oCMOB* pMob)
	{
		if (!pMob) return;

		if (pMob->name.Length() == 0)
		{
			return;
		}

		if (auto visual = pMob->GetVisual())
		{
			zPOINT3 focusNamePosWS;

			focusNamePosWS = pMob->GetBBox3DWorld().GetCenter();
			focusNamePosWS.n[VY] += (pMob->GetBBox3DWorld().maxs.n[VY] - pMob->GetBBox3DWorld().mins.n[VY]) * 0.82F;

			zCCamera::activeCam->Activate();
			zPOINT3	  ProjPoint1 = zCCamera::activeCam->Transform(focusNamePosWS);
			if (ProjPoint1[VZ] > 0.0f)
			{
				auto text = parser->GetScriptString(pMob->name);
				auto scene = "ScemeName: " + pMob->GetScemeName();
				

				zPOINT2	  ProjPoint2;
				zCCamera::activeCam->Project(&ProjPoint1, ProjPoint2[VX], ProjPoint2[VY]);
				ProjPoint2[VX] = ProjPoint2[VX] - screen->nax(screen->FontSize(text) / 2);

				// CLAMP
				int nX = screen->anx(ProjPoint2[VX]);
				int nY = screen->any(ProjPoint2[VY]);

				if (nY < screen->FontY()) nY = screen->FontY();
				if (nY > 8192 - screen->FontY()) nY = 8192 - screen->FontY();

				// [Ulf] Clampen auch in X-Richtung
				zClamp(nX, 0, 8191 - screen->FontSize(text));

				screen->Print(nX, nY, text);

				if (theApp.options.GetIntVal("bShowMobScheme"))
				{
					screen->Print(nX, nY + 220, scene);
				}

				
			};
		}
	}

	void PrintItemName(oCItem* pItem)
	{
		if (!pItem) return;



		if (auto visual = pItem->GetVisual())
		{
			zPOINT3 focusNamePosWS;

			focusNamePosWS = pItem->GetBBox3DWorld().GetCenter();
			focusNamePosWS.n[VY] += (pItem->GetBBox3DWorld().maxs.n[VY] - pItem->GetBBox3DWorld().mins.n[VY]) * 1.75F;

			zCCamera::activeCam->Activate();
			zPOINT3	  ProjPoint1 = zCCamera::activeCam->Transform(focusNamePosWS);
			if (ProjPoint1[VZ] > 0.0f)
			{
				auto text = pItem->GetName(0);

				zPOINT2	  ProjPoint2;
				zCCamera::activeCam->Project(&ProjPoint1, ProjPoint2[VX], ProjPoint2[VY]);
				ProjPoint2[VX] = ProjPoint2[VX] - screen->nax(screen->FontSize(text) / 2);

				// CLAMP
				int nX = screen->anx(ProjPoint2[VX]);
				int nY = screen->any(ProjPoint2[VY]);

				if (nY < screen->FontY()) nY = screen->FontY();
				if (nY > 8192 - screen->FontY()) nY = 8192 - screen->FontY();

				// [Ulf] Clampen auch in X-Richtung
				zClamp(nX, 0, 8191 - screen->FontSize(text));

				screen->Print(nX, nY, text);

			};
		}
	}

	zCArray<zSTRING> Split(zSTRING str, char t) {
		CString cstr = str;
		zCArray<zSTRING> carr;
		Array<CStringA> arr = cstr.Split(t);
		for (uint i = 0; i < arr.GetNum(); i++) {
			CStringA* node = arr.GetSafe(i);
			if (node) {
				zSTRING snode = *node;
				carr.Insert(snode);
			}
		}

		return carr;
	}

	bool IsItemExistsInScript(zSTRING itemInstName)
	{
		static int oCItemIndex = parser->GetIndex(oCItem::classDef->scriptClassName);

		int itemIndex = parser->GetIndex(itemInstName);

		return parser->GetBaseClass(itemIndex) == oCItemIndex;

	}

	bool FindVobByName(zSTRING triggerName)
	{
		zCArray<zCVob*> activeVobList;

		auto pVob = ogame->GetWorld()->SearchVobByName(triggerName);

		return pVob != NULL;
	}


	bool ContainsSymbolsOtherThanRL(const std::string& str) {
		for (const char& c : str) {
			if (c != 'R' && c != 'L') {
				return true;
			}
		}
		return false;
	}

	bool IsVobWaypointOrFreepoint(zCVob* pVob)
	{
		if (!pVob) return false;

		auto pWP = pVob->CastTo<zCVobWaypoint>();
		auto pFP = pVob->CastTo<zCVobSpot>();

		return pWP || pFP;
	}

	zSTRING GetMaterialInfoAsString(zCMaterial* mat)
	{
		zSTRING text = "Mat: " + mat->GetName()
			+ " | Texture: " + mat->texture->GetObjectName() 
			+ " | noCollDet: " + Z mat->noCollDet
			+ " | MatGroup: " + Z mat->GetMatGroupString()		
			;

		return text;
	}
#if ENGINE >= Engine_G2
	zBOOL zVEC3::IsEqualEps(const zVEC3& b) const {								// fuzzy compare with small epsilon
		return ((zAbs(zREAL(n[0] - b.n[0])) < zALG_EQUAL_EPSILON) &&
			(zAbs(zREAL(n[1] - b.n[1])) < zALG_EQUAL_EPSILON) &&
			(zAbs(zREAL(n[2] - b.n[2])) < zALG_EQUAL_EPSILON));
	};
#endif

	std::string zMAT4::ToString()
	{
		std::string result;

		result += "\n";

		for (int i = 0; i < 4; ++i) {
			result += "[";

			for (int j = 0; j < 4; ++j) {
				float value = v[i][j];

				result += std::to_string(value); // Преобразуем число в строку
				if (j < 3) {
					result += ", "; // Добавляем запятую между элементами
				}
			}

			result += "]\n"; // Добавляем символ новой строки
		}

		return result;
	}

	extern void DrawLightSphere_Loop();


	void UpdatePfxBbox(zCVob* pickedVob)
	{
#if ENGINE >= Engine_G2
		if (auto pVisual = pickedVob->GetVisual())
		{
			if (zCParticleFX* pfx = dynamic_cast<zCParticleFX*>(pVisual))
			{
				zTParticle* p = pfx->firstPart;

				zTBBox3D& bb = pfx->bbox3DWorld;

				bb.Init();

				if (p)
				{
					while (p)
					{
						bb.AddPoint(p->positionWS);
						p = p->next;
					}

				}
				else
				{
					pfx->bbox3DWorld.Init();

					zVEC3 vobPos = pickedVob->GetPositionWorld();

					const zVEC3 vMax(50, 50, 50);

					pfx->bbox3DWorld.AddPoint(vobPos + vMax);
					pfx->bbox3DWorld.AddPoint(vobPos - vMax);

					pickedVob->bbox3D = pfx->bbox3DWorld;

				}
			}
		}
#endif

	}

#if ENGINE == Engine_G1
	zCMesh* zCProgMeshProto::GetMesh_G1(const int a_iLODIndex)
	{
		zCMesh* mesh = zNEW(zCMesh);
		int numPoly = 0;

		for (int n = 0; n < numSubMeshes; n++) numPoly += subMeshList[n].triList.GetNum();

		//zCMesh::S_InitVertexMergeCache(mesh);

		mesh->AllocPolys(numPoly);
		mesh->AllocVerts(numPoly * 3);

		for (int i = 0; i < numSubMeshes; i++)
		{
			for (int p = 0; p < subMeshList[i].triList.GetNum(); p++)
			{
				zPOINT3 a, b, c;
				zVEC2	f, g, h;
				a = posList[subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[0]].position];
				b = posList[subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[1]].position];
				c = posList[subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[2]].position];
				f = subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[0]].texUV;
				g = subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[1]].texUV;
				h = subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[2]].texUV;
				zCPolygon* newPoly = mesh->AddPoly();
				newPoly->AddVertex(mesh->AddVertexSmart(a));
				newPoly->AddVertex(mesh->AddVertexSmart(b));
				newPoly->AddVertex(mesh->AddVertexSmart(c));
				zERR_ASSERT(newPoly);
				newPoly->SetMapping(0, f);
				newPoly->SetMapping(1, g);
				newPoly->SetMapping(2, h);
				newPoly->polyPlane = subMeshList[i].triPlaneList[subMeshList[i].triPlaneIndexList[p]];
				newPoly->SetMaterial(subMeshList[i].material);

			};
		}

		mesh->meshName = this->GetObjectName();
		//zCMesh::S_DeleteVertexMergeCache();

		return mesh;
	}
#endif


	int RAMUsed() {
		int nRam;

		PROCESS_MEMORY_COUNTERS pmc;

		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		nRam = pmc.WorkingSetSize / 1000;
		return nRam;
	}

	void CheckRamUsage()
	{
		if (theApp.options.GetIntVal("bCheckRamMemory"))
		{
			if (theApp.IsDx11Active() && RAMUsed() >= 2500000)
			{
				print.PrintRed(GetLang("COMMON_WARN_MEMORY_FULL"), 5);

			}
			else if (!theApp.IsDx11Active() && RAMUsed() >= 2650000)
			{
				print.PrintRed(GetLang("COMMON_WARN_MEMORY_FULL"), 5);
			}
		}
	}

	void PrintInfoWinMessage(zSTRING text, zSTRING color = "#FF00000")
	{
		static auto pointer = (callVoidFunc)GetProcAddress(theApp.module, "InfoWin_AddText");

		Stack_PushString("\n" + text + "\n");
		Stack_PushString(color);

		pointer();
	}
}

