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
		Rename
	};


	
}
