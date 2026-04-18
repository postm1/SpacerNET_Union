// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void MyPrint::Init() {
		pView = new zCView(0, 0, 8192, 8192);
		pView->SetFont("FONT_OLD_10_WHITE.TGA");
		pView->SetFontColor(zCOLOR(15, 255, 15));
		pView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
		pwMaxMsgs = 6;
	}

	void MyPrint::ClearMsgs() {
		arrMsgs.DeleteList();
	}

	void MyPrint::CheckMsgs() {
		int isRemove = 0;
		for (int i = 0; i < arrMsgs.GetNum(); i++) {
			PrintWinMessage& msg = arrMsgs.GetSafe(i);

			

			if (msg.amountMillisec < 1) {

				msg.RemoveView();

				arrMsgs.RemoveIndex(i);
				i--;
				isRemove++;
			}
		}

		if (isRemove > 0) {
			zCArray<PrintWinMessage> arr;

			while (arrMsgs.GetNum()) {
				int min = INT_MAX;
				int index = 0;
				for (int i = 0; i < arrMsgs.GetNum(); i++) {
					PrintWinMessage& msg = arrMsgs.GetSafe(i);
					if (min > msg.id) {
						min = msg.id;
						index = i;
					}
				}
				PrintWinMessage& msg = arrMsgs.GetSafe(index);
				arr.InsertEnd(msg);
				arrMsgs.RemoveIndex(index);
			}

			for (int i = 0; i < arr.GetNum(); i++) {
				PrintWinMessage& msg = arr.GetSafe(i);
				arrMsgs.InsertEnd(msg);
			}
		}
	}



	void MyPrint::Loop() {
		if (pView && arrMsgs.GetNum()) {

			bool showBack = theApp.options.GetVal("bShowTextBack");


			screen->InsertItem(pView);

			for (int i = 0; i < arrMsgs.GetNum(); i++) {
				PrintWinMessage& msg = arrMsgs.GetSafe(i);
				msg.amountMillisec -= (int)ztimer->frameTimeFloat;
				zCOLOR& color = msg.color;
				int x = (8192 - pView->FontSize(msg.msg)) / 2;
				int y = 400 + i * (pView->FontY() + 140);

				auto* pViewBack = msg.pViewBack;


				if (pViewBack && showBack)
				{
					screen->InsertItem(pViewBack);
					pViewBack->ClrPrintwin();
					pViewBack->SetPos(x - TEXT_PADDING / 2, y - TEXT_PADDING / 2);
					pViewBack->Blit();
					screen->RemoveItem(pViewBack);
				}
				
				pView->ClrPrintwin();
				pView->SetFontColor(color);
				pView->Print(x, y, msg.msg);
				pView->Blit();
				
			}
			

			screen->RemoveItem(pView);

			CheckMsgs();
		}
	}

	void MyPrint::PrintWin(zSTRING text, zCOLOR color, int maxTime) {
		PrintWinMessage msg;
		msg.msg = text;
		msg.amountMillisec = maxTime * 1000;
		msg.id = currentId++;
		msg.color = color;

		msg.pViewBack = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
		msg.pViewBack->InsertBack("BLACK.TGA");
		msg.pViewBack->alpha = 175;
		msg.pViewBack->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);

		// set size of the background
		msg.pViewBack->SetSize(pView->FontSize(text) + TEXT_PADDING, pView->FontY() + TEXT_PADDING);

		if (arrMsgs.GetNum() < pwMaxMsgs) {
			arrMsgs.Insert(msg);
		}
		else {
			arrMsgs.RemoveIndex(0);
			arrMsgs.Insert(msg);
		}
	}

	void MyPrint::PrintRed(zSTRING text, int time) {
		zCOLOR color = zCOLOR(255, 15, 15);
		PrintWin(text, color, time);
	}

	void MyPrint::PrintGreen(zSTRING text, int time) {
		zCOLOR color = zCOLOR(0, 255, 0);
		PrintWin(text, color, time);
	}

	int MyPrint::PrintGreenTracked(zSTRING text, int time) {
		zCOLOR color = zCOLOR(0, 255, 0);
		PrintWinMessage msg;
		msg.msg = text;
		msg.amountMillisec = time * 1000;
		msg.id = currentId++;
		msg.color = color;
		if (arrMsgs.GetNum() < pwMaxMsgs) {
			arrMsgs.Insert(msg);
		}
		else {
			arrMsgs.RemoveIndex(0);
			arrMsgs.Insert(msg);
		}
		return msg.id;
	}

	void MyPrint::RemoveById(int id) {
		for (int i = 0; i < arrMsgs.GetNum(); i++) {
			if (arrMsgs.GetSafe(i).id == id) {
				arrMsgs.RemoveIndex(i);
				return;
			}
		}
	}
}