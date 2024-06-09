// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


    enum ErrorReportType
    {
        ERROR_REPORT_TYPE_NONE = 0,
        ERROR_REPORT_TYPE_INFO,
        ERROR_REPORT_TYPE_WARNING,
        ERROR_REPORT_TYPE_CRITICAL
    };

    enum ErrorReportProblemType
    {
        ERROR_REPORT_PROBLEM_TYPE_NONE = 0,
        ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_NAME,
        ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_TEXTURE_BAD_NAME,
        ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_TEXTURE_NOT_FOUND,
        ERROR_REPORT_PROBLEM_TYPE_TRIGGER_NO_NAME,
        ERROR_REPORT_PROBLEM_TYPE_PFX_CANT_BE_PARENT,
        ERROR_REPORT_PROBLEM_TYPE_ITEM_CANT_BE_PARENT,
        ERROR_REPORT_PROBLEM_TYPE_ITEM_NO_VISUAL,
        ERROR_REPORT_PROBLEM_TYPE_ZCVOB_EMPTY_VISUAL,
        ERROR_REPORT_PROBLEM_TYPE_FOG_ZONES,
        ERROR_REPORT_PROBLEM_TYPE_VOB_ZONES,
        ERROR_REPORT_PROBLEM_TYPE_MUSIC_ZONES,
        ERROR_REPORT_PROBLEM_TYPE_STARTPOINT,
        ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME,
        ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME_MOB_FOCUS,
        ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_COLLISION,
        ERROR_REPORT_PROBLEM_TYPE_NAME_SPACE,
        ERROR_REPORT_PROBLEM_TYPE_NOT_UNIQ_NAME,
        ERROR_REPORT_PROBLEM_TYPE_BAD_NAME_SYMBOLS,
        ERROR_REPORT_PROBLEM_TYPE_NAME_IS_VISUAL,
        ERROR_REPORT_PROBLEM_TYPE_FOCUS_NAME,
        ERROR_REPORT_PROBLEM_TYPE_KEY_INSTANCE,
        ERROR_REPORT_PROBLEM_TYPE_CONTAINTER_ITEM,
        ERROR_REPORT_PROBLEM_TYPE_SPACES_CONTAINER,
        ERROR_REPORT_PROBLEM_TYPE_SOUNDNAME,
        ERROR_REPORT_PROBLEM_TYPE_SOUNDNAME_NOINST,
        ERROR_REPORT_PROBLEM_TYPE_CONTAINER_BAD_SYMBOLS
    };

    class ErrorReportEntry
    {
    public:
        ErrorReportType errorType;
        ErrorReportProblemType problemType;

        uint objectAddr;
        zSTRING textureName;
        zSTRING materialName;
        zSTRING vobName;

        ErrorReportEntry::ErrorReportEntry()
        {
            errorType = ErrorReportType::ERROR_REPORT_TYPE_NONE;
            problemType = ErrorReportProblemType::ERROR_REPORT_PROBLEM_TYPE_NONE;

            objectAddr = 0;
        }

        void SetErrorType(ErrorReportType type)
        {
            errorType = type;
        }

        void SetProblemType(ErrorReportProblemType type)
        {
            problemType = type;
        }

        void SetVobName(zSTRING name)
        {
            vobName = name;
        }

        void SetObject(uint obj)
        {
            objectAddr = obj;
        }

        void SetMaterialName(zSTRING name)
        {
            materialName = name;
        }

        void SetTextureName(zSTRING name)
        {
            textureName = name;
        }
    };


    class ErrorReport
    {
        zCArray<ErrorReportEntry*> pList;
        zCArray<zSTRING> pDebugList;

    public:
        void Init();
        void SendReport(ErrorReportEntry* pEntry);
        void SearchAll();
        void ClearAll();
        void CheckPolygons();
        void CheckVobs();
        void PrintReport();
        void SendAll();

        void AddEntry(ErrorReportEntry* pEntry);

        void DebugClearVisualsList();
        void DebugRemoveVisuals();
        void DebugAddVisualInList(zSTRING visual);
    };
}