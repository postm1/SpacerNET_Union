// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	

	struct DebugPointEntry
	{
		zVEC3 origin;
		zVEC3 target;
		zCOLOR color;
		int time;
		zSTRING textStart;
		zSTRING textEnd;
		bool useZBuffer;
		bool isBbox;
		zTBBox3D bbox;

		DebugPointEntry::DebugPointEntry()
		{
			isBbox = false;
		}
	};


	class AB_Debug
	{
	public:
		zSTRING fileName;
		zSTRING fileNameABLog;
		int level;
		bool showTraceLines;
	public:
		bool canWriteZSpy;
		zCView* viewText;


		zCArray<DebugPointEntry*> pListPoints;

	public:
		//void Write(zSTRING msg, bool close = true, bool isScript = false, int level = AB_DebugLevel::HIGH, bool addStrMes = true);
		void Init();
		void Loop();

		void CleanLines();
		DebugPointEntry* AddLine(zVEC3 pos, zVEC3 point, zCOLOR color, float timeSeconds = 15000, bool useZBuffer = false, zSTRING textStart = "", zSTRING textEnd = "");
		void ManageLines();
		DebugPointEntry* AB_Debug::AddBbox(zTBBox3D bbox, zCOLOR color, float timeSeconds = 15000, bool useZBuffer = false, zSTRING textStart = "", zSTRING textEnd = "");
	};
}