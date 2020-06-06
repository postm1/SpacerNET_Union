// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	struct SpacerApp
	{
		// handle ГЛАВНОЙ формы c#
		HWND mainWin;
		// Библиотека c#
		HMODULE module;

		COption spcOpt;
		CSettings options;

		struct
		{
			zCMover* moverVob;
			int m_kf_pos;
			zCVob* event_sourcevob;
		};

		struct
		{
			bool isMesh;
			bool isMerged;
			// выбранный воб
			zCVob* pickedVob;

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
			bool nextInsertBlocked;


			bool dataFilled;
			bool hideWindows;
			bool isExit;
			bool selectedWpForCreateIsBlocked;
			bool firstZenLoad;

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
			//spcCObjPropertyList	propList;

			struct
			{
				bool g_bIsPlayingGame;
				bool g_bInsertNPCs;
			};
		};

		void BlockMouseClick(int time);
		bool IsClicksBlocked();
		void PluginLoop();
		void KeysLoop();
		void RenderStartScreen();
		SpacerApp();
		zCVob * GetSelectedVob();
		void SetSelectedVob(zCVob * vob, zSTRING funcName = "");
		void SetToKeyPos();
		void OnKeyRemove();
		void SendTrigger(int actionIndex);
		void OnAddMoverKey(int mode);
		void SetCurrentKey(int key);
		void ApplyProps(char* str, char* name);
		void PreRender();
		void RemoveTargetListTrigger();
		void CollectTargetListTrigger();
		void CollectSourcerTriggerList();
		void AfterRender();
		void PickVob();
		void ToggleWP();
		void ConnectWP();
		void DisconnectWP();
		zSTRING GenerateWPName(zSTRING baseName);
		void Loop();
		void SetMover();
		bool GameFocused();

		zCWorld * GetWorld(zBOOL getEmptyWorldToo);

		bool IsAWorldLoaded();

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

		void ApplyPhysicsVob();




		void CreateNewWaypoint(zCVob * newvob, bool addToNet);

		void CreateNewWaypointFromButtonClick();

		void CreateNewWaypointFromNet(CString classNamePtr, CString vobNamePtr, bool addToNet, bool autoGenerate);


		void CreateNewFreepoint(CString classNameChar, CString nameVob, bool autoName, bool ground);

		void CreateNewVob(CString classNamePtr, CString vobNamePtr, CString visual, int dyn, int stat);

		void CreatePFX(CString name);

		void CreateItem(CString name);


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

		void OpenVobTree(zSTRING path, bool globalInsert);



		void SaveFile(zSTRING worldName, int type);

		void HandleWorldBeforeSave();

		void HandleWorldAfterSave();

		void SaveWorld(zSTRING worldName, int type);

		void LoadMesh(zSTRING worldName);

		void MergeZen(zSTRING worldName);

		void LoadWorld(zSTRING worldName);

		void GameLoop();

		void PlayTheGame();

		void StopPlay();

		void Init();

	};

}