// Supported with union (c) 2020 Union team
// Union HEADER file

//#include <dinput.h>
//#define DIRECTINPUT_VERSION 0x0700

namespace GOTHIC_ENGINE {
	// Add your code here . . .



	#define ToStr (zSTRING)

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
	#define DllExport __declspec(dllexport)
	#define MultiZenDef(aG1, aG1A, aG2, aG2A) CHECK_THIS_ENGINE ? ZenDef<TInstance>(aG1, aG1A, aG2, aG2A) : 0x00000000

	#define max(a,b) (a > b ? a : b)

	#define min(a,b) (a < b ? a : b)

	//CENTER_SCREEN
	#define _CENTER_SCREEN 4096								

	// MAX SCREEN SIZE
	#define SCREEN_MAX 8192

	// sorting stuff
	#define d(i) (((char *)data)+(i)*size)

	#define PixelToVirtualX(pView, value) pView->anx(value) // �� ��� �
	#define PixelToVirtualY(pView, value) pView->any(value) // �� ��� �

	#define VirtualToPixelX(pView, value) pView->nax(value) // �� ��� �
	#define VirtualToPixelY(pView, value) pView->nay(value) // �� ��� �

		// screen
	#define zAnx(a) screen->anx(a)
	#define zAny(a) screen->any(a)

	#define zREAL_MAX			zREAL(+FLT_MAX)
	#define zREAL_MIN			zREAL(-FLT_MAX)

	#define zADDREF_ACCT(obj)			{(obj)->AddRef(); }
	#define zRELEASE_ACCT(obj)			zRELEASE(obj)
	#define zRELEASE_ACCT_NONULL(obj)	{(obj)->Release(); }
	#define RELEASE_OBJECT(p) do { if(p) { p->Release(); p = Null; }} while(FALSE)

	#define DLLEXPORT __declspec(dllexport)
	#define NOINLINE __declspec(noinline)
	#define UAPI EXTERN_C NOINLINE DLLEXPORT

// G1 wheel
#define zMOUSE_SCROLL_UP		0x809
#define zMOUSE_SCROLL_DOWN		0x80A

	#define M_PI			3.14159265358979323846
	#define Alg_Rad2Deg(rad) (zREAL(zREAL(rad)*zREAL(180.0f)/zREAL(M_PI)))
#define MulColor( /*zCOLOR*/ vertColor, /*int[3]*/ matColor ) \
  (((vertColor.GetRedByte	()*matColor[0])>>8)<<16)	\
 |(((vertColor.GetGreenByte ()*matColor[1])>>8)<<8)		\
 |(((vertColor.GetBlueByte	()*matColor[2])>>8))


#define zERR_MESSAGE(level, flag, text)		((level)<=zerr->GetFilterLevel()) ? zerr->Report(zERR_TYPE_INFO,  0, text, level, flag, __LINE__,  __FILE__, 0 ) : zERR_NONE
#define zERR_WARNING(text)					zerr->Report(zERR_TYPE_WARN,  0, text, 0, 0, __LINE__,  __FILE__, 0 )
#define zERR_FAULT(text)					zerr->Report(zERR_TYPE_FAULT, 0, text, 0, 0, __LINE__,  __FILE__, 0 )
#define zERR_FATAL(text)					zerr->Report(zERR_TYPE_FATAL, 0, text, -1, 0, __LINE__,  __FILE__, 0 )


	inline float zFloor(const float a) { return float(floor(a)); }
	inline float zCeil(const float a) { return float(ceil(a)); }

	const uint TIMER_ID_OBJECTS_SEWER = 0;
	const uint TIMER_ID_SHUTDOWN_SOUNDS = 1;
	const uint TIMER_ID_PFX_UPDATE_BBOX = 2;
	const uint TIMER_ID_CHECK_RAM = 3;

#if ENGINE == Engine_G1
#define zERR_NONE 0
#endif

	#pragma warning( disable : 4190)
	#pragma warning( disable : 4244)

	typedef void(*voidFuncPointerStart)(void);
	typedef void(*callVoidFunc)(void);
	typedef void(*callIntFunc)(int);
	typedef void(*callUIntFunc)(int);
	typedef void(*callInt2Func)(int, int);
	typedef int(*voidFuncPointer)(void);
	typedef int(*intFuncPointer)(void);
	typedef int(*appActivePointer)(int);
	typedef void(*addToVobList)(uint);
	typedef void(*callIntInt)(int, int);
	typedef void(*updateParentVobRemoveNode)(uint);
	typedef void(*updateParentAddNode)(uint, uint);

	typedef void(*loadForm)(int);
	typedef void(*onUpdateVobName)(uint);
	typedef void(*onSelectVob)(uint);
	typedef void(*onSelectNode)(uint);
	typedef void(*addStrToList)(char*);
	typedef void(*strFuncPoiterTree)(char*, uint, uint, char*);
	typedef void(*addNewVob)(uint, uint, int, int);
	typedef void(*onVobRemove)(uint);
	typedef void(*onShowInputBox)();
	typedef char*(*getInputBox)();
	typedef void(*addPropValue)(char*, char*);
	typedef void(*loadZenAuto)();
	typedef void(*strFuncPoiter)(char*, int, char*);
	typedef void(*AddGlobalEntryPointer)(uint, uint);

	typedef bool(*compareVobs)();


	typedef void(*sendIniValue)(char*);
	typedef void(*sendKey)(int);
	typedef void(*addClassNode)(int);

	extern zCVob* pfxEditorVob;

}