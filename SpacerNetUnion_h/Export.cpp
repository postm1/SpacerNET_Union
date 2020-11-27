// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	// Функции вызываются из C#


	extern "C"
	{
		__declspec(dllexport) void Extern_LoadWorld() {

			CString path = Stack_PeekString().Upper();
			theApp.LoadWorld(path);

		}

		__declspec(dllexport) void Extern_PrintRed() {

			CString str = Stack_PeekString();

			print.PrintRed(str);
		}

		__declspec(dllexport) void Extern_PrintGreen() {

			CString str = Stack_PeekString();

			print.PrintGreen(str);
		}


		__declspec(dllexport) void Extern_PlayHero() {
			print.PrintRed("Playing the game...");
			//theApp.PlayTheGame();
		}



		__declspec(dllexport) void Extern_LoadMesh() {
			CString path = Stack_PeekString().Upper();
			theApp.LoadMesh(path);
			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}

		__declspec(dllexport) void Extern_MergeMesh() {
			CString path = Stack_PeekString().Upper();
			theApp.MergeMesh(path);
			//PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}



		__declspec(dllexport) void Extern_MergeZen() {
			CString path = Stack_PeekString().Upper();
			theApp.MergeZen(path);
			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}


		__declspec(dllexport) void Extern_SaveWorld(int type) {
			CString path = Stack_PeekString().Upper();
			theApp.SaveWorld(path, type);
			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}


		__declspec(dllexport) void Extern_SaveVobTree() {
			CString path = Stack_PeekString().Upper();
			theApp.SaveVobTree(path);
		}


		__declspec(dllexport) void Extern_OpenVobTree(bool globalInsert) {
			CString path = Stack_PeekString().Upper();
			theApp.OpenVobTree(path, globalInsert);
		}

		__declspec(dllexport) void Extern_BlockMouse(bool disabled) {

			if (disabled)
			{
				theApp.BlockMouseClick(1000 * 3600);
			}
			else
			{
				theApp.BlockMouseClick(0);
			}
		}


		__declspec(dllexport) void Extern_RemoveVob(uint vob) {

			void* ptr = (void*)vob;

			if (!IsValidZObject(ptr))
			{
				MessageBox(0, "Bad vob pointer in Extern_RemoveVob!", 0, 0);
				OutFile("Bad vob pointer in Extern_RemoveVob: " + AHEX32((int)ptr), false);
				return;
			}


			OutFile("Extern_RemoveVob: " + AHEX32(vob), false);
			theApp.RemoveVob	((zCVob*)vob);
		}


		__declspec(dllexport) void Extern_Exit() {

			theApp.isExit = true;
			theApp.options.Save();
			gameMan->ExitGame();
		}

		__declspec(dllexport) void Extern_ConnectWP() {

			theApp.ConnectWP();
		}

		__declspec(dllexport) void Extern_DisconnectWP() {

			theApp.DisconnectWP();
		}


		__declspec(dllexport) void Extern_PlaySound() {

			CString name = Stack_PeekString().Upper();
			PlaySoundGame(ToStr name);
		}

		__declspec(dllexport) void Extern_StopAllSounds() {

			if (zsound) zsound->StopAllSounds();
		}


		__declspec(dllexport) void Extern_ClearMouseClick() {

			//print.PrintRed("Click");
			zinput->ClearKey(MOUSE_LEFT);
			ClearLMB();
		}

		__declspec(dllexport) bool Extern_IsWorldLoaded() {

			return theApp.IsAWorldLoaded();
		}

		__declspec(dllexport) bool Extern_IsWorldCompiled() {

			return (ogame->GetWorld() && ogame->GetWorld()->IsCompiled());
		}




		__declspec(dllexport) void Extern_CreateItem() {

			CString name = Stack_PeekString();
			OutFile("Extern_CreateItem: vob " + A name, true);
			theApp.CreateItem(name);
		}


		__declspec(dllexport) void Extern_CollectTriggerSources() {

			theApp.CollectSourcerTriggerList();
		}


		__declspec(dllexport) void Extern_CreatePFX() {

			CString name = Stack_PeekString();
			OutFile("Extern_CreatePFX: vob " + A name, true);
			theApp.CreatePFX(name);
		}

		__declspec(dllexport) void Extern_ApplyProps() {

			CString props = Stack_PeekString();
			CString currentName = Stack_PeekString();
			CString visual = Stack_PeekString();

			OutFile("Extern_ApplyProps: vob " + A currentName, true);
			theApp.ApplyProps(props, currentName, visual);
		}

		__declspec(dllexport) bool Extern_VobNameExist(bool isWaypoint) {

			CString name = Stack_PeekString();

			return CheckVobNameExist(name, isWaypoint);
		}

		__declspec(dllexport) bool Extern_CheckUniqueNameExist() {

			CString name = Stack_PeekString();

			return CheckUniqNameExist(name);
		}

		__declspec(dllexport) void Extern_CreateWaypoint(bool addToNet, bool autoGenerate) {


			CString name = Stack_PeekString();
			CString vobName = Stack_PeekString();

			theApp.CreateNewWaypointFromNet(name, vobName, addToNet, autoGenerate);
		}

		__declspec(dllexport) void Extern_CreateFreePoint(bool autoGenerate, bool ground) {

			CString className = Stack_PeekString();
			CString vobName = Stack_PeekString();

			theApp.CreateNewFreepoint(className, vobName, autoGenerate, ground);
		}




		__declspec(dllexport) void Extern_CreateNewVobVisual(int dyn, int stat) {

			CString name = Stack_PeekString();
			CString vobName = Stack_PeekString();
			CString visual = Stack_PeekString();

			OutFile("Extern_CreateNewVobVisual: vob " + A vobName, true);
			theApp.CreateNewVob(name, vobName, visual, dyn, stat);
		}



		__declspec(dllexport) float Extern_GetBBox(int mode) {

			return GetBboxValue(mode);
		}

		__declspec(dllexport) void Extern_SetBBox(int x, int y, int z) {

			SetBBoxValue(x, y, z);
		}

		__declspec(dllexport) void Extern_CompileWorld(int mode) {

			auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
			load(1);
			theApp.DoCompileWorld(mode);

			(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();

			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}



		__declspec(dllexport) void Extern_ComplileLight(int mode, int radius) {

			auto load = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
			load(2);

			theApp.DoCompileLight(mode, radius);

			(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();


			PlaySoundGame(ToStr "CS_IAI_ME_ME");


		}


		__declspec(dllexport) void Extern_GetSettingStr()
		{
			CString name = Stack_PeekString();

			CString result = theApp.options.GetVal(name);

			Stack_PushString(result);
		}

		__declspec(dllexport) int Extern_GetSetting()
		{
			CString name = Stack_PeekString();

			return theApp.options.GetIntVal(name);
		}

		__declspec(dllexport) void Extern_SetSetting(int value)
		{
			CString name = Stack_PeekString();

			theApp.options.SetIntVal(name, value);
			theApp.options.Apply();

		}

		__declspec(dllexport) void Extern_SetSettingStr()
		{
			CString key = Stack_PeekString();
			CString value = Stack_PeekString();

			theApp.options.SetStringVal(key, value);
			theApp.options.Apply();

		}

		__declspec(dllexport) void Extern_AnalyseWaynet()
		{
			CString wayNetMsg = OnWorldAnalysewaynet();

			Stack_PushString(wayNetMsg);
		}





		__declspec(dllexport) void Extern_ResetWorld() {
			theApp.Reset();
		}



		__declspec(dllexport) void Extern_SelectVobSync(void* ptr) {

			zCVob* pVob = (zCVob*)ptr;


			if (!IsValidZObject(ptr))
			{
				MessageBox(0, "Bad vob pointer in Extern_SelectVobSync!", 0, 0);
				OutFile("Bad vob pointer in Extern_SelectVobSync: " + AHEX32((int)ptr), false);
				return;
			}



			OutFile("Extern_SelectVobSync: " + AHEX32((int)pVob), false);

			if (pVob)
			{

				theApp.SetSelectedVob(pVob, "Extern_SelectVobSync");
				theApp.SelectObject(theApp.pickedVob);
			}

		}



		__declspec(dllexport) void Extern_SelectVob(void* ptr) {

			zCVob* pVob = (zCVob*)ptr;


			if (!IsValidZObject(ptr))
			{
				MessageBox(0, "Bad vob pointer in Extern_SelectVob!", 0, 0);
				OutFile("Bad vob pointer in Extern_SelectVob: " + AHEX32((int)ptr), false);
				return;
			}

			OutFile("Extern_SelectVob: " + AHEX32((int)pVob), false);

			if (pVob)
			{



				zCVob* movvob = ogame->GetCamera()->connectedVob;

				zVEC3 vob_pos = pVob->GetPositionWorld();
				zVEC3 cam_vec = movvob->GetAtVectorWorld();



				movvob->SetPositionWorld(vob_pos - cam_vec*(float)400);
				theApp.SetSelectedVob(pVob, "Extern_SelectVob");
				theApp.SelectObject(theApp.pickedVob);

			}

		}


		__declspec(dllexport) void Extern_SetCameraPos() {

			float x, y, z;

			x = Stack_PeekFloat();
			y = Stack_PeekFloat();
			z = Stack_PeekFloat();

			if (ogame && ogame->GetCamera())
			{
				ogame->GetCamera()->connectedVob->SetPositionWorld(zVEC3(x, y, z));
				ogame->GetCamera()->connectedVob->ResetXZRotationsWorld();
			}
		}


		__declspec(dllexport) void Extern_SetToKeyPos(int key) {
			theApp.SetCurrentKey(key);
			theApp.SetToKeyPos();
		}


		__declspec(dllexport) void Extern_OnKeyRemove() {
			theApp.OnKeyRemove();
		}


		__declspec(dllexport) void Extern_OnAddKey(int mode) {
			return theApp.OnAddMoverKey(mode);
		}



		__declspec(dllexport) void Extern_SetCollTrigger(int type, int value) {

			if (theApp.moverVob)
			{
				if (type == 0)
				{
					theApp.moverVob->SetCollDetDyn(value);
				}

				if (type == 1)
				{
					theApp.moverVob->SetCollDetStat(value);
				}
			}
		}

		__declspec(dllexport) void Extern_SendTrigger(int index) {
			theApp.SendTrigger(index);
		}

		__declspec(dllexport) int Extern_GetCurrentKey() {
			return theApp.m_kf_pos;
		}

		__declspec(dllexport) int Extern_GetMaxKey() {

			if (theApp.moverVob)
			{
				return theApp.moverVob->keyframeList.GetNumInList();
			}
			return 0;
		}



		__declspec(dllexport) void Extern_SetTime(int hour, int min) {
			SetTime(hour, min);
		}


		__declspec(dllexport) void Extern_RenderSelectedVob() {

			CString visual = Stack_PeekString();
			AddVobToRender(visual, false);
		}
		
		__declspec(dllexport) void Extern_RenderPFX() {
			CString visual = Stack_PeekString();
			pfxManager.RenderPFX(visual);
		}

		__declspec(dllexport) void Extern_KillPFX() {
			pfxManager.RemovePFX();
		}


		__declspec(dllexport) void Extern_KillPreviewItem() {
			AddVobToRender("", true);
		}

		__declspec(dllexport) void Extern_RenderItem() {
			CString visual = Stack_PeekString();
			AddVobToRender(visual, true);
		}


		__declspec(dllexport) void Extern_ToggleMusic(int mod) {

			ToggleMusic(mod);
		}


		__declspec(dllexport) void Extern_SendNewKeyPreset(int code, int mod) {

			CString key = Stack_PeekString();
			keys.UpdateKey(key, code, mod);
		}

		__declspec(dllexport) void Extern_GetIniKey() {

			CString key = Stack_PeekString();
			CString result = keys.GetIniValue(key);

			Stack_PushString(result);

		}

		__declspec(dllexport) void Extern_ClearLangStrings() {

			ClearLangStrings();

		}


		__declspec(dllexport) void Extern_ResetKeysDefault() {
			keys.ResetDefault();

		}


		__declspec(dllexport) void Extern_GetClassFields(bool isNew) {

			theApp.SearchFillVobClass(Stack_PeekString(), isNew);

		}

		__declspec(dllexport) int Extern_SearchVobs(bool derived, int type) {

			int selectedCount = Stack_PeekInt();

			return theApp.SearchFillVobClass(derived, type, selectedCount);

		}

		__declspec(dllexport) void Extern_GetConvertSubClasses() {

			theApp.SearchGetSubClasses(Stack_PeekString());

		}
		

		__declspec(dllexport) void Extern_AddConvertVob() {

			theApp.SearchDoConvert(Stack_PeekString());

		}


		__declspec(dllexport) void Extern_GetInstanceProps() {

			theApp.GetPFXInstanceProps(Stack_PeekString());
		}

		__declspec(dllexport) void Extern_GetAllPfx() {

			theApp.GetAllPfx();
		}

		__declspec(dllexport) void Extern_UpdatePFXField() {

			theApp.UpdatePFXField();
		}
		
	}

}