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

    public:
        void Init();
        void SendReport(ErrorReportEntry* pEntry);
        void SearchAll();
        void ClearAll();
        void CheckPolygons();
        void PrintReport();
        void SendAll();

        void AddEntry(ErrorReportEntry* pEntry);

    };
}