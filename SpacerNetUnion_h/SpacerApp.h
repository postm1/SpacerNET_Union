// Supported with union (c) 2020 Union team
// Union HEADER file




namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct SpacerApp
	{
		// handle ГЛАВНОЙ формы c#
		HWND mainWin;
		HWND vobsWin;
		// Библиотека c#
		HMODULE module;

		COption spcOpt;
		CSettings options;
		bool visualEditorActive;
		bool pluginsChecked;
		zCArray<zSTRING> whiteArrayPlugins;
		zCArray<zSTRING> blackArrayPlugins;
		ItemsLocator itemsLocator;
		

		Common::Map<CString, RespawnEntry*> respawnShowList;

		void ClearRespList()
		{

			respawnShowList.Clear();
		}

		struct
		{
			zCMover* moverVob;
			int m_kf_pos;
			zCVob* event_sourcevob;
		};
		struct 
		{
			zCVob* cur_vob;
			zCVob* cur_vob_convert;
			zSTRING lastClass;

		} search;

		struct
		{
			zCMesh*	s_pLightSphereMesh;
			zCVobLight* vobLightSelected;
		};
		struct
		{
			bool isMesh;
			bool isMerged;
			// выбранный воб
			zCVob* pickedVob;
			zCVob* globalParent;

			zCVob* currentVobRender;
			zCVob* currenItemRender;

			TurnWpMode turnWpMode;

			// воб для копирования
			zCVob* vobToCopy;
			// нужно для отрисовки bbox выбранного воба
			bool pickedVobBBoxMode;
			bool isVobParentChange;
			bool dynLightCompile;

			bool levelReady;
			bool treeIsReady;
			bool selectNextVob;
			bool selectNextVobForce;
			bool nextInsertBlocked;
			bool pickUnshareShow;


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
			bool bDebugSpacerLoadMesh;

			zCVob*		pickedWaypoint2nd;
			zCVob*		pickedWaypoint;
			zCVob*		selectedWaypointForNet;


			zCObject* current_object;


			zCList<zCVob> sourceVobList;
			zCList<zCVob> targetVobList;

			zCTree<zCVob>* treeToCopy;


			zCArray<zSTRING> modelsStrVDF;
			zCArray<zSTRING> modelsStrWORK;
			int mouseClicksBlockedTime;
			zCView* viewInfo;
			//spcCObjPropertyList	propList;

			struct
			{
				bool g_bIsPlayingGame;
				bool g_bInsertNPCs;
			};

			SpacerPickTry pickTryEntry;
			

			Array<zCVob*> SelectedVobs;
		};

		void BlockMouseClick(int time);
		bool IsClicksBlocked();
		void PluginLoop();
		void KeysLoop();
		void CheckForBadPlugins();
		void RenderStartScreen();
		SpacerApp();
		zCVob * GetSelectedVob();
		void SetSelectedVob(zCVob * vob, zSTRING funcName = "");
		void SetToKeyPos();
		void OnKeyRemove();
		void UpdateGrattController();
		void ManagerGrattController();
		void SendTrigger(int actionIndex);
		void OnAddMoverKey(int mode);
		void SetCurrentKey(int key);
		void ApplyProps(char* str, char* name);
		SpacerWorkMode GetPickMode();
		void PreRender();
		void RenderSelectedVobBbox();
		bool IsDx11Active();
		void RenderDx11_Bbox(zCVob* vob);
		void RemoveTargetListTrigger();
		void CollectTargetListTrigger();
		void CollectSourcerTriggerList();
		void AfterRender();
		void PickMaterial();
		void PickVob();
		zBOOL SpacerApp::PickSceneNew(zCCamera& cam, int xscr, int yscr, zREAL rayLength);
		void ToggleWP();
		void ConnectWP();
		void DisconnectWP();
		zSTRING GenerateWPName(zSTRING baseName);
		void Loop();
		void SetMover();
		bool GameFocused();
		void ToggleWindowsVisible();
		void SetHoldTime(int enabled);
		zCWorld * GetWorld(zBOOL getEmptyWorldToo);
		bool TryPickMouse();
		bool IsAWorldLoaded();
		zVEC2 GetMousePosVirt();
		void ExportWorldMesh(zSTRING worldName);

		void Reset();

		void SetProperties(zSTRING & propString, zSTRING classNameVob);

		void SelectObject(zCObject * object);

		void ApplyProps(CString str, CString nameCurrent, CString visualStr);

		void WorldPreLoad();
		void WorldAfterLoad();

		void OnLevelReady();

		void OnDynamicUpdateVob(zCVob * vob);

		void OnCreateVob(zCVob * vob, bool select);

		void OnRemoveVob(zCVob * vob);

		void RemoveVob(zCVob * vob);
		void MakeGlobalParent(zCVob* pVob);
		void ApplyPhysicsVob();
		void CleanGlobalParent();



		void CreateNewWaypoint(zCVob * newvob, bool addToNet);

		void CreateNewWaypointFromButtonClick();

		void CreateNewWaypointFromNet(CString classNamePtr, CString vobNamePtr, bool addToNet, bool autoGenerate);


		void CreateNewFreepoint(CString classNameChar, CString nameVob, bool autoName, bool ground);

		void CreateNewVob(CString classNamePtr, CString vobNamePtr, CString visual, int dyn, int stat);

		void CreatePFX(CString name);

		bool CheckForUniqVobs(CString className);

		oCItem* CreateItem(CString name);


		void CreateItemsList();

		void CreatePFXList();

		void CreateMobNameList();

		void CreateVisualsList();

		void CreateSoundList();

		void BuildTree();

		void RebuiltWaypoints();

		void DoCompileLight(int type, int radius);

		void DoCompileWorld(int type);

		void SaveVobTree(zSTRING path);

		zCVob* GetParentVob(zCVob * candidate);

		void OpenVobTree(zSTRING path, bool globalInsert, bool insertNearCamera);



		void SaveFile(zSTRING worldName, int type);

		void HandleWorldBeforeSave();

		void HandleWorldAfterSave();

		void RemoveBadLevelCompoVisual();

		void SaveWorld(zSTRING worldName, int type);

		void MergeMesh(CString worldName);

		void LoadMesh(zSTRING worldName);

		void MergeZen(zSTRING worldName);

		void LoadWorld(zSTRING worldName, int type);


		void GameLoop();

		void PlayTheGame();

		void ToggleGame();
		void InitCamera(zVEC3 pos);

		void StopPlay();

		void Init();

		void SearchSelectVob(zCVob * pickedVob);

		void SearchFillVobClass(CString vobClass, bool isNewType=false);

		void SearchGetSubClasses(CString className);


		bool SearchHandleVob(zCVob *& vob);

		void SearchDoConvert(CString prop);

		bool SearchHandleConvert(zCVob *& vob);

		int SearchFillVobClass(bool derived, int type, int selectedCount);

		void GetAllPfx();

		

		void GetPFXInstanceProps(CString pfxName);

		void UpdatePFXField();
	};

}