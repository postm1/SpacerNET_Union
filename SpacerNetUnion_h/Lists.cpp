// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	// список итемов в окно
	void SpacerApp::CreateItemsList()
	{
		auto addItem = (callVoidFunc)GetProcAddress(theApp.module, "AddItemToList");

		zSTRING scriptClassName = "C_ITEM";

		int pos, typ, anz, c, classindex;

		classindex = parser->GetIndex(scriptClassName);
		if (classindex != -1)
		{
			parser->GetSymbolInfo(classindex, typ, anz);
			pos = parser->GetInstance(classindex, 0);


			while (pos >= 0)
			{
				zSTRING s = parser->GetSymbolInfo(pos, typ, c);
				// instance_list.Insert(s);
				Stack_PushString(s.Upper());
				addItem();
				pos = parser->GetInstance(classindex, pos + 1);
			}
		}

		auto sort = (voidFuncPointer)GetProcAddress(theApp.module, "SortItems");
		sort();
	}


	void SpacerApp::CreateMobNameList()
	{
		zCArray<zSTRING> mobNames;


		cmd << "Looking for mobnames..." << endl;


		//MOBNAME_SIGH
		for (int i = 0; i < parser->symtab.table.GetNumInList(); i++)
		{
			if (parser->symtab.table.GetSafe(i)->name.Search("MOBNAME_", 1) != -1)
			{
				mobNames.Insert(parser->symtab.table.GetSafe(i)->name);
			}
		}

		for (int i = 0; i < mobNames.GetNumInList(); i++)
		{
			//cmd << CString(mobNames.GetSafe(i)) << endl;
		}

	}
	// список всех визуалов из файловой системы игры
	void SpacerApp::CreateVisualsList()
	{


		CreateFileList();

		auto addVisuals = (callVoidFunc)GetProcAddress(theApp.module, "AddVisualToListVDF");

		for (int i = 0; i < modelsStrVDF.GetNum(); i++)
		{
			Stack_PushString(modelsStrVDF.GetSafe(i).Upper());
			addVisuals();
		}


		addVisuals = (callVoidFunc)GetProcAddress(theApp.module, "AddVisualToListWORK");

		for (int i = 0; i < modelsStrWORK.GetNum(); i++)
		{
			Stack_PushString(modelsStrWORK.GetSafe(i));
			addVisuals();
		}



		auto sort = (callVoidFunc)GetProcAddress(theApp.module, "SortVisuals");
		sort();

		modelsStrWORK.DeleteList();
		modelsStrVDF.DeleteList();
	}

	// список PFX в окно CreateVisualsList
	void SpacerApp::CreatePFXList()
	{
		auto addPFX = (callVoidFunc)GetProcAddress(theApp.module, "AddPacticleToList");

		for (int i = 0; i<zCParticleFX::s_emitterPresetList.GetNumInList(); i++)
		{
			Stack_PushString(zCParticleFX::s_emitterPresetList.GetSafe(i)->particleFXName.Upper());
			addPFX();
		}
		auto sort = (callVoidFunc)GetProcAddress(theApp.module, "SortPFX");
		sort();
	}

	// список Sounds в окно
	void SpacerApp::CreateSoundList()
	{
		auto addSound = (callVoidFunc)GetProcAddress(theApp.module, "AddSoundToList");


		zCParser* soundParser = zsound->GetSFXParser();;
		soundParser->SetStopOnError(FALSE);
		zoptions->ChangeDir(DIR_SCRIPTS);

		zSTRING classname = "C_SFX";

		int pos, typ, anz, c, classindex;
		zSTRING s;

		classindex = soundParser->GetIndex(classname);
		if (classindex != -1)
		{
			// Get instances
			soundParser->GetSymbolInfo(classindex, typ, anz);
			pos = soundParser->GetInstance(classindex, 0);
			while (pos >= 0)
			{
				s = soundParser->GetSymbolInfo(pos, typ, c);
				Stack_PushString(s);
				addSound();
				pos = soundParser->GetInstance(classindex, pos + 1);
			}

		}



		(voidFuncPointer)GetProcAddress(theApp.module, "SortSounds")();
	}

	void SpacerApp::BuildTree()
	{
		zCWorld* world = ogame->GetWorld();

		(callVoidFunc)GetProcAddress(theApp.module, "ClearAllEntries")();


		zCTree<zCVob>* tree = world->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateBaseVobTree(tree);
			tree = tree->GetNextChild();
		}
		treeIsReady = true;
		std::cout << "Union: Creating TreeView in C#..." << std::endl;
		CreateTree();





	}


	void CreateBaseVobTreeNew(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();


		if (vob)
		{

			if (vob->GetVisual())
			{
				vob->GetVisual()->Release();
			}
			vob->SetShowVisual(FALSE);

			vob->RemoveVobFromBspTree();
			//if ((vob->GetVisual()) && (vob->GetHomeWorld()))	vob->GetVisual()->HostVobRemovedFromWorld(vob, vob->GetHomeWorld());
			//vob->RemoveVobFromGlobalVobTree();
		}


		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateBaseVobTreeNew(node);
			node = node->GetNextChild();
		}
	}


	void SaveCurrentWorldToMSH()
	{
		// текущий мир
		zCWorld* pWorld = ogame->GetWorld();

		// текущий мир объектов
		oCWorld* oWorld = ogame->GetGameWorld();

		// если указателей нет, то выходим
		if (!pWorld || !oWorld) return;



		zCPolygon::S_ResetMorphedVerts();
		zCVertex::ResetVertexTransforms();

		cmd << Col16(CMD_YELLOW) << "Start removing" << endl;
		zCTree<zCVob>* tree = pWorld->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateBaseVobTreeNew(tree);
			tree = tree->GetNextChild();
		}

		cmd << Col16(CMD_YELLOW) << "End removing" << endl;
		// часть меша, которая будет сохраняться
		zCMesh* pMesh = zNEW(zCMesh);

		// максимальное кол-во полигонов в одной части сохраняемого меша
		int nPolyMax = 20000;

		// выделяем память под полигоны
		pMesh->AllocPolys(nPolyMax);

		// выделяем память под вертексы
		pMesh->AllocVerts(nPolyMax * 3);

		// счётчк вставленых в меш полигонов
		int nPoly = 0;

		// порядковый номер сохраняемого меша
		int nSaved = 1;


		// пробегаемся по всем зонам(или лепесткам дерева)
		for (int i = 0; i < pWorld->bspTree.numLeafs; i++)
		{
			// теперь пробегаемся по всем полигонам в текущей зоне (или лепестке)
			for (int j = 0; j < pWorld->bspTree.leafList[i].numPolys; j++)
			{

				// берём полигон
				zCPolygon* pPoly = pWorld->bspTree.leafList[i].polyList[j];

				// если указатель в порядке
				if (pPoly)
				{
					// добавляем 3 вертекса в сохраняемый меш, взяв координаты от текущего полигона мира
					// (получаем указатели на добавленные вертексы)
					zCVertex* vert1 = pMesh->AddVertex(pPoly->vertex[0]->position);
					zCVertex* vert2 = pMesh->AddVertex(pPoly->vertex[1]->position);
					zCVertex* vert3 = pMesh->AddVertex(pPoly->vertex[2]->position);

					// добавляем полигон в меш, используя ранее полученные указатели на вертексы,
					// чтобы привязать вертексы к данному полигону
					// (получаем указатель на добавленный полигон)
					zCPolygon* poly = pMesh->AddPoly(vert1, vert2, vert3);

					// применяем к полигону такой же материал как и у полигона, взятого из мира
					poly->material = pPoly->material;

					// проверяем счётчик добавленных полигонов
					// если вышли за пределы максимального кол-ва
					// добавляемых полигонов в меш
					if (nPoly >= nPolyMax)
					{
						// формируем название сохраняемой части меша
						string saveName = A "_" + A oWorld->worldName + A "_PART" + A nSaved + A ".MSH";

						// сохраняем меш
						pMesh->SaveMSH(saveName.ToChar());

						// выводим инфо в консоль
						cmd << Col16(CMD_YELLOW) << saveName << Col16(CMD_WHITE) << " saved" << endl;

						// увеличиваем счётчик сохранённых частей
						nSaved++;

						// сбрасываем счётчик
						nPoly = 0;

						// создаём новый меш. Из за этого возможна утечка памяти!
						// способа удалить старый - пока не знаю.
						pMesh = zNEW(zCMesh);
					}

					// увеличиваем счётчик полигонов
					nPoly++;
				}
			}
		}

		// если цикл закончился, но остались несохранённые полигоны, то
		// до сохраняем остатки меша в последний файл
		if (nPoly > 1)
		{
			// формируем название сохраняемой части меша
			string saveName = A "_" + A oWorld->worldName + A "_PART" + A nSaved + A ".MSH";

			// сохраняем меш
			pMesh->SaveMSH(saveName.ToChar());

			// выводим инфо в консоль
			cmd << Col16(CMD_YELLOW) << saveName << Col16(CMD_WHITE) << " saved" << endl << "\nIt's all!" << endl;
		}
	}




	void CreateWaypointVobTree(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		static AddGlobalEntryPointer addEntry = NULL;

		if (!addEntry)
		{
			addEntry = (AddGlobalEntryPointer)GetProcAddress(theApp.module, "AddGlobalEntry");
		}


		if (dynamic_cast<zCVobWaypoint*>(vob))
		{
			Stack_PushString(vob->_GetClassDef()->className);
			Stack_PushString(GetVobName(vob));

			addEntry((uint)vob, vob->GetParent());
		}

		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateWaypointVobTree(node);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::RebuiltWaypoints()
	{
		zCWorld* world = ogame->GetWorld();

		(callVoidFunc)GetProcAddress(theApp.module, "ClearWaypoints")();


		zCTree<zCVob>* tree = world->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateWaypointVobTree(tree);
			tree = tree->GetNextChild();
		}

		(callVoidFunc)GetProcAddress(theApp.module, "ReloadWaypoint")();
	}
}