// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	
	
	CString INTERFACE_DLL_NAME = "SpacerUnionInterface.dll";

	HOOK Ivk_CGameManager_Init AS(&CGameManager::Init, &CGameManager::Init_Spacer);

	void CGameManager::Init_Spacer(struct HWND__ *& hwnd)
	{
		//Gothic_II_Addon::hWndApp
		cmd << "CGameManager::Init_Spacer()" << endl;


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


		if (!zoptions->ReadBool("VIDEO", "zStartupWindowed", FALSE))
		{
			MessageBox(0, "Спейсер нужно запускать в оконном режиме! zStartupWindowed=1 в gothic.ini\nYou must launch SPACER_NET in a window mode! Set zStartupWindowed=1 in gothic.ini", 0, 0);
			exit(0);
		}

		if (!IsDx11Active())
		{
			//if (zrenderer) zrenderer->Vid_SetScreenMode(zRND_SCRMODE_HIDE);
		}

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

		mm.Init();

		(voidFuncPointer)GetProcAddress(theApp.module, "Form_EnableInterface")();
		//zrenderer->Vid_SetScreenMode(zRND_SCRMODE_WINDOWED);

		// PlaySoundGame(ToStr "SPACER_INIT");
		//cmd << "Game_Init" << endl;

		itemsLocator.Init();
		nograss.Init();


		
		
	}
}