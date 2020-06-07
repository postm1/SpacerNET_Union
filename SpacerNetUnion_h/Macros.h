// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	#define ToStr (zSTRING)

		// напсать строчку на несколько секунд, максимум 6 шт.
	#define printWin(a) ogame->game_text->Printwin(ToStr a);
	#define INP_CLR zinput->ClearKey();
	#define FACTOR_M 81.919998
	#define F(a) a * FACTOR_M			// 8
	#define KeyClick(a)  zinput->KeyToggled(a)
	#define KeyPress(a)  zinput->KeyPressed(a)
	#define Radian(val) val * PI / 180
	#define Dist(a, b)   a->GetDistanceToVob(*b)
	#define GetNPC(a) dynamic_cast<oCNpc*>(a)
	#define LogicKey(a)	zinput->GetState(a)

		// максимальное из двух
	#define max(a,b) (a > b ? a : b)

		// минимальное из двух
	#define min(a,b) (a < b ? a : b)

		//ЦЕНТР ЭКРАНА
	#define _CENTER_SCREEN 4096								

		// максимальный размер экрана
	#define SCREEN_MAX 8192


		// каст пикселей в в.юниты и наоборот
		// pView
	#define PixelToVirtualX(pView, value) pView->anx(value) // по оси Х
	#define PixelToVirtualY(pView, value) pView->any(value) // по оси У

		// Преобразование виртуальных координат в пиксели относительно заданного объекта "pView"
	#define VirtualToPixelX(pView, value) pView->nax(value) // по оси Х
	#define VirtualToPixelY(pView, value) pView->nay(value) // по оси У

		// screen
	#define zAnx(a) screen->anx(a)
	#define zAny(a) screen->any(a)

	#define zREAL_MAX			zREAL(+FLT_MAX)
	#define zREAL_MIN			zREAL(-FLT_MAX)

	#define zADDREF_ACCT(obj)			{(obj)->AddRef(); }
	#define zRELEASE_ACCT(obj)			zRELEASE(obj)
	#define zRELEASE_ACCT_NONULL(obj)	{(obj)->Release(); }
	#define RELEASE_OBJECT(p) do { if(p) { p->Release(); p = NULL; }} while(FALSE)

	#define DLLEXPORT __declspec(dllexport)
	#define NOINLINE __declspec(noinline)
	#define UAPI EXTERN_C NOINLINE DLLEXPORT


	#pragma warning( disable : 4190)
	#pragma warning( disable : 4244)

	typedef void(*voidFuncPointerStart)(void);
	typedef void(*callVoidFunc)(void);
	typedef void(*callIntFunc)(int);
	typedef int(*voidFuncPointer)(void);
	typedef int(*intFuncPointer)(void);

	typedef void(*addToVobList)(uint);

	typedef void(*updateParentVobRemoveNode)(uint);
	typedef void(*updateParentAddNode)(uint, uint);

	typedef void(*loadForm)(int);
	typedef void(*onUpdateVobName)(uint);
	typedef void(*onSelectVob)(uint);
	typedef void(*onSelectNode)(uint);
	typedef void(*addStrToList)(char*);
	typedef void(*strFuncPoiterTree)(char*, uint, uint, char*);
	typedef void(*addNewVob)(uint, uint, int, bool);
	typedef void(*onVobRemove)(uint);
	typedef void(*onShowInputBox)();
	typedef char*(*getInputBox)();
	typedef void(*addPropValue)(char*, char*);
	typedef void(*loadZenAuto)();
	typedef void(*strFuncPoiter)(char*, int, char*);
	typedef void(*AddGlobalEntryPointer)(uint, uint);


	typedef void(*sendIniValue)(char*);

	typedef void(*addClassNode)(int);

	struct SpacerApp;
	struct PFXManager;
	class KeysManager;
	class MyPrint;

#define extFuncs 
	extern int GetFPS(void); \
		extern void	__cdecl sysEvent(); \
		extern SpacerApp theApp; \
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
		extern void InsertVobTest(); \
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
		extern void SetSelectedTool(int tool); \
		extern int GetRandVal(int min, int max); \
		extern void GetDesktopResolution(int& horizontal, int& vertical); \
		extern void SaveCurrentWorldToMSH(); \
		extern void ToggleMusic(bool mode); \
		extern void ScaleVob(zCVob* pVob, float scale); \
		extern zSTRING GetFileExtension(zSTRING name); \
		extern PFXManager pfxManager; \
		extern void LoadMaterials(); \
		extern void LoadMatLib(CString name); \
		extern void LoadMat(); \
		extern bool IsValidZObject(void* ptr); \
		extern KeysManager keys; \
		extern CString GetLang(CString key); \
		extern void ClearLangStrings(); \

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
}