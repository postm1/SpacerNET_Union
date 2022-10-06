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
		ON_CAMERA
	};

	enum SpacerWorkMode 
	{
		SWM_VOBS = 0,
		SWM_MATERIALS = 1,
		SWM_GRASS = 2,
		SWM_MULTISELECT = 3,
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
		DynColl
	};

	struct RespawnEntry
	{
		zCVob* vob;
		zCArray<CString> monsters;
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


	// перечисление событий (индексы для распознавания)
	enum SPC_EVENTLIST
	{
		SPC_EVT_OnCreateVob,        // событие после создания объекта
		SPC_EVT_OnDeleteVob,        // событие перед удалением объекта
		SPC_EVT_OnApplyDataToVob,    // событие после применения свойств объекта
		SPC_EVT_OnSelectVob          // событие при выборе воба
	};

}

DllExport Gothic_II_Addon::zTWE_ControllerEvents ControllerEvents;
