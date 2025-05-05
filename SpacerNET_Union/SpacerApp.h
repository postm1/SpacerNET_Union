// Supported with union (c) 2020 Union team
// Union HEADER file




namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct SpacerApp
	{
		// handle of main form of c#
		HWND mainWin;
		HWND vobsWin;
		// library c#
		HMODULE module;

		COption spcOpt;
		CSettings options;
		bool visualEditorActive;
		bool pluginsChecked;
		bool filterMatWindowActive;
		
		zCArray<zSTRING> whiteArrayPlugins;
		zCArray<zSTRING> blackArrayPlugins;
		ItemsLocator itemsLocator;
		AB_NoGrass nograss;
		GrassPlacer gp;
		AB_Debug debug;

		int isBboxChangeMod;

		zCArray<zCPolygon*>* leakPolyList;
		

		Common::Map<CString, RespawnEntry*> respawnShowList;
		zCArray<zSTRING> respFuncList;
		void ClearRespList()
		{
			showRespawnOnVobs = false;
			ClearSpawnFuncList();
			DeleteAndClearMap(respawnShowList);
		}

		void AddRespListFunc(zSTRING func)
		{
			if (!respFuncList.IsInList(func))
			{
				respFuncList.InsertEnd(func);
			}
		}

		void ClearSpawnFuncList()
		{
			respFuncList.DeleteList();
		}

		void CollectRestList()
		{
			for (int i = 0; i < respFuncList.GetNumInList(); i++)
			{
				auto func = respFuncList.GetSafe(i);

				if (parser->GetSymbol(func))
				{
					parser->CallFunc(func.Upper());
				}
			}

			showRespawnOnVobs = TRUE;
		}


		struct
		{
			zCMover* moverVob;
			zCTrigger* triggerVob;
			zCTriggerBase* triggerBaseVob;
			int m_kf_pos;
			zCVob* event_sourcevob;
		};
		struct 
		{
			zCVob* cur_vob;
			zCVob* cur_vob_convert;
			zSTRING lastClass;
			CString searchVobNameGlobal;
			CString replaceZenPath;
			
		} search;

		struct
		{
			zCMesh*	s_pLightSphereMesh;
			zCVobLight* vobLightSelected;
			zCVobLight* pLightDx11;
		};

		struct
		{
			bool isNewBalanceMod;
		};

		struct
		{
			// draw sphere mod
			eSphereDrawMode sphereDrawMode = DRAW_NONE;

			// wire color
			zCOLOR sphereWireColor = zCOLOR(170, 170, 170, 255);

			// show radius as text
			BOOL bDrawRadiusValue = TRUE;

			// use tilt of a sphere
			BOOL bUseSphereTrafo = TRUE;

			// string format for output radius as text
			string radiusTextFmt = "R: %s";

		} lightSphereSettings;

		struct
		{
			int zSpyLevel;
			bool zSpyActive;
			bool insertBlockedTemp;

			bool isMesh;
			bool isMerged;
			bool isMergingMeshNow;
			bool isLoadingMeshNow;
			int filterPickVobIndex;
			// selected current vob
			zCVob* pickedVob;
			zCVob* globalParent;
			

			zCVob* currentVobRender;
			zCVob* currenItemRender;


			zCVob* bboxMinsVob;
			zCVob* bboxMaxsVob;

			TurnWpMode turnWpMode;

			// vob to copy
			zCVob* vobToCopy;
			// needed to draw the bbox of the selected vob
			bool pickedVobBBoxMode;
			bool isVobParentChange;
			bool dynLightCompile;
			bool wasCopiedPressed;
			bool showVobVisualInfo;
			bool holdTime;

			bool levelReady;
			bool treeIsReady;
			bool selectNextVob;
			bool selectNextVobForce;
			bool nextInsertBlocked;
			bool pickUnshareShow;
			
			bool spacerWasInit;

			bool useSortPolys;
			bool dataFilled;
			bool hideWindows;
			bool hideWindowsForce;
			bool isExit;
			bool selectedWpForCreateIsBlocked;
			bool firstZenLoad;
			bool firstTimeZenSaved;
			int vobListSelectedIndex;
			bool isGrattControlActive;
			bool isNextCopyVobInsertNear;
			bool nextInsertionIsTempPfx;
			bool showRespawnOnVobs;
			int showRespawnOnVobsRadius;
			bool bDebugSpacerLoadMesh;
			bool showScreenInfo;
			bool cameraMoveWithVobActive;
			bool showAfterWorldCompileMessage;
			zCArray<zCVob*> compareDynList;
			zCArray<zCVob*> compareVobsAll;
			std::map<std::string, int> compareCatalogVisualsMap;

			zEVobMotionHeading rotMod;

			zCVob* floorVob;
			zCVob*		pickedWaypoint2nd;
			zCVob*		pickedWaypoint;
			zCVob*		selectedWaypointForNet;

			zCView* pViewVobInfo = NULL;
			zCView* pViewVobInfoBack = NULL;


			zCObject* current_object;


			zCList<zCVob> sourceVobList;
			zCList<zCVob> targetVobList;

			zCTree<zCVob>* treeToCopy;
			zCArray<zCVob*> mobInterSlotsVobs;


			zCArray<zSTRING> modelsStrVDF;
			zCArray<zSTRING> modelsStrWORK;
			int mouseClicksBlockedTime;
			int mouseWheelKeyCodeG1;
			zCView* viewInfo;
			//spcCObjPropertyList	propList;

			struct
			{
				bool g_bIsPlayingGame;
				bool g_bInsertNPCs;
			};


			struct
			{
				int type;
				int startNumber;
				zSTRING allName;
				zSTRING prefixName;
			} renameOptions;

			SpacerPickTry pickTryEntry;
			

			Array<zCVob*> SelectedVobs;

			ActionRestore restorator;
			CameraManager camMan;

			zCObjPresetLib presetsLib;
		};

		struct
		{
			callInt2Func toggleUIElement;
			callVoidFunc getShaString;
			callVoidFunc Fill_MatFilter_Filters;
			callVoidFunc Clear_MatFilter_Filters;
			callVoidFunc AddMatByMatFilterName;
			callVoidFunc MatFilter_SelectFilterByIndex;
			callVoidFunc MatFilter_SelectMaterialByAddr;
			callVoidFunc MatFilter_ToggleWindow;
			callVoidFunc MatFilter_SetTextureColor;
			callVoidFunc MatFilter_SendTexture;
			callVoidFunc MatFilter_UpdateTextureSize;
			callVoidFunc MatFilter_Clear;
			callVoidFunc MatFilter_UpdateTextureBit;
			callVoidFunc MatFilter_UpdateTextureAlphaInfo;
			callVoidFunc MatFilter_OnCreateNewMat;
			callVoidFunc MatFilter_AddMatInSearchByName;
			callVoidFunc MatFilter_AddCurrentFilterIndexToSave;
			callVoidFunc MatFilter_SetEmptyTexture;
			callVoidFunc MatFilter_SetTextureBigDontRender;

			
		} exports;


		struct UpdateMatrix
		{
			zCVob* lastSelectedVob;
			int timeSince;
			bool active;

			void ResetMatrixUpdate()
			{
				active = false;
				timeSince = 0;
				lastSelectedVob = NULL;
			}

			void OnUpdateMatrix(zCVob* pVob)
			{
				timeSince = 300;
				active = true;
				lastSelectedVob = pVob;
			}

			void UpdateTimers();
			void CallUpdate();

		} updateMatrix;

		void BlockMouseClick(int time);
		bool IsClicksBlocked();
		void PluginLoop();
		void KeysLoop();
		void RenderPortals();
		void RemoveMobInterHelpVobs();
		void HideMobInterHelpVobs();
		void CheckForBadPlugins();
		void RenderStartScreen();
		SpacerApp();
		void SetExportsFuncs();
		zCVob* GetSelectedVob();
		void SetSelectedVob(zCVob * vob, zSTRING funcName = "");
		void PrepareBboxVobs(zCVob * vob);
		void SetToKeyPos();
		void TriggerLoop();
		void ShowVobInfo();
		void ExtractVisualInfoShow(zCVisual* visual, int& start, int startX, int& longestLine, zSTRING& textToCopy);
		void Trigger_DrawKey(int key, zVEC3 pos);
		void OnKeyRemove();
		void RemoveAllMoverKeysWithSavePosition();
		void RemoveAllMoverKeysWithSetZero();
		void UpdateGrattController();
		void ManagerGrattController();
		void SendTrigger(int actionIndex);
		void SetTriggerToKey(int key);
		void OnAddMoverKey(int mode);
		void SetCurrentKey(int key);
		void ToggleNoGrass();
		void ApplyProps(char* str, char* name);
		SpacerWorkMode GetPickMode();
		void PreRender();
		void RenderSelectedVobBbox();
		void RenderDX11_AmbientLight();
		void RenderDX11_RemoveAmbientLight();
		bool IsDx11Active();
		void RenderDx11_Bbox(zCVob* vob);
		void RenderDx11_Pivot(zCVob* vob);
		void RemoveTargetListTrigger();
		void CollectTargetListTrigger();
		void CollectSourcerTriggerList();
		void AfterRender();
		void PickMaterial();
		void PickVob();
		void PickVobNew(bool ctrlUsed);
		zCVob* TrySpherePick();
		void PickVobFilter();
		void PickVobInvisible(zVEC3 start, zVEC3 ray);
		zBOOL PickSceneNew(zCCamera& cam, int xscr, int yscr, zREAL rayLength);
		void ToggleWP();
		void ConnectWP();
		void DisconnectWP();
		void PlayMusic(CString name);
		int GetLightPresetIdx(const char* presetName);
		void UpdateLightPresetData(zCVobLightData& lightData);
		void UpdateLightPresetView(zCVobLightData& lightData);
		zSTRING GenerateWPName(zSTRING baseName);
		void Loop();
		void SpecialWindowsKeys();
		void CallUIAction(SpacerNetInterfaceAction action);
		void SetMover();
		bool GameFocused();
		void ToggleWindowsVisible();
		void SetHoldTime(int enabled);
		zCWorld * GetWorld(zBOOL getEmptyWorldToo);
		bool TryPickMouse();
		bool IsAWorldLoaded();
		zVEC2 GetMousePosVirt();
		void ExportWorldMesh(zSTRING worldName);
		void FindVobsVisualsUnique(CString path);
		int IsVisualInVDF(CString visualName, CString vdfName);
		void Reset();
		void AddVobToFavorite(zCVob* pVob);
		void SetProperties(zSTRING & propString, zSTRING classNameVob);

		void SelectObject(zCObject * object, bool clearInput = true, bool isMatrixUpdate = false);

		void ApplyProps(CString str, CString nameCurrent, CString visualStr, int posChanged);

		void WorldPreLoad();
		void WorldAfterLoad();

		void OnLevelReady();

		void OnDynamicUpdateVob(zCVob * vob);

		void OnCreateVob(zCVob * vob, bool select);

		void OnRemoveVob(zCVob * vob);

		void RemoveVob(zCVob* pVob, bool useSelect=true);
		void RemoveAllChilds(zCVob* pVob);
		void MakeGlobalParent(zCVob* pVob);
		void Extern_RemoveAsParent(zCVob* pVob);
		void CleanGlobalParent();



		void CreateNewWaypoint(zCVob * newvob, bool addToNet);
		void CreateCameraPresetsList();

		void CreateNewWaypointFromButtonClick();

		void CreateNewWaypointFromNet(CString classNamePtr, CString vobNamePtr, bool addToNet, bool autoGenerate);


		void CreateNewFreepoint(CString classNameChar, CString nameVob, bool autoName, bool ground);

		void CreateNewVob(CString classNamePtr, CString vobNamePtr, CString visual, int dyn, int stat, int isStaticVob = 0);

		void CreatePFX(CString name);

		bool CheckForUniqVobs(CString className);

		oCItem* CreateItem(CString name);

		zCVobLight* CreateLightVob(CString vobName, CString presetName);

		void CreateItemsList();

		void CreatePFXList();

		void CreateMobNameList();

		void CreateVisualsList();

		void CreateSoundList();

		void BuildTree();

		void RebuiltWaypoints();

		void DoCompileLight(int type, int radius);

		void DoCompileWorld(int type);
		void ClearLeakPolys();
		void CollectLeakPolys();

		void SaveVobTree(zSTRING path);

		zCVob* GetParentVob(zCVob * candidate);

		void OpenVobTree(zSTRING path, bool globalInsert, bool insertNearCamera);



		void SaveFile(zSTRING worldName, int type);

		void HandleWorldBeforeSave();
		
		void HandleWorldAfterSave();

		void HandleUpperCase();

		void RemoveBadLevelCompoVisual();

		void SaveWorld(zSTRING worldName, int type, int polysSort);

		void MergeMesh(CString worldName);

		void LoadMesh(zSTRING worldName);

		void MergeZen(zSTRING worldName, bool isMacro=false);

		void LoadWorld(zSTRING worldName, int type, bool useMacros=false);


		void GameLoop();

		void PlayTheGame();

		void ToggleGame();
		void InitCamera(zVEC3 pos);

		void StopPlay();

		void Init();

		void CreateVobSpacerCameraStartPos();

		void SearchSelectVob(zCVob * pickedVob);

		void SearchFillVobClass(CString vobClass, bool isNewType=false);

		void PrepareSearchEntries(int regExOnFromUI);
		void AddSearchEntry(CString fieldName, CString groupName, TPropEditType type, CString value, TSearchNumberType numberSearchTyp);

		bool FastSearchVob_String(SearchVobEntry& entry, std::string value);

		void SearchGetSubClasses(CString className);


		bool SearchHandleVob(zCVob *& vob, int selectedCount, int onlyVisualOrName);
		bool SearchHandleVobByFastMethod(zCVob*& vob, int selectedCount, int fastSearchTypeMask);

		void SearchDoConvert(CString prop);

		bool SearchHandleConvert(zCVob *& vob);

		int SearchFillVobClass(bool derived, bool hasChildren, int type, int selectedCount, int onlyVisualOrName, int matchNames, int searchOCItem, int radius);

		void GetAllPfx();

		

		void GetPFXInstanceProps(CString pfxName);

		void UpdatePFXField();
		void ToggleInsertBlocked(bool toggle);
		void OnVobMovement(zCVob* vob);
		void TestLoop();

		// macros

		void Macros_Clean();
		void Macros_Run();
		void Macros_Add(CString command, CString raw, MacrosType type);


		//bbox decals for areas
		void ClearBboxDecal();
		void ClearBboxDecalReset();
		void BBoxDecal_OnLevelLoaded();
		void SetBBoxDecalPosAndSize(zCVob* sides[6], zVEC3 points[8], int index, int a, int b, int c, int d);
		void BBoxDecal_Render();

		zCVob* sides[6];
		zVEC3 points[8];
		bool sidesInit = false;
		zCMaterial* decalMat = NULL;
	};

}