// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	enum CompileLightMod
	{
		VertexOnly,
		Low,
		Mid,
		High
	};


	enum zSAVE_TYPE { SAVE_ZEN, SAVE_ZEN_UC, SAVE_ZEN_BIN, SAVE_3DS };
	enum zLOAD_TYPE { LOAD_ZEN, LOAD_ZEN_UC, LOAD_3DS };


	enum TPropEditType {
		PETunknown,
		PETbool,
		PETenum,
		PETint,
		PETfloat,
		PETstring,
		PETvec3,
		PETcolor,
		PETraw,
		PETclass,
		PETgroup,
		PETchunkend
	};


	enum zTWorldLoadMergeMode {
		zWLD_LOAD_MERGE_ADD,
		zWLD_LOAD_MERGE_REPLACE_ROOT_VISUAL
	};


	enum TurnWpMode {
		NONE = 0,
		AGAINST_CAMERA,
		ON_CAMERA,
		RANDOM
	};

	enum SpacerWorkMode 
	{
		SWM_VOBS = 0,
		SWM_MATERIALS = 1,
		SWM_GRASS = 2,
		SWM_MULTISELECT = 3
	};

	enum SpacerToolMode
	{
		TM_NONE = 0,
		TM_TRANSLATE,
		TM_ROTATE,
		TM_BBOXEDIT,
		TM_ONEMODE
	};


	enum ToggleMenuType
	{
		ToggleVobs = 0,
		ToggleWaynet,
		ToggleHelpers,
		ToggleBbox,
		ToggleInvis
	};

	enum SearchVobType
	{
		Search = 0,
		Convert,
		ReplaceVobTree,
		Remove,
		DynColl,
		SearchSingleWP,
		Rename
	};

	struct RespawnEntry
	{
		zCVob* vob;
		zCArray<CString> monsters;
	};


	enum SpacerRainType
	{
		SPACER_RAIN_TYPE_STOP = 0,
		SPACER_RAIN_TYPE_SMOOTH,
		SPACER_RAIN_TYPE_FULL
	};

	enum zEVobMotionMode {
		zMM_NONE,
		zMM_MOVE,
		zMM_ROTATE,
		zMM_SCALE
	};

	enum zEVobMotionHeading {
		zMH_WORLD,
		zMH_LOCALE,
		zMH_VIEW
	};

	enum zEVobCollisionMode {
		zVC_DEFAULT,
		zVC_DISABLED,
		zVC_ENABLED
	};

	enum zECamMotionMode {
		zCM_DISABLED,
		zCM_ENABLED
	};

	enum zEPickMode {
		zPM_DISABLED,
		zPM_VOBS,
		zPM_POLYS
	};

	enum zEPickMethod {
		zPM_CURSOR,
		zPM_CROSSHAIR
	};

	struct zTWE_ControllerEvents {
		Array<zCVob*>      SelectedVobs;
		zCVob*             PickedVob;
		zCPolygon*         PickedPolygon;
		zEPickMode         PickMode;
		zEPickMethod       PickMethod;
		zEVobMotionMode    MotionMode;
		zEVobMotionHeading MotionHeading;
		zEVobCollisionMode CollisionMode;
		zECamMotionMode    CameraMoving;
		zVEC2              CursorPosition;
		float              GizmoScale;
	};


	struct SpacerPickTry
	{
		int ax, ay;
		bool allowed;

		SpacerPickTry::SpacerPickTry()
		{
			allowed = true;
			ax = 0;
			ay = 0;
		}
	};
	

	enum MacrosType
	{
		MT_NONE = 0,
		MT_COMMENT,
		MT_RESET,

		MT_LOAD_WORLD,
		MT_LOAD_MESH,


		MT_SAVE_WORLD_BIN,
		MT_SAVE_WORLD_COMPILED_ASCII,
		MT_SAVE_WORLD_UNCOMPILED_VOBS,


		MT_SAVE_MESH,

		MT_COMPILE_WORLD_OUTDOOR,
		MT_COMPILE_WORLD_INDOOR,
		MT_COMPILE_LIGHT_VERTEX,
		MT_COMPILE_LIGHT_LOW,
		MT_COMPILE_LIGHT_MID,
		MT_COMPILE_LIGHT_HIGH

	};


	enum UIElementType
	{
		UI_SOUND_LIST = 0,
		UI_MUSIC_LIST,
		UI_MAIN_CLASSES,
		UI_LIST_PFX,
		UI_LIST_ITEMS,
		UI_ALL_VOBS_TREE_LIST,
		UI_MAT_LIST,
		UI_LIST_SEARCH_RESULT,
		UI_WIN_VOBLIST,
		UI_CAM_PRESETS
	};

	// Some events for external library
	enum SPC_EVENTLIST
	{
		SPC_EVT_OnCreateVob,        // after object is created
		SPC_EVT_OnDeleteVob,        // before object is removed
		SPC_EVT_OnApplyDataToVob,    // after apply properties to a vob
		SPC_EVT_OnSelectVob          // when a vob is selected
	};

	enum SpacerPfxMotion
	{
		SPACER_PFX_MOTION_TYPE_STATIC = 0,
		SPACER_PFX_MOTION_TYPE_CIRCLE,
		SPACER_PFX_MOTION_TYPE_FORWBACK,
		SPACER_PFX_MOTION_TYPE_ROTATE_LOCAL_Y,
		SPACER_PFX_MOTION_TYPE_FORW_ONLY
	};

	enum SpaceFastVobFieldsSearch
	{
		FAST_SEARCH_FIELD_NONE					= 0,
		FAST_SEARCH_FIELD_NAME					= 1 << 0,
		FAST_SEARCH_FIELD_VISUAL				= 1 << 1,
		FAST_SEARCH_FIELD_SHOW_VISUAL			= 1 << 2,
		FAST_SEARCH_FIELD_CD_STATIC				= 1 << 3,
		FAST_SEARCH_FIELD_CD_DYNYAMIC			= 1 << 4,
		FAST_SEARCH_FIELD_STATIC_VOB			= 1 << 5,
		FAST_SEARCH_FIELD_ANI_MODE				= 1 << 6,
		FAST_SEARCH_FIELD_ANI_MODE_STR			= 1 << 7,
		FAST_SEARCH_FIELD_FAR_CLIP_ZSCALE	    = 1 << 8,
		FAST_SEARCH_FIELD_VISUAL_CAM_ALIGN      = 1 << 9,
	};

	enum TSearchNumberType
	{
		TS_EQUALS,
		TS_LESSTHAN,
		TS_MORETHAN
	};

	enum eSphereDrawMode
	{
		DRAW_NONE,			// no Sphere, standart BBX
		DRAW_ONLY_ORBITS,	// only orbits
		DRAW_FULL_SPHERE	// full mesh
	};


	enum TMatLibFlag { NullLib = 0 };


	enum SpacerNetInterfaceAction
	{
		SPACER_UI_ACTION_OPEN_ZEN = 0,
		SPACER_UI_ACTION_OPEN_MESH = 1
	};
}

#if ENGINE == Engine_G1
DllExport Gothic_I_Classic::zTWE_ControllerEvents ControllerEvents;
#elif ENGINE == Engine_G1A
DllExport Gothic_I_Addon::zTWE_ControllerEvents ControllerEvents;
#elif ENGINE == Engine_G2
DllExport Gothic_II_Classic::zTWE_ControllerEvents ControllerEvents;
#elif ENGINE == Engine_G2A
DllExport Gothic_II_Addon::zTWE_ControllerEvents ControllerEvents;
#endif