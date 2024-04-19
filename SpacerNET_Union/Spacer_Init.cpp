// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	

	CString INTERFACE_DLL_NAME = "SpacerUnionInterface.dll";

	HOOK Ivk_CGameManager_Init AS(&CGameManager::Init, &CGameManager::Init_Spacer);

	void CGameManager::Init_Spacer(struct HWND__ *& hwnd)
	{
		cmd << "CGameManager::Init_Spacer()" << endl;

#if ENGINE == Engine_G1

		enum { GOTHIC1_EXECUTABLE = 0, GOTHIC1A_EXECUTABLE = 1, GOTHIC2_EXECUTABLE = 2, GOTHIC2A_EXECUTABLE = 3, INVALID_EXECUTABLE = -1 };

		int foundExecutable = INVALID_EXECUTABLE;

		DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));

		if (*reinterpret_cast<DWORD*>(baseAddr + 0x160) == 0x37A8D8 && *reinterpret_cast<DWORD*>(baseAddr + 0x37A960) == 0x7D01E4
			&& *reinterpret_cast<DWORD*>(baseAddr + 0x37A98B) == 0x7D01E8) {
			foundExecutable = GOTHIC1_EXECUTABLE;
		}

		if (foundExecutable != GOTHIC1_EXECUTABLE)
		{
			MessageBox(0, "Bad Gothic 1 EXE version! Only 1.08k (mod) supported!", 0, 0);
			exit(0);
		}

		cmd << foundExecutable << endl;
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
			MessageBox(0, "Спейсер нужно запускать в оконном режиме! zStartupWindowed=1 в gothic.ini\nYou must launch SPACER_NET in a window mode! Set zStartupWindowed=1 in gothic.ini", 0, 0);
			exit(0);
		}
		*/

		// create directories for spacer_net
		CreateDirectory("_work\\tools", NULL);
		CreateDirectory("_work\\tools\\data", NULL);

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
		theApp.options.Init();
		theApp.options.Load();
		theApp.visualEditorActive = false;

		//cmd << "InitValues" << endl;
		keys.InitValues();
		//zoptions->AddParameters("-MERGEVOBSWITHLEVEL");

		int limitFps = theApp.options.GetIntVal("maxFPS");

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

#if ENGINE == Engine_G1
		// kill gLogStatistics
		* (int*)0x0085EB00 = 0;
#else
		// kill gLogStatistics
		* (int*)0x008C2B50 = 0;
#endif

		spacerWasInit = true;
		
	}
}