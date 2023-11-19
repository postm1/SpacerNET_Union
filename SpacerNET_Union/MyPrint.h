// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class MyPrint {
		struct PrintWinMessage {
			int id;
			zSTRING msg;
			int amountMillisec;
			zCOLOR color;
		};
		int currentId;
		zCView * pView;
		zCArray<PrintWinMessage> arrMsgs;
		int pwMaxTime;
		int pwMaxMsgs;

	public:

		void Init();
		void Loop();
		void PrintWin(zSTRING text, zCOLOR color = zCOLOR(255, 255, 255));
		void CheckMsgs();
		void ClearMsgs();
		void PrintRed(zSTRING text);
		void PrintGreen(zSTRING text);
	};
}