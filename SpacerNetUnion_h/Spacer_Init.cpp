// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	

	CString INTERFACE_DLL_NAME = "SpacerUnionInterface.dll";

	HOOK Ivk_CGameManager_Init AS(&CGameManager::Init, &CGameManager::Init_Spacer);

	void CGameManager::Init_Spacer(struct HWND__ *& hwnd)
	{
		//Gothic_II_Addon::hWndApp
		//cmd << "Init_Spacer" << endl;

		theApp.module = CPlugin::FindModule(INTERFACE_DLL_NAME);

		//cmd << "FindModule ok" << endl;

		if (theApp.module) {

			(voidFuncPointerStart)GetProcAddress(theApp.module, "UnionInitialize")();

			auto GetHandlerFunc = (voidFuncPointer)GetProcAddress(theApp.module, "GetHandlerFunc");

			if (GetHandlerFunc) {

				DestroyWindow(hwnd);

				//получить handler из с#
				hWndApp = (HWND)GetHandlerFunc();

				auto getMainWinHandle = (intFuncPointer)GetProcAddress(theApp.module, "GetMainWinHandler");

				if (getMainWinHandle) {
					theApp.mainWin = (HWND)getMainWinHandle();
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
		//cmd << "options" << endl;
		theApp.options.Init();
		theApp.options.Load();
		theApp.visualEditorActive = false;

		//cmd << "InitValues" << endl;
		keys.InitValues();
		//zoptions->AddParameters("-MERGEVOBSWITHLEVEL");

		int limitFps = theApp.options.GetIntVal("maxFPS");

		if (limitFps > 0)
		{
			ztimer->LimitFPS(limitFps);
		}
		else
		{
			ztimer->minFrameTime = 0;
		}

		//cmd << "LoadMat" << endl;

		print.Init();
		LoadMat();

		(voidFuncPointer)GetProcAddress(theApp.module, "Form_EnableInterface")();
		//zrenderer->Vid_SetScreenMode(zRND_SCRMODE_WINDOWED);

		// PlaySoundGame(ToStr "SPACER_INIT");
		//cmd << "Game_Init" << endl;
	}
}