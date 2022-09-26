// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void AB_NoGrass::HideObjects()
	{
		//RX_Begin(0);

		int count = filteredList.GetNumInList();

		if (count == 0)
		{
			return;
		}

		//если ничего не скрываем, то ничего и не делаем
		if (percentGrass == 0 && percentBush == 0)
		{
			return;
		}

		hiddenAmount = 0;
		int maxDist = 0;
		int time = 7000;
		//

		for (int i = 0; i < count; i++)
		{
			zCVob* pVob = filteredList.GetSafe(i);

			if (pVob && pVob->GetVisual())
			{
				zSTRING name = pVob->GetVisual()->GetVisualName().Upper();

				if (!pVob->showVisual)
				{
					//Col16 color_red(CMD_RED);

					//cmd << "ShowVisual false for " << name << color_red << endl;
				}

				if (hideGrass && grassList.IsInList(name))
				{
					bariCenter += pVob->GetPositionWorld();
					hiddenAmount += 1;
					vobListGrass.Insert(pVob);
				}
				else if (hideBush && (name.contains("NW_NATURE_BUSH") || name.contains("NW_NATURE_FARN") || bushList.IsInList(name)))
				{
					bariCenter += pVob->GetPositionWorld();
					hiddenAmount += 1;
					vobListGrass.Insert(pVob);
				}
			}
		}


		//cmd << "vobListPlantsList: " << vobListGrass.GetNumInList() << endl;


		if (hiddenAmount == 0)
		{
			//printWin("No grass found");
			return;
		}

		//cmd << "GrassOrBust list: " << vobListGrass.GetNumInList() << endl;


		// считаем геометрический центр всех вобов
		bariCenter /= hiddenAmount;

		//cmd << "bariCenter: " << bariCenter.ToString() << endl;



		// ищем самый удаленый воб, чтоб сделать квадрат нужного размера
		for (int i = 0; i < vobListGrass.GetNumInList(); i++)
		{
			zCVob* pVob = vobListGrass.GetSafe(i);

			if (pVob)
			{
				int dist = pVob->GetPositionWorld().Distance(bariCenter);

				//cmd << pVob->GetObjectName() << " Dist: " << dist << endl;

				if (dist > maxDist)
				{
					maxDist = dist;
				}
			}
		}

		//увеличиваем размер, потому что воб может оказаться за квадратом в некоторых случаях
		maxDist *= sqrt(2.0f);

		// еще небольшой запас
		maxDist *= 1.10f;






		//cmd << "maxDist: " << maxDist << endl;


		//4 точки квадрата, который покрывает всю карту
		zVEC3 first = bariCenter + zVEC3(maxDist * cos(PI / 4), 0, maxDist * sin(PI / 4));
		zVEC3 second = bariCenter + zVEC3(maxDist * cos(PI / 4 + PI / 2), 0, maxDist * sin(PI / 4 + PI / 2));
		zVEC3 third = bariCenter + zVEC3(maxDist * cos(5 * PI / 4), 0, maxDist * sin(5 * PI / 4));
		zVEC3 forth = bariCenter + zVEC3(maxDist * cos(-PI / 4), 0, maxDist * sin(-PI / 4));


		/*
		debug.AddLine(first, second, GFX_WHITE, time);
		debug.AddLine(second, third, GFX_WHITE, time);
		debug.AddLine(third, forth, GFX_WHITE, time);
		debug.AddLine(forth, first, GFX_WHITE, time);
		*/


		zVEC3 norm12 = (second - first).Normalize();
		zVEC3 norm14 = (forth - first).Normalize();

		int cellSize = 2000;
		int width = first.Distance(second);
		int countCells = width / cellSize + 2;


		int num = 0;

		//
		for (int i = 0; i < countCells; i++)
		{
			for (int j = 0; j < countCells; j++)
			{
				GridEntry* entry = new GridEntry();

				entries.InsertEnd(entry);


				entry->p1 = first + norm12 * (cellSize * j) + norm14 * (cellSize * i);
				entry->p2 = entry->p1 + norm12 * cellSize;
				entry->p3 = entry->p1 + (norm12 + norm14).Normalize() * sqrt(2.0f) * cellSize;
				entry->p4 = entry->p1 + norm14 * cellSize;
				//entry->center = (entry->p1 + entry->p2 + entry->p3 + entry->p4) / 4;
				//zVEC3 forth_dot = first_dot + zVEC3(0, 0, -1) * cellSize;
				/*

				zVEC3 forth_dot = first + zVEC3((i+1) * cellSize, 1000, (j + 1) * cellSize);
				*/

				/*zVEC3 second_dot = bariCenter + zVEC3(maxDist * cos(PI / 4 + PI / 2), 1000, maxDist * sin(PI / 4 + PI / 2));
				zVEC3 third_dot = bariCenter + zVEC3(maxDist * cos(5 * PI / 4), 1000, maxDist * sin(5 * PI / 4));
				zVEC3 forth_dot = bariCenter + zVEC3(maxDist * cos(-PI / 4), 1000, maxDist * sin(-PI / 4));
				*/
				/*
				zCOLOR col;

				switch (i)
				{
				case 0: { col = GFX_RED; break; };
				case 1: { col = GFX_YELLOW; break; };
				case 2: { col = GFX_GREEN; break; };
				case 3: { col = GFX_WHITE; break; };
				default: { col = GFX_BLUE; break; };
				}
				*/
				/*
				debug.AddLine(entry->p1, entry->p2, col, time);
				debug.AddLine(entry->p2, entry->p3, col, time);
				debug.AddLine(entry->p3, entry->p4, col, time);
				debug.AddLine(entry->p4, entry->p1, col, time);
				*/

				num += 1;


			}
		}


		//XYZ

		/*
		debug.AddLine(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(200, 0, 0), GFX_RED, 60000, false, "", "X");
		debug.AddLine(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(0, 200, 0), GFX_YELLOW, 60000, false, "", "Y");
		debug.AddLine(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(0, 0, 200), GFX_RED, 60000, false, "", "Z");
		*/




		//
		for (int i = 0; i < vobListGrass.GetNumInList(); i++)
		{
			zCVob* pVob = vobListGrass.GetSafe(i);



			if (pVob)
			{
				bool flag = false;

				for (int k = 0; k < entries.GetNumInList(); k++)
				{
					auto entry = entries.GetSafe(k);

					if (entry && entry->IsPosInGrid(pVob->GetPositionWorld()))
					{
						zSTRING name = pVob->GetVisual()->GetVisualName().Upper();

						if (name.contains("NW_NATURE_BUSH") || name.contains("NW_NATURE_FARN") || bushList.IsInList(name))
						{
							entry->pListBush.InsertEnd(pVob);
						}
						else
						{
							entry->pListGrass.InsertEnd(pVob);
						}


						//debug.AddLine(pVob->GetPositionWorld(), pVob->GetPositionWorld() + zVEC3(0, 3000, 0), GFX_RED, time, 0, "", Z k);
						flag = true;
						break;
					}
				}

				if (!flag)
				{
					//printWin("No cell found for... ");
					//debug.AddLine(pVob->GetPositionWorld(), pVob->GetPositionWorld() + zVEC3(0, 3000, 0), GFX_RED, time);
				}
			}
		}

		//  cmd << "Entries..." << endl;


		//
		for (int k = 0; k < entries.GetNumInList(); k++)
		{
			auto entry = entries.GetSafe(k);

			if (entry && entry->pListGrass.GetNumInList() > 0)
			{
				//cmd << entry->pList.GetNumInList() << endl;
				int numberToSelect = (entry->pListGrass.GetNumInList() * percentGrass) / 100;

				if (percentGrass == 100)
				{
					for (int z = 0; z < entry->pListGrass.GetNumInList(); z++)
					{
						entry->pListGrass.GetSafe(z)->showVisual = 0;
						hiddenList.Insert(entry->pListGrass.GetSafe(z));
					}
				}
				else
				{
					//cmd << "entry: " << k << "; Number " << numberToSelect << endl;

					int counterAdd = 0;
					for (int x = 0; x < entry->pListGrass.GetNumInList(); x++)
					{
						auto pCurVob = entry->pListGrass.GetSafe(x);

						if (pCurVob)
						{
							pCurVob->showVisual = 0;
							hiddenList.Insert(pCurVob);
							counterAdd += 1;
						}

						if (counterAdd >= numberToSelect) {
							break;
						}
					}

					/*
					while (numberToSelect > 0)
					{
					int x = rand() % entry->pListGrass.GetNumInList();

					if (entry->pListGrass.GetSafe(x))
					{
					entry->pListGrass.GetSafe(x)->showVisual = 0;
					hiddenList.Insert(entry->pListGrass.GetSafe(x));
					numberToSelect -= 1;
					}
					}
					*/
				}

			}

			if (entry && entry->pListBush.GetNumInList() > 0)
			{
				int numberToSelect = (entry->pListBush.GetNumInList() * percentBush) / 100;

				if (percentBush == 100)
				{
					for (int z = 0; z < entry->pListBush.GetNumInList(); z++)
					{
						entry->pListBush.GetSafe(z)->showVisual = 0;
						hiddenList.Insert(entry->pListBush.GetSafe(z));
					}
				}
				else
				{
					int counterAdd = 0;
					for (int x = 0; x < entry->pListBush.GetNumInList(); x++)
					{
						auto pCurVob = entry->pListBush.GetSafe(x);

						if (pCurVob)
						{
							pCurVob->showVisual = 0;
							hiddenList.Insert(pCurVob);
							counterAdd += 1;
						}

						if (counterAdd >= numberToSelect) {
							break;
						}
					}

					/*
					while (numberToSelect > 0)
					{
					int x = rand() % entry->pListBush.GetNumInList();

					if (entry->pListBush.GetSafe(x))
					{
					entry->pListBush.GetSafe(x)->showVisual = 0;
					hiddenList.Insert(entry->pListBush.GetSafe(x));
					numberToSelect -= 1;
					}
					}
					*/
				}

			}
		}

		//RX_End(0);

		//cmd << "HiddenList: " << hiddenList.GetNumInList() << endl;
		//cmd << "Time: " << RX_PerfString(0) << endl;

		/*
		cmd << "Perf50: " << RX_PerfString(50) << endl;
		cmd << "Perf51: " << RX_PerfString(51) << endl;
		cmd << "Perf52: " << RX_PerfString(52) << endl;
		cmd << "Perf53: " << RX_PerfString(53) << endl;
		*/

		//printWin("Max dist: " + Z maxDist);
		//printWin("width: " + Z width);
		//printWin("countCells: " + Z countCells);
	}

	void AB_NoGrass::PrepareObjectsSaveGame()
	{
		//cmd << "PrepareObjectsSaveGame Start: " << hiddenList.GetNumInList() << endl;

		int count = hiddenList.GetNumInList();

		for (int i = 0; i < count; i++)
		{
			zCVob* pVob = hiddenList.GetSafe(i);

			if (pVob)
			{
				pVob->showVisual = 1;
			}
		}
		//cmd << "PrepareObjectsSaveGame count: " << restoreSaveList.GetNum() << endl;

		//cmd << "PrepareObjectsSaveGame End" << endl;
	}

	void AB_NoGrass::RestoreObjectsSaveGame()
	{
		//cmd << "RestoreObjectsSaveGame Start: " << restoreSaveList.GetNumInList() << endl;

		int count = hiddenList.GetNumInList();

		for (int i = 0; i < count; i++)
		{
			zCVob* pVob = hiddenList.GetSafe(i);

			if (pVob)
			{
				pVob->showVisual = 0;
			}
		}

		//cmd << "RestoreObjectsSaveGame End" << endl;
	}

	void AB_NoGrass::RestoreObjects()
	{
		//cmd << "RestoreObjects Start: " << hiddenList.GetNumInList() << endl;

		int count = hiddenList.GetNumInList();

		for (int i = 0; i < count; i++)
		{
			zCVob* pVob = hiddenList.GetSafe(i);

			if (pVob)
			{
				pVob->showVisual = 1;
			}
		}

		//cmd << "RestoreObjects End" << endl;
	}

	void AB_NoGrass::Clear()
	{
		//cmd << "AB_NoGrass::Clear" << endl;

		if (filteredList.GetNumInList() > 0)
		{
			//cmd << "Try delete vobListManage" << endl;
			filteredList.DeleteList();
		}

		if (vobListGrass.GetNumInList() > 0)
		{
			//cmd << "Try delete vobListGrass" << endl;
			vobListGrass.DeleteList();
		}

		if (entries.GetNumInList() > 0)
		{
			//cmd << "Try delete entries" << endl;
			entries.DeleteListDatas();
		}

		hiddenList.DeleteList();

		hiddenAmount = 0;
		bariCenter = zVEC3(0, 0, 0);


		//cmd << "Clear End" << endl;
	}



	void AB_NoGrass::CollectVobsFromLocation()
	{

		if (!hideActive)
		{

			return;
		}
		zCArray<zCVob*> vobList;

		ogame->GetWorld()->SearchVobListByClass(zCVob::classDef, vobList, 0);

		/*
		int radius = 500;
		zTBBox3D box;
		box.maxs = player->GetPositionWorld() + zVEC3(radius, radius, radius);
		box.mins = player->GetPositionWorld() - zVEC3(radius, radius, radius);


		zCArray<zCVob*>resVobList;
		ogame->GetWorld()->CollectVobsInBBox3D(vobList, box);
		*/


		//cmd << "zCVob Found in location: " << vobList.GetNumInList() << endl;


		int count = 0;

		for (int i = 0; i < vobList.GetNumInList(); i++) {

			zCVob* pVob = vobList.GetSafe(i);

			if (pVob) {

				if (//!pVob->collDetectionDynamic
						 pVob->GetVisual()
					&& pVob->homeWorld
					)
				{
					filteredList.Insert(pVob);
				}

			}
		}

		//cmd << "Filtered: " << filteredList.GetNumInList() << endl;

		HideObjects();
	}

	void AB_NoGrass::UpdateSettings()
	{
		//cmd << "================Update Settings grass================" << endl;


		RestoreObjects();
		Clear();
		CollectVobsFromLocation();
	}


}