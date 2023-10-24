// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	// ������� ���������� �� C#


	extern "C"
	{

		__declspec(dllexport) int SpacerIsActive = 1;



		__declspec(dllexport) void Extern_LoadWorld() {


			CString path = Stack_PeekString().Upper();
			int type = Stack_PeekInt();

			theApp.LoadWorld(path, type);

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
			
			theApp.ToggleGame();
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

			int polysSort = Stack_PeekInt();

			CString path = Stack_PeekString().Upper();
			theApp.SaveWorld(path, type, polysSort);
			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}


		__declspec(dllexport) void Extern_SaveMesh() {
			CString path = Stack_PeekString().Upper();

			theApp.ExportWorldMesh(path);
			PlaySoundGame(ToStr "CS_IAI_ME_ME");
		}


		__declspec(dllexport) void Extern_SaveVobTree() {
			CString path = Stack_PeekString().Upper();
			theApp.SaveVobTree(path);
		}


		__declspec(dllexport) void Extern_OpenVobTree(bool globalInsert, bool insertNearCamera) {
			CString path = Stack_PeekString().Upper();
			theApp.OpenVobTree(path, globalInsert, insertNearCamera);
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

			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
			theApp.RemoveVob	((zCVob*)vob);
			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
		}


		__declspec(dllexport) void Extern_MakeGlobalParent(uint vob) {

			void* ptr = (void*)vob;


			if (!IsValidZObject(ptr))
			{
				MessageBox(0, "Bad vob pointer in Extern_MakeGlobalParent!", 0, 0);
				OutFile("Bad vob pointer in Extern_MakeGlobalParent: " + AHEX32((int)ptr), false);
				return;
			}



			OutFile("Extern_MakeGlobalParent: " + AHEX32(vob), false);
			theApp.MakeGlobalParent((zCVob*)vob);
		}

		__declspec(dllexport) void Extern_RemoveAsParent(uint vob) {

			void* ptr = (void*)vob;


			if (!IsValidZObject(ptr))
			{
				MessageBox(0, "Bad vob pointer in Extern_RemoveAsParent!", 0, 0);
				OutFile("Bad vob pointer in Extern_RemoveAsParent: " + AHEX32((int)ptr), false);
				return;
			}



			OutFile("Extern_RemoveAsParent: " + AHEX32(vob), false);
			theApp.Extern_RemoveAsParent((zCVob*)vob);
		}


		__declspec(dllexport) void Extern_CleanGlobalParent() {
			theApp.CleanGlobalParent();
		}


		__declspec(dllexport) int Extern_CanBeGlobalParent(uint vob) {

			void* ptr = (void*)vob;

			if (!IsValidZObject(ptr))
			{
				return 0;
			}

			zCVob* pVob = (zCVob*)vob;

			if (pVob)
			{
				if (
					pVob->CastTo<zCVobLight>()

					|| dynamic_cast<zCVobWaypoint*>(pVob)
					|| dynamic_cast<oCWaypoint*>(pVob)
					|| dynamic_cast<zCVobSpot*>(pVob)
					|| dynamic_cast<zCVobStartpoint*>(pVob)
					|| dynamic_cast<oCItem*>(pVob)
					|| dynamic_cast<zCVobLight*>(pVob)
					|| dynamic_cast<zCVobLevelCompo*>(pVob)
					|| dynamic_cast<zCZone*>(pVob)
					|| dynamic_cast<zCVobSound*>(pVob)
					|| dynamic_cast<zCEffect*>(pVob)
					
					|| theApp.currentVobRender == pVob
					|| pfxManager.testVob == pVob
					|| pVob->IsPFX()

					)
				{
					return 0;
				}
			}

			return 1;
		}
		


		__declspec(dllexport) void Extern_Exit() {

			theApp.isExit = true;
			theApp.options.Save();
			theApp.restorator.Reset();
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

		__declspec(dllexport) void Extern_PlayMusic() {

			CString name = Stack_PeekString().Upper();
			theApp.PlayMusic(ToStr name);
		}

		__declspec(dllexport) void Extern_StopAllSounds() {

			if (zsound) zsound->StopAllSounds();
		}


		__declspec(dllexport) void Extern_ClearMouseClick() {

			//print.PrintRed("Click");
			zinput->ClearKey(MOUSE_LEFT);
			ClearLMB();
		}

		__declspec(dllexport) int Extern_IsWorldLoaded() {

			return theApp.IsAWorldLoaded();
		}

		__declspec(dllexport) int Extern_IsWorldCompiled() {

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

			int posChanged = Stack_PeekInt();

			CString props = Stack_PeekString();
			CString currentName = Stack_PeekString();
			CString visual = Stack_PeekString();

			OutFile("Extern_ApplyProps: vob " + A currentName, true);
			theApp.ApplyProps(props, currentName, visual, posChanged);
		}

		__declspec(dllexport) int Extern_VobNameExist(bool isWaypoint) {

			CString name = Stack_PeekString();

			return CheckVobNameExist(name, isWaypoint);
		}

		__declspec(dllexport) int Extern_CheckUniqueNameExist() {

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

		

		__declspec(dllexport) void Extern_RestorePosition(void* ptr) {

			zCVob* pVob = (zCVob*)ptr;


			if (!IsValidZObject(ptr))
			{
				//MessageBox(0, "Bad vob pointer in Extern_SelectVobSync!", 0, 0);
				OutFile("Bad vob pointer in Extern_RestorePosition: " + AHEX32((int)ptr), false);
				return;
			}

			theApp.restorator.RestoreVobPos(pVob);
		}


		__declspec(dllexport) void Extern_SelectVobSync(void* ptr) {

			zCVob* pVob = (zCVob*)ptr;

			
			if (!IsValidZObject(ptr))
			{
				//MessageBox(0, "Bad vob pointer in Extern_SelectVobSync!", 0, 0);
				OutFile("Bad vob pointer in Extern_SelectVobSync: " + AHEX32((int)ptr), false);
				return;
			}
			



			OutFile("Extern_SelectVobSync: " + AHEX32((int)pVob), false);

			if (pVob)
			{
				if (theApp.nextInsertionIsTempPfx)
				{
					//cmd << "Protection" << endl;
					theApp.nextInsertionIsTempPfx = false;
					return;
				}

				theApp.SetSelectedVob(pVob, "Extern_SelectVobSync");
				theApp.SelectObject(pVob);

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
				if (theApp.nextInsertionIsTempPfx)
				{
					theApp.nextInsertionIsTempPfx = false;
				}


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

		__declspec(dllexport) void Extern_SetMoverToKey() {
			
			int index = Stack_PeekInt();

			theApp.SetTriggerToKey(index);
		}

		

		__declspec(dllexport) int Extern_GetCurrentKey() {

			if (theApp.moverVob)
			{
				return theApp.moverVob->GetActKeyframe();
			}
			else
			{
				return 0;
			}
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

		__declspec(dllexport) int Extern_SearchVobs(bool derived, bool hasChildren, int type, int onlyVisualOrName) {

			
			int matchNames = Stack_PeekInt();
			int searchOCItem = Stack_PeekInt();
			int selectedCount = Stack_PeekInt();

			return theApp.SearchFillVobClass(derived, hasChildren, type, selectedCount, onlyVisualOrName, matchNames, searchOCItem);

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

		__declspec(dllexport) void Extern_SetVobListType(int type) {

			theApp.vobListSelectedIndex = type;
		}

		__declspec(dllexport) void Extern_FreezeTime(int enabled) {

			theApp.SetHoldTime(enabled);
		}
		__declspec(dllexport) void Extern_SetRenderMode(int mode) {

			if (mode == 0) {
				zrenderer->SetPolyDrawMode(zRND_DRAW_MATERIAL);
			}
			else
			if (mode == 1) {
				zrenderer->SetPolyDrawMode(zRND_DRAW_MATERIAL_WIRE);
			}
			else
			if (mode == 2) {
				zrenderer->SetPolyDrawMode(zRND_DRAW_FLAT);
			}
			else
			if (mode == 3) {
				zrenderer->SetPolyDrawMode(zRND_DRAW_WIRE);
			}
		}


		__declspec(dllexport) void Extern_SaveVobVisualsUnique() {

			CString path = Stack_PeekString();
			
			theApp.FindVobsVisualsUnique(path);
		}


		__declspec(dllexport) int Extern_VisualIsInVDF() {

			CString vdf = Stack_PeekString();
			CString visualName = Stack_PeekString();
			return theApp.IsVisualInVDF(visualName, vdf);
		}
		

		__declspec(dllexport) int Extern_GetPolysCount() {

			int result = 0;

			if (ogame && ogame->GetWorld() && ogame->GetWorld()->IsCompiled())
			{
				result = ogame->GetWorld()->bspTree.numPolys;
			}

			return result;
		}

		__declspec(dllexport) void Extern_ToggleNoGrass() {

			theApp.ToggleNoGrass();
		}




		__declspec(dllexport) void Extern_ClearMacrosCmd() {

			theApp.Macros_Clean();
		}
		

		__declspec(dllexport) void Extern_RunMacrosCmd() {

			theApp.Macros_Run();
		}


		__declspec(dllexport) void Extern_AddMacrosCmd() {

			MacrosType type = (MacrosType)Stack_PeekInt();


			CString raw = Stack_PeekString();

			CString str = Stack_PeekString();



			theApp.Macros_Add(str, raw, type);
		}


		__declspec(dllexport) void Extern_InsertNewCamera() {

			CString name = Stack_PeekString();

			theApp.camMan.InsertCam(name);
		}
		

		__declspec(dllexport) void Extern_InsertNewSplineKey() {

			theApp.camMan.InsertNewSplineKey();
		}

		__declspec(dllexport) void Extern_InsertNewTargetKey() {

			theApp.camMan.InsertNewTargetKey();
		}

		__declspec(dllexport) void Extern_CameraRun() {

			theApp.camMan.OnRun();
		}
		
		__declspec(dllexport) void Extern_CameraStop() {

			theApp.camMan.OnStop();
		}

		__declspec(dllexport) void Extern_CameraUpdateSpacerTime(int time, int hide) {

			theApp.camMan.timeDurationOnSpacer = time;
			theApp.camMan.hideVisualWhileActive = hide;
		}


		__declspec(dllexport) void Extern_CameraGotoKey(int key) {

			theApp.camMan.OnChangeKey(key);
		}

		

		__declspec(dllexport) int Extern_CameraGetMaxKey() {

			if (theApp.camMan.cur_cam)
			{
				return theApp.camMan.cur_cam->GetNumCamKeys();
			}
			else if (theApp.camMan.tempCam)
			{
				return theApp.camMan.tempCam->GetNumCamKeys();
			}
			else
			{
				return 0;
			}
		}


		__declspec(dllexport) void Extern_RemoveTargetKeyByIndex(int index) {
			theApp.camMan.RemoveTargetKeyByIndex(index);
		}

		__declspec(dllexport) void Extern_RemoveSplineKeyByIndex(int index) {
			theApp.camMan.RemoveSplineKeyByIndex(index);
		}

		__declspec(dllexport) void Extern_SelectTargetKeyByIndex(int index) {
			theApp.camMan.SelectTargetKeyByIndex(index);
		}

		__declspec(dllexport) void Extern_SelectSplineKeyByIndex(int index) {
			theApp.camMan.SelectSplineKeyByIndex(index);
		}

		__declspec(dllexport) void Extern_InsertPosKeyAtIndex(int index) {
			theApp.camMan.InsertPosKeyAtIndex(index);
		}

		__declspec(dllexport) void Extern_InsertTargetKeyAtIndex(int index) {
			theApp.camMan.InsertTargetKeyAtIndex(index);
		}

		
		__declspec(dllexport) int Extern_CanEditBboxCurrentVob() {

			int result = 1;


			if (auto pVob = theApp.GetSelectedVob())
			{
				auto pOcVob = pVob->CastTo<oCVob>();
				auto pWp = pVob->CastTo<zCVobWaypoint>();
				auto pSpot = pVob->CastTo<zCVobSpot>();

				
				if ((pVob->GetVisual() || pOcVob || pSpot || pWp))
				{
					result = 0;
				}
			}
			else
			{
				result = 0;
			}

			return result;
		}


		__declspec(dllexport) void Extern_SearchVobList() {
			FindNearestVobs();
		}
		
		
		__declspec(dllexport) void Extern_FillMatListByFilterName() {

			CString name = Stack_PeekString();
			mf.FillMatListByFilterName(name);
		}

		__declspec(dllexport) int Extern_FillMat_GetCurrentMat_FilterIndex() {
			
			if (mm.matSelectedInTree)
			{
				return mm.matSelectedInTree->libFlag;
			}
			return 0;
		}

		__declspec(dllexport) int Extern_FillMat_GetCurrentMat_GroupIndex() {

			if (mm.matSelectedInTree)
			{
				return mm.matSelectedInTree->matGroup;
			}
			return 0;
		}

		__declspec(dllexport) void Extern_FillMat_ApplyFilterAndGroup(int filter, int gr, int followFilter, int changeFilter) {

			if (mm.matSelectedInTree)
			{
				auto mat = mm.matSelectedInTree;

				mat->SetMatGroup((zTMat_Group)gr);
				mat->libFlag = filter;

				auto AddProps = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

				Stack_PushString("");
				Stack_PushString("");

				AddProps();


				if (followFilter || !changeFilter)
				{
					zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, FALSE, 0);

					arch->SetStringEOL(zSTRING("\n"));
					arch->WriteObject(mat);
					zSTRING arcString;
					arch->GetBufferString(arcString);

					theApp.SetProperties(arcString, "zCMaterial");
					arch->Close();
					zRELEASE(arch);

					mm.OnSelectMaterial(mat);
				}

				
			}
			else
			{
				print.PrintRed("No selected mat");
			}
			
		}


		__declspec(dllexport) void Extern_SelectMat(void* ptr) {

			zCMaterial* pMat = (zCMaterial*)ptr;


			if (!IsValidZObject(ptr))
			{
				MessageBox(0, "Bad material pointer in Extern_SelectMat!", 0, 0);
				OutFile("Bad material pointer in Extern_SelectMat: " + AHEX32((int)ptr), false);
				return;
			}


			OutFile("Extern_SelectMat: " + AHEX32((int)pMat), false);

			if (pMat)
			{
				theApp.SetSelectedVob(NULL);
				

				auto sel = mm.GetCurrentSelection();

				if (sel && sel->mat && sel->mat != pMat)
				{
					if (sel->mat->GetName() != pMat->GetName())
					{
						//cmd << "cur: " << sel->mat->GetName() << " new: " << pMat->GetName() << endl;
						//mm.CleanSelection();
					}
					
				}
				mm.SelectMaterial(pMat);
			}
		}
		
		__declspec(dllexport) void Extern_FilterMat_SaveFilters()
		{
			if (mf.init)
			{
				mf.SaveFilterList();
			}
		}

		__declspec(dllexport) void Extern_FillMat_AddNewFilter()
		{
			CString name = Stack_PeekString();
			mf.AddNewFilter(name);
		}

		__declspec(dllexport) void Extern_MatFilter_SaveCurrentFilter()
		{
			int index = Stack_PeekInt();
			mf.SaveCurrentFilter(index);
		}

		__declspec(dllexport) void Extern_MatFilter_RenameFilter()
		{
			int index = Stack_PeekInt();
			CString name = Stack_PeekString();
			mf.OnRenameFilter(index, name);
		}
		
		__declspec(dllexport) void Extern_SetSearchVobName()
		{
			CString replaceZenPath = Stack_PeekString();
			CString name = Stack_PeekString();
			
			
			theApp.search.searchVobNameGlobal = name;
			theApp.search.replaceZenPath = replaceZenPath;
			
		}


		__declspec(dllexport) void Extern_MatFilter_UpdateTextureShowSettings()
		{
			mf.bResizeSmallTextures = Stack_PeekInt();
			mf.bUseCenterAligment = Stack_PeekInt();
			mf.bUseAlphaChannels = Stack_PeekInt();

			mf.removeTextureCacheNext = true;


			if (mm.matSelectedInTree)
			{
				mm.ExtractTexture(mm.matSelectedInTree);
			}
			

		}
		
		__declspec(dllexport) void Extern_Filter_CreateNewMaterial()
		{
			CString name = Stack_PeekString();
			int index = Stack_PeekInt();
			mf.CreateNewMat(name, index);
		}

		__declspec(dllexport) void Exter_MatFilter_SearchMatByName()
		{
			CString name = Stack_PeekString();
			mf.SearchMaterialByName(name);
		}


		__declspec(dllexport) void Extern_SetLocatorItemType()
		{
			int type = Stack_PeekInt();
			theApp.itemsLocator.searchType = type;
		}

		__declspec(dllexport) void Extern_MatFilter_RemoveFilterByIndex()
		{
			int index = Stack_PeekInt();
			mf.RemoveFilterByIndex(index);
		}

		__declspec(dllexport) int Extern_GetOption_CanCompileWorldAgain()
		{
			return theApp.options.GetIntVal("canCompileWorldAgain");
		}

		__declspec(dllexport) void Extern_SetVobPickFilter(int index)
		{
			theApp.filterPickVobIndex = index;

			//print.PrintRed(Z theApp.filterPickVobIndex);
		}

		__declspec(dllexport) void Extern_Light_AddPreset(const char* presetName)
		{
			zCVobLightPreset* preset = zNEW(zCVobLightPreset);

			preset->presetName = presetName;

			preset->lightData.isStatic = Stack_PeekBool();
			preset->lightData.lightQuality = Stack_PeekInt();
			preset->lightData.range = Stack_PeekInt();

			for (int i = 0, colorsCount = Stack_PeekInt(); i < colorsCount; ++i)
				preset->lightData.colorAniList.InsertAtPos(zCOLOR::FromARGB(Stack_PeekInt()), 0);

			preset->lightData.colorAniFPS = Stack_PeekFloat();
			preset->lightData.colorAniSmooth = Stack_PeekBool();

			for (int i = 0, rangeAniScalesCount = Stack_PeekInt(); i < rangeAniScalesCount; ++i)
				preset->lightData.rangeAniScaleList.InsertAtPos(Stack_PeekFloat(), 0);

			preset->lightData.rangeAniFPS = Stack_PeekFloat();
			preset->lightData.rangeAniSmooth = Stack_PeekBool();

			zCVobLight::lightPresetList.Insert(preset);
		}

		__declspec(dllexport) void Extern_Light_DeletePreset(const char* presetName)
		{
			for (int i = 0; i < zCVobLight::lightPresetList.GetNumInList(); ++i)
			{
				zCVobLightPreset* preset = zCVobLight::lightPresetList[i];

				if (preset->presetName != presetName)
					continue;

				zRELEASE(preset);
				zCVobLight::lightPresetList.RemoveIndex(i);

				break;
			}
		}

		__declspec(dllexport) void Extern_Light_QueryPresetData(const char* presetName)
		{
			for (int i = 0; i < zCVobLight::lightPresetList.GetNumInList(); ++i)
			{
				zCVobLightPreset* preset = zCVobLight::lightPresetList[i];

				if (preset->presetName != presetName)
					continue;

				Stack_PushBool(preset->lightData.rangeAniSmooth);
				Stack_PushFloat(preset->lightData.rangeAniFPS);

				for (int j = preset->lightData.rangeAniScaleList.GetNumInList() - 1; j >= 0; --j)
					Stack_PushFloat(preset->lightData.rangeAniScaleList[j]);

				Stack_PushInt(preset->lightData.rangeAniScaleList.GetNumInList());

				Stack_PushBool(preset->lightData.colorAniSmooth);
				Stack_PushFloat(preset->lightData.colorAniFPS);

				if (!preset->lightData.isStatic)
				{
					for (int j = preset->lightData.colorAniList.GetNumInList() - 1; j >= 0; --j)
						Stack_PushInt(preset->lightData.colorAniList[j].GetARGBDword());

					Stack_PushInt(preset->lightData.colorAniList.GetNumInList());
				}
				else
				{
					Stack_PushInt(preset->lightData.lightColor.GetARGBDword());
					Stack_PushInt(1);
				}

				Stack_PushInt(preset->lightData.range);
				Stack_PushInt(preset->lightData.lightQuality);
				Stack_PushBool(preset->lightData.isStatic);

				GetProcAddress(theApp.module, "UpdateLightPresetView")();

				break;
			}
		}

		__declspec(dllexport) void Extern_Light_SavePresets()
		{
			zCVobLight::SaveLightPresets();
		}

		__declspec(dllexport) void Extern_Light_DynamicCompile(bool toggle)
		{
			theApp.dynLightCompile = toggle;

			if (theApp.dynLightCompile && theApp.vobLightSelected)
				theApp.DoCompileLight(1, 15);
		}
	}

}