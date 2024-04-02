// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {


	zCView* standardView() {
		zCView* pView = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
		pView->SetFont("FONT_OLD_10_WHITE.TGA");
		pView->SetFontColor(zCOLOR(255, 255, 255, 255));
		return pView;
	}

	void AB_Debug::Init()
	{


		//cmd << level << endl;
		viewText = standardView();
		screen->InsertItem(viewText);
	}



	void AB_Debug::Loop()
	{
		if (true || showTraceLines)
		{
			ManageLines();
		}

	}

	// Trance lines output
	void AB_Debug::ManageLines()
	{
		viewText->ClrPrintwin();

		zCCamera*& pCamera = zCCamera::activeCam;

		for (int i = 0; i < pListPoints.GetNumInList(); i++)
		{
			auto entry = pListPoints.GetSafe(i);

			if (entry)
			{
				if (entry->time > 0)
				{
					entry->time -= ztimer->frameTimeFloat;

					if (entry->isBbox)
					{
						entry->bbox.Draw(entry->color);

						continue;
					}

					zlineCache->Line3D(entry->origin, entry->target, entry->color, FALSE);


					if (entry->textStart.Length() > 0)
					{
						zVEC3 vWorldPos = entry->target + zVEC3(0, 20, 0);

						// transform the world position to pixels on screen
						float fPixelPosX = FLT_MAX, fPixelPosY = FLT_MAX;
						if (vWorldPos[0] != FLT_MAX) {
							pCamera->Activate();
							zVEC3 vec = pCamera->camMatrix * vWorldPos;
							if (vec[2] > 0)
								pCamera->Project(&vec, fPixelPosX, fPixelPosY);
						}

						if (fPixelPosX != FLT_MAX) {
							fPixelPosX -= screen->nax(screen->FontSize(entry->textStart) / 2);
							int x = screen->anx(fPixelPosX);
							int y = screen->any(fPixelPosY);

							zClamp(y, screen->FontY(), 8191 - screen->FontY());
							zClamp(x, 0, 8191 - screen->FontSize(entry->textStart));


							viewText->Print(x, y, entry->textStart);
						}
					}


					if (entry->textEnd.Length() > 0)
					{
						zVEC3 vWorldPos = entry->target + zVEC3(0, 20, 0);

						// transform the world position to pixels on screen
						float fPixelPosX = FLT_MAX, fPixelPosY = FLT_MAX;
						if (vWorldPos[0] != FLT_MAX) {
							pCamera->Activate();
							zVEC3 vec = pCamera->camMatrix * vWorldPos;
							if (vec[2] > 0)
								pCamera->Project(&vec, fPixelPosX, fPixelPosY);
						}

						if (fPixelPosX != FLT_MAX) {
							fPixelPosX -= screen->nax(screen->FontSize(entry->textEnd) / 2);
							int x = screen->anx(fPixelPosX);
							int y = screen->any(fPixelPosY);

							zClamp(y, screen->FontY(), 8191 - screen->FontY());
							zClamp(x, 0, 8191 - screen->FontSize(entry->textEnd));


							viewText->Print(x, y, entry->textEnd);
						}
					}

				}
			}
		}
	}

	//

	DebugPointEntry* AB_Debug::AddBbox(zTBBox3D bbox, zCOLOR color, float timeSeconds, bool useZBuffer, zSTRING textStart, zSTRING textEnd)
	{
		DebugPointEntry* pEntry = new DebugPointEntry();
		pEntry->color = color;
		pEntry->time = timeSeconds;
		pEntry->textEnd = textEnd;
		pEntry->textStart = textStart;
		pEntry->useZBuffer = useZBuffer;
		pEntry->isBbox = true;
		pListPoints.InsertEnd(pEntry);

		pEntry->bbox = bbox;

		return pEntry;
	}

	DebugPointEntry* AB_Debug::AddLine(zVEC3 pos, zVEC3 point, zCOLOR color, float timeSeconds, bool useZBuffer, zSTRING textStart, zSTRING textEnd)
	{
		DebugPointEntry* pEntry = new DebugPointEntry();
		pEntry->origin = pos;
		pEntry->target = point;
		pEntry->color = color;
		pEntry->time = timeSeconds;
		pEntry->textEnd = textEnd;
		pEntry->textStart = textStart;
		pEntry->useZBuffer = useZBuffer;
		pListPoints.InsertEnd(pEntry);

		return pEntry;
	}

	void AB_Debug::CleanLines()
	{
		pListPoints.DeleteListDatas();
	}
}