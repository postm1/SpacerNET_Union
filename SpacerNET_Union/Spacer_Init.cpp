// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	

	CString INTERFACE_DLL_NAME = "SpacerUnionInterface.dll";

	HOOK Ivk_CGameManager_Init AS(&CGameManager::Init, &CGameManager::Init_Spacer);

	void CGameManager::Init_Spacer(struct HWND__ *& hwnd)
	{
		cmd << "CGameManager::Init_Spacer()" << endl;



		enum { GOTHIC1_EXECUTABLE = 0, GOTHIC1A_EXECUTABLE = 1, GOTHIC2_EXECUTABLE = 2, GOTHIC2A_EXECUTABLE = 3, INVALID_EXECUTABLE = -1 };

		int foundExecutable = INVALID_EXECUTABLE;

		DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));

		if (*reinterpret_cast<DWORD*>(baseAddr + 0x160) == 0x37A8D8 && *reinterpret_cast<DWORD*>(baseAddr + 0x37A960) == 0x7D01E4
			&& *reinterpret_cast<DWORD*>(baseAddr + 0x37A98B) == 0x7D01E8)
		{
			foundExecutable = GOTHIC1_EXECUTABLE;
		}
		else if (*reinterpret_cast<DWORD*>(baseAddr + 0x168) == 0x3D4318 && *reinterpret_cast<DWORD*>(baseAddr + 0x3D43A0) == 0x82E108
			&& *reinterpret_cast<DWORD*>(baseAddr + 0x3D43CB) == 0x82E10C) 
		{
			foundExecutable = GOTHIC2A_EXECUTABLE;
		}
		//cmd << foundExecutable << endl;

#if ENGINE == Engine_G1
		if (foundExecutable != GOTHIC1_EXECUTABLE)
		{
			MessageBox(0, "Bad Gothic 1 EXE version! Use only 1.08k (mod)!", 0, 0);
			exit(0);
		}

#elif ENGINE == Engine_G2A
		if (foundExecutable != GOTHIC2A_EXECUTABLE)
		{
			MessageBox(0, "Bad Gothic 2 NR EXE version! Use only 2.6.0.0 version!", 0, 0);
			exit(0);
		}
#endif

		//cmd << "FindModule ok" << endl;

		if (theApp.module) {

			(voidFuncPointerStart)GetProcAddress(theApp.module, "UnionInitialize")();

			auto GetHandlerFunc = (voidFuncPointer)GetProcAddress(theApp.module, "GetHandlerFunc");

			if (GetHandlerFunc) {

				DestroyWindow(hwnd);

				//get handler from c#
				hWndApp = (HWND)GetHandlerFunc();

				auto getMainWinHandle = (intFuncPointer)GetProcAddress(theApp.module, "GetMainWinHandler");

				if (getMainWinHandle) {
					theApp.mainWin = (HWND)getMainWinHandle();
				}

				auto vobsWinHandle = (intFuncPointer)GetProcAddress(theApp.module, "GetVobsWinHandler");

				if (vobsWinHandle) {
					theApp.vobsWin = (HWND)vobsWinHandle();
				}



			}
		}
		else {
			MessageBox(0, "Error while loading .NET dll!", 0, 0);
			exit(0);
		}

		//cmd << "Ivk_CGameManager_Init" << endl;

		THISCALL(Ivk_CGameManager_Init)(hWndApp);
	}

	void SpacerApp::Init()
	{

		/*
		if (!zoptions->ReadBool("VIDEO", "zStartupWindowed", FALSE))
		{
			MessageBox(0, "Ńďĺéńĺđ íóćíî çŕďóńęŕňü â îęîííîě đĺćčěĺ! zStartupWindowed=1 â gothic.ini\nYou must launch SPACER_NET in a window mode! Set zStartupWindowed=1 in gothic.ini", 0, 0);
			exit(0);
		}
		*/

		// create directories for spacer_net
		CreateDirectory("_work\\tools", NULL);
		CreateDirectory("_work\\tools\\data", NULL);
		CreateDirectory("_work\\tools\\vobcatalog_backups", NULL);

		CreateDirectory("_work\\data", NULL);
		CreateDirectory("_work\\data\\meshes\\", NULL);
		CreateDirectory("_work\\data\\meshes\\_compiled\\", NULL);


		if (!IsDx11Active())
		{
			//if (zrenderer) zrenderer->Vid_SetScreenMode(zRND_SCRMODE_HIDE);
		}

		filterPickVobIndex = 0;

		SetExportsFuncs();

		//cmd << "options" << endl;
		options.Init();
		options.Load();
		visualEditorActive = false;

		//cmd << "InitValues" << endl;
		keys.InitValues();
		//zoptions->AddParameters("-MERGEVOBSWITHLEVEL");

		int limitFps = options.GetIntVal("maxFPS");

		/*
		if (limitFps > 0)
		{
			ztimer->LimitFPS(limitFps);
		}
		else
		{
			ztimer->minFrameTime = 0;
		}
		*/

		//cmd << "LoadMat" << endl;
		debug.Init();
		print.Init();
		

		mm.Init();

		(voidFuncPointer)GetProcAddress(theApp.module, "Form_EnableInterface")();
		//zrenderer->Vid_SetScreenMode(zRND_SCRMODE_WINDOWED);

		// PlaySoundGame(ToStr "SPACER_INIT");
		//cmd << "Game_Init" << endl;

		itemsLocator.Init();
		nograss.Init();
		camMan.Init();


		
		mf.Init();

		//presetsLib.Load();

		// kill gLogStatistics
#if ENGINE == Engine_G1
		*(bool*)0x0085EB00 = false;
#elif ENGINE == Engine_G1A
		*(bool*)0x008A3454 = false;
#elif ENGINE == Engine_G2
		*(bool*)0x008B4590 = false;
#elif ENGINE == Engine_G2A
		*(bool*)0x008C2B50 = false;
#endif

		showRespawnOnVobsRadius = options.GetIntVal("showSpawnListRadius");


		if (showRespawnOnVobsRadius < 2000 || showRespawnOnVobsRadius > 25000)
		{
			showRespawnOnVobsRadius = 7500;
		}

		spacerWasInit = true;

		// check for uniq mod vars
		this->isNewBalanceMod = parser->GetSymbol("RX_TP_AZGAN_FORT") && parser->GetSymbol("RX_IsScoutUpdate") && parser->GetSymbol("RX_IsDruidUpdate") && parser->GetSymbol("AIVRX_NPC_SPEED");
		
	}
}