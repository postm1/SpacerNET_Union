// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .




#define extFuncs 
	extern int GetFPS(void); \
		extern MatManager mm; \
		extern SpacerApp theApp; \
		extern MatFilter mf; \
		extern void	__cdecl sysEvent(); \
		extern void CameraMoving(); \
		extern void VobMoving(); \
		extern void SetTime(int hour, int min); \
		extern void BuildClassHierarchy(); \
		extern bool SetBaseClass(zSTRING class_name); \
		extern void CreateBaseVobTree(zCTree<zCVob>* node); \
		extern void CreateTree(); \
		extern MyPrint print; \
		extern void OutFile(zSTRING str, bool close); \
		extern void PrintDebug(zSTRING str); \
		extern void ResetPrintDebug(); \
		extern zSTRING GetVobName(zCVob* vob); \
		extern void __cdecl PlaySoundGame(class zSTRING &); \
		extern void DrawLine(zCCamera *cam, zCVob* v1, zCVob* v2, zCOLOR col); \
		extern float GetBboxValue(int coord); \
		extern void SetBBoxValue(int x, int y, int z); \
		extern void InsertIntoWorld(zCVob* newVob, zCVob* pickedVob, bool selectVob = true); \
		extern bool CheckVobNameExist(CString name, bool isWaypoint); \
		extern void SetOnFloor(zCVob* pickedVob); \
		extern void ClearLMB(); \
		extern void SetRangeVobs(); \
		extern void SetRangeWorld(); \
		extern zSTRING GetVobNameSafe(zCVob* vob); \
		extern int& playerLightInt; \
		extern double GetAngleBetweenVobs(zCVob* vob, zCVob* target, zVEC3 realUnit); \
		extern void FindNearestVobs(); \
		extern void HandleVobTranslation(zCVob* pickedVob, zVEC3 pos); \
		extern void WriteLine(char* message); \
		extern void CreateFileList(); \
		extern bool GetWayPointInfo(Common::string& name, uint& index); \
		extern Common::Array<zCWaypoint*> CollectWaypointsByName(Common::string baseName); \
		extern bool HasWayPointInCollection(uint targetIndex, Common::Array<zCWaypoint*> wayPoints); \
		extern Common::string FormatIndex(uint index); \
		extern Common::string GetNextWayPointName(Common::string copiedName); \
		extern Common::string GetNextFreePointName(Common::string copiedName); \
		extern bool CheckUniqNameExist(CString name); \
		extern void RecalcWPBBox(zCVob* vob); \
		extern zSTRING OnWorldAnalysewaynet(); \
		extern void RenderVobsLoop(); \
		extern void AddVobToRender(zSTRING visual, bool isItem); \
		extern int GetRandVal(int min, int max); \
		extern void GetDesktopResolution(int& horizontal, int& vertical); \
		extern void ToggleMusic(bool mode); \
		extern void ScaleVob(zCVob* pVob, float scale); \
		extern zSTRING GetFileExtension(zSTRING name); \
		extern PFXManager pfxManager; \
		extern void LoadMat(); \
		extern bool IsValidZObject(void* ptr); \
		extern KeysManager keys; \
		extern CString GetLang(CString key); \
		extern void ClearLangStrings(); \
		extern int GetZenLevelCompoCount(); \
		extern float Lerp(float a, float b, float t); \
		extern zVEC3 Slerp(zVEC3 start, zVEC3 end, float percent); \
		extern void SetSelectedTool(SpacerToolMode tool); \
		extern bool IsVirtualFile(CString fileName); \
		extern void HandleInsertVobCopy(zCVob* pickedVob); \
		extern void HandleParentChange(zCVob* vob, zCVob* newParent); \
		extern void CALL_OnCreateVob(zCVob* pVob); \
		extern void CALL_OnDeleteVob(zCVob* pVob); \
		extern void CALL_OnApplyDataToVob(zCVob* pVob); \
		extern void CALL_OnSelectVob(zCVob* pVob); \



	UAPI extern void   Stack_PushString(const char* v);
	UAPI extern void   Stack_PushInt(int v);
	UAPI extern void   Stack_PushLong(int64 v);
	UAPI extern void   Stack_PushUInt(uint v);
	UAPI extern void   Stack_PushULong(uint64 v);
	UAPI extern void   Stack_PushFloat(float v);
	UAPI extern void   Stack_PushDouble(double v);
	UAPI extern char*  Stack_PeekString_S();
	UAPI extern string Stack_PeekString();
	UAPI extern CStringW Stack_PeekStringW();
	UAPI extern int    Stack_PeekInt();
	UAPI extern int64  Stack_PeekLong();
	UAPI extern uint   Stack_PeekUInt();
	UAPI extern uint64 Stack_PeekULong();
	UAPI extern float  Stack_PeekFloat();
	UAPI extern double Stack_PeekDouble();
	UAPI extern void   Stack_Pop();
	UAPI extern int	   GetStackSize();
}