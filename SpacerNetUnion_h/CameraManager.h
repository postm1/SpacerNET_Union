// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class CameraManager
	{
	public:
		zCCSCamera* cur_cam;
		zCCSCamera* tempCam;
		bool blockUpdateCamWindow;
		bool blockRename;
		zCView* textView;
		bool cameraRun;
		bool hideVisualWhileActive;
		bool hideHelpVisualsTemp;
		float timeDurationOnSpacer;
		float		lastPresetTime; // временная, для сохранения
		zCVob*		cur_vob;
		
		
		zBOOL		lastPresetAutoAddKF;



	public:
		void Init();
		void Reset();
		void OnSelectCameraVob();
		void Loop();
		void SetCamera(zCVob*);
		void InitAfterWorldLoad();
		void InsertCam(CString name);

		void InsertNewSplineKey();
		void InsertNewTargetKey();

		void RemoveSplineKey();
		void OnRenameSplineKey(int index, CString name);
		void OnRenameTargetKey(int index, CString name);

		void OnRun();
		void OnStop();

		void OnChangeKey(int key);

		void RemoveTargetKeyByIndex(int index);
		void RemoveSplineKeyByIndex(int index);

		void SelectTargetKeyByIndex(int index);
		void SelectSplineKeyByIndex(int index);
		
		void InsertPosKeyAtIndex(int index);
		void InsertTargetKeyAtIndex(int index);

		
	};
}