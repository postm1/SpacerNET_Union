// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class MyPrint {
		struct PrintWinMessage {
			int id; // айди сообщения, нужен для сортировки одинаковых сообщений
			zSTRING msg; // сообщение
			int amountMillisec; // колличество миллисекунд, что сообщение активно
			zCOLOR color; // цвет сообщения, по умолчанию - белый
		};
		int currentId; // максимально используемоей айди в текущей сессии
		zCView * pView;
		zCArray<PrintWinMessage> arrMsgs;
		int pwMaxTime; // максимальное время, что сообщения висят на экране
		int pwMaxMsgs; // количество сообщений

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