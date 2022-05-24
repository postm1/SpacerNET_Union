// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	void AB_NoGrass::Init()
	{

		// список вобов, которые будем скрывать
		grassList.Insert("NW_NATURE_GRASSGROUP_01.3DS");
		grassList.Insert("NW_NATURE_GRASSGROUP_02.3DS");

		grassList.Insert("NW_NATURE_WATERGRASS_56P.3DS");
		grassList.Insert("NW_GRASSGROUP_01.3DS");
		grassList.Insert("NW_NATURE_GREAT_WEED_XGROUP.3DS");
		grassList.Insert("NW_GRASSGROUP_01.3DS");
		grassList.Insert("NW_NATURE_WATERGRASS_56W.3DS");


		grassList.Insert("AV_NATURE_BUSH_01.3DS");
		grassList.Insert("AV_NATURE_BUSH_02.3DS");
		grassList.Insert("ADDON_PLANTS_BUSH_01_36P.3DS");
		grassList.Insert("AV_NATURE_BUSH_02.3DS");


		grassList.Insert("NW_NATURE_FARNTEPPICH_306P.3DS");
		grassList.Insert("NW_NATURE_FARN_102P.3DS");
		grassList.Insert("NW_NATURE_FARNTEPPICH_612P.3DS");
		grassList.Insert("OW_NATURE_BUSH_BIG_01.3DS");
		grassList.Insert("OW_NATURE_BUSH_02.3DS"); // да, это тоже трава, название такое
		grassList.Insert("OW_NATURE_BUSH_03.3DS"); // да, это тоже трава, название такое
		
		grassList.Insert("NW_CAVE_PLANTS_01.3DS");
		grassList.Insert("NW_CAVE_PLANTS_02.3DS");
		grassList.Insert("NW_CAVE_PLANTS_03.3DS");
		grassList.Insert("NW_CAVE_PLANTS_04.3DS");
		grassList.Insert("NW_CAVE_PLANTS_05.3DS");

		grassList.Insert("XW_NATURE_BUSH_BIG_01.3DS");
		grassList.Insert("OW_NATURE_BUSH_BIG_02.3DS");
		

		grassList.Insert("GRASS_WUESTE1.3DS");
		grassList.Insert("GRASS_WUESTE2.3DS");
	
		grassList.Insert("NW_FERN_FOREST_CAVE_02_HL.3DS");
		grassList.Insert("KB_UNTERHOLZ_01.3DS");
		grassList.Insert("KB_UNTERHOLZ_02.3DS");
		grassList.Insert("KB_UNTERHOLZ_03.3DS");
		grassList.Insert("KB_UNTERHOLZ_04.3DS");
	
		//=============================================================

		bushList.Insert("NW_NATURE_LONG_BUSH_360P.3DS");
		bushList.Insert("NW_NATURE_LONG_BUSH_370P.3DS");
		bushList.Insert("GRASS_01.3DS");
		bushList.Insert("NW_NATURE_WATERGRASS_56P_NEW.3DS");
		bushList.Insert("NW_NATURE_PLANT_02.3DS");
		bushList.Insert("OW_LOB_BUSH_REED_V1.3DS");
		bushList.Insert("NW_NATURE_WATERGRASS_02_100P.3DS");
		bushList.Insert("NW_NATURE_WATERGRASS_02_900P.3DS");
		bushList.Insert("NW_NATURE_GRASSGROUP_01LPS_NEW.3DS");
		bushList.Insert("OW_LOB_BUSH_V7.3DS");
		bushList.Insert("NW_GROUNDCLOVER03.3DS");
		bushList.Insert("OW_BUSHES_01.3DS");

		bushList.Insert("OW_LOB_BUSH_V4.3DS");
		bushList.Insert("OW_LOB_BUSH_V5.3DS");
		bushList.Insert("OW_BUSHES_01.3DS");
		
		bushList.Insert("ADDON_CANYONPLANT_BUSHES_02_240P.3DS");
		bushList.Insert("ADDON_CANYONPLANT_BUSHES_01_120P.3DS");
		bushList.Insert("ADDON_CANYONPLANT_BUSHES_05_168P.3DS");
		
		
		/*
		zCArray<zSTRING> secondLevelList;
		secondLevelList.Insert("AV_NATURE_BUSH_01.3DS");
		secondLevelList.Insert("AV_NATURE_BUSH_02.3DS");
		secondLevelList.Insert("ADDON_PLANTS_BUSH_01_36P.3DS");
		secondLevelList.Insert("NW_NATURE_TANNE_100P.3DS");
		secondLevelList.Insert("NW_NATURE_TANNE_33P.3DS");
		*/


		hiddenAmount = 0;
		bariCenter = zVEC3(0, 0, 0);
		percent = 100;
		hideBush = true;
		hideGrass = true;
	}


	void AB_NoGrass::ManageObjects()
	{

	}

	void AB_NoGrass::HideObjects()
	{
		int count = vobListManage.GetNumInList();

		if (count == 0)
		{
			printWin("No vobs found");
			return;
		}

		if (percent == 0)
		{
			printWin("percent is 0");
			return;
		}

		hiddenAmount = 0;
		int maxDist = 0;

		//RX_Begin(50);

		for (int i = 0; i < count; i++)
		{
			zCVob* pVob = vobListManage.GetSafe(i);

			if (pVob)
			{
				zSTRING name = pVob->GetVisual()->GetVisualName().Upper();

				if (hideGrass && grassList.IsInList(name))
				{
					bariCenter += pVob->GetPositionWorld();
					//pVob->showVisual = 0;
					hiddenAmount += 1;
					vobListGrass.Insert(pVob);
				}
				else if (hideBush && (name.contains("NW_NATURE_BUSH") || name.contains("NW_NATURE_FARN") || bushList.IsInList(name)))
				{
					bariCenter += pVob->GetPositionWorld();
					//pVob->showVisual = 0;
					hiddenAmount += 1;
					vobListGrass.Insert(pVob);
				}
			}
		}

		if (vobListGrass.GetNumInList() == 0)
		{
			printWin("No grass found");
			return;
		}

		if (hiddenAmount > 0)
		{
			// считаем геометрический центр всех вобов
			bariCenter /= hiddenAmount;
		}


		// ищем самый удаленый воб, чтоб сделать квадрат нужного размера
		for (int i = 0; i < vobListGrass.GetNumInList(); i++)
		{
			zCVob* pVob = vobListGrass.GetSafe(i);

			if (pVob)
			{
				int dist = pVob->GetPositionWorld().Distance(bariCenter);

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

		//RX_End(50);

		//4 точки квадрата, который покрывает всю карту
		zVEC3 first = bariCenter + zVEC3(maxDist * cos(PI / 4), 0, maxDist * sin(PI / 4));
		zVEC3 second = bariCenter + zVEC3(maxDist * cos(PI / 4 + PI / 2), 0, maxDist * sin(PI / 4 + PI / 2));
		zVEC3 third = bariCenter + zVEC3(maxDist * cos(5 * PI / 4), 0, maxDist * sin(5 * PI / 4));
		zVEC3 forth = bariCenter + zVEC3(maxDist * cos(-PI / 4), 0, maxDist * sin(-PI / 4));


		int time = 7000;

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


		if (entries.GetNumInList() > 0)
		{
			entries.DeleteListDatas();
		}


		int num = 0;

		//RX_Begin(51);
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
				zCOLOR col;

				switch (i)
				{
				case 0: { col = GFX_RED; break; };
				case 1: { col = GFX_YELLOW; break; };
				case 2: { col = GFX_GREEN; break; };
				case 3: { col = GFX_WHITE; break; };
				default: { col = GFX_BLUE; break; };
				}

				/*
				debug.AddLine(entry->p1, entry->p2, col, time);
				debug.AddLine(entry->p2, entry->p3, col, time);
				debug.AddLine(entry->p3, entry->p4, col, time);
				debug.AddLine(entry->p4, entry->p1, col, time);
				*/

				num += 1;


			}
		}

		//RX_End(51);
		//XYZ

		/*
		debug.AddLine(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(200, 0, 0), GFX_RED, 60000, false, "", "X");
		debug.AddLine(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(0, 200, 0), GFX_YELLOW, 60000, false, "", "Y");
		debug.AddLine(player->GetPositionWorld(), player->GetPositionWorld() + zVEC3(0, 0, 200), GFX_RED, 60000, false, "", "Z");
		*/




		//RX_Begin(52);
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
						entry->pList.InsertEnd(pVob);

						//debug.AddLine(pVob->GetPositionWorld(), pVob->GetPositionWorld() + zVEC3(0, 3000, 0), GFX_RED, time, 0, "", Z k);
						flag = true;
						break;
					}
				}

				if (!flag)
				{
					printWin("No cell found for... ");
					//debug.AddLine(pVob->GetPositionWorld(), pVob->GetPositionWorld() + zVEC3(0, 3000, 0), GFX_RED, time);
				}
			}
		}
		//RX_End(52);
		//cmd << "Entries" << endl;


		//RX_Begin(53);
		for (int k = 0; k < entries.GetNumInList(); k++)
		{
			auto entry = entries.GetSafe(k);

			if (entry && entry->pList.GetNumInList() > 0)
			{
				//cmd << entry->pList.GetNumInList() << endl;
				int numberToSelect = (entry->pList.GetNumInList() * percent) / 100;

				if (percent == 100)
				{
					for (int z = 0; z < entry->pList.GetNumInList(); z++)
					{
						entry->pList.GetSafe(z)->showVisual = 0;
					}
					continue;
				}

				//cmd << "entry: " << k << "; Number " << numberToSelect << endl;

				while (numberToSelect > 0)
				{
					int x = rand() % entry->pList.GetNumInList();

					if (entry->pList.GetSafe(x) && entry->pList.GetSafe(x)->showVisual)
					{
						entry->pList.GetSafe(x)->showVisual = 0;
						numberToSelect -= 1;
					}
				}
			}
		}

		/*
		RX_End(53);
		cmd << "Perf50: " << RX_PerfString(50) << endl;
		cmd << "Perf51: " << RX_PerfString(51) << endl;
		cmd << "Perf52: " << RX_PerfString(52) << endl;
		cmd << "Perf53: " << RX_PerfString(53) << endl;

		*/

		//printWin("Max dist: " + Z maxDist);
		printWin("width: " + Z width);
		printWin("countCells: " + Z countCells);
	}


	void AB_NoGrass::ShowObjects()
	{
		int count = vobListManage.GetNumInList();

		for (int i = 0; i < count; i++)
		{
			zCVob* pVob = vobListGrass.GetSafe(i);

			if (pVob)
			{
				pVob->showVisual = 1;
			}
		}

		Clear();
	}

	void AB_NoGrass::Clear()
	{
		if (vobListManage.GetNumInList() > 0)
		{
			vobListManage.DeleteList();
		}

		if (vobListGrass.GetNumInList() > 0)
		{
			vobListGrass.DeleteList();
		}

		if (ignoreList.GetNumInList() > 0)
		{
			ignoreList.DeleteList();
		}

		if (entries.GetNumInList() > 0)
		{
			entries.DeleteListDatas();
		}

		hiddenAmount = 0;
		bariCenter = zVEC3(0, 0, 0);
	}



	void AB_NoGrass::CollectVobsFromLocation()
	{
		printWin("CollectVobsFromLocation");

		Clear();

		zCArray<zCVob*> vobList;

		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, vobList, 0);

		/*
		int radius = 500;
		zTBBox3D box;
		box.maxs = player->GetPositionWorld() + zVEC3(radius, radius, radius);
		box.mins = player->GetPositionWorld() - zVEC3(radius, radius, radius);


		zCArray<zCVob*>resVobList;
		ogame->GetWorld()->CollectVobsInBBox3D(vobList, box);
		*/


		int count = 0;

		for (int i = 0; i < vobList.GetNumInList(); i++) {

			zCVob* pVob = vobList.GetSafe(i);

			if (pVob) {
				// невидимые мобинтеры в игнор и прочие вобы у кого !showVisual
				if (!pVob->showVisual && pVob->CastTo<oCVob>() && !pVob->CastTo<oCItem>() && !pVob->CastTo<oCNpc>())
				{
					ignoreList.Insert(pVob);
				}
				else
				{
					if (!pVob->collDetectionDynamic && pVob->GetVisual()/* || pVob->type == zTVobType::zVOB_TYPE_ITEM*/)
					{
						vobListManage.Insert(pVob);
					}
				}
			}
		}

		HideObjects();
	}

	void AB_NoGrass::Loop()
	{
		return;

		if (zKeyPressed(KEY_F1)) {
			zinput->ClearKeyBuffer();

			ShowObjects();
			CollectVobsFromLocation();

		}

		if (zKeyPressed(KEY_F2)) {
			zinput->ClearKeyBuffer();

			ShowObjects();

		}

		if (zKeyPressed(KEY_NUMPADPLUS)) {
			zinput->ClearKeyBuffer();
			percent += 10;
			zClamp(percent, 0, 100);
		}

		if (zKeyPressed(KEY_NUMPADMINUS)) {
			zinput->ClearKeyBuffer();
			percent -= 10;
			zClamp(percent, 0, 100);
		}

		if (!ogame->singleStep)
		{
			PrintDebug("Ignore list: " + Z ignoreList.GetNum());
			PrintDebug("Candidates list: " + Z vobListManage.GetNum());
			PrintDebug("Hidden amount: " + Z hiddenAmount);
			PrintDebug("percent: " + Z percent);
		}

	}
}