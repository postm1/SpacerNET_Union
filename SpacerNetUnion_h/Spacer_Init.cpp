// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	

	char* dllName = "SpacerUnionInterface.dll";

	//CInvoke<void(__thiscall*)(CGameManager*, struct HWND__*&)> Ivk_CGameManager_Init(0x00424C70, &CGameManager::Init_Spacer, IVK_AUTO);
	HOOK Ivk_CGameManager_Init AS(&CGameManager::Init, &CGameManager::Init_Spacer);

	void CGameManager::Init_Spacer(struct HWND__ *& hwnd)
	{
		theApp.module = CPlugin::FindModule(dllName);

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


		THISCALL(Ivk_CGameManager_Init)(hWndApp);

		//Ivk_CGameManager_Init(this, hWndApp);
	}

	void SpacerApp::Init()
	{
		theApp.options.Init();
		theApp.options.Load();


		keys.InitValues();
		//zoptions->AddParameters("-MERGEVOBSWITHLEVEL");

		int limitFps = theApp.options.GetIntVal("maxFPS");

		if (limitFps > 0)
		{
			ztimer->LimitFPS(limitFps);
		}

		print.Init();
		//RX_FIX
		//LoadMat();

		auto enableInterface = (voidFuncPointer)GetProcAddress(theApp.module, "Form_EnableInterface");

		if (enableInterface) {
			enableInterface();
		}


		// PlaySoundGame(ToStr "SPACER_INIT");
		//cmd << "Game_Init" << endl;
	}
}