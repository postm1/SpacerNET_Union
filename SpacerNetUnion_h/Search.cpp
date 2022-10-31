

// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	const zSTRING	LIST_SEP = "----------";
	const zSTRING	LIST_ABSTRACT = "[abstract class]";
	const zSTRING	LIST_CORRUPT = "[corrupt class]";
	const zSTRING	LIST_SCRIPTED = "(scripted)";
	const char		LIST_PREFIX = '.';
	const char		LIST_POSTFIX = ' ';


	void SpacerApp::SearchSelectVob(zCVob* pickedVob)
	{
		if (!pickedVob || dynamic_cast<zCVobLevelCompo*>(pickedVob))
		{
			// dialog leer
			search.cur_vob = NULL;
			search.cur_vob_convert = NULL;
			Stack_PushString("");
		}
		else
		{

			zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, 0, 0);
			arch->SetStringEOL(zSTRING("\n"));
			arch->WriteObject(pickedVob);
			zSTRING archBuffer;
			arch->GetBufferString(archBuffer);

			Stack_PushString(archBuffer);

			arch->Close();
			zRELEASE(arch);

		}
	}

	void SpacerApp::SearchFillVobClass(CString vobClass, bool isNewType)
	{

		if (isNewType)
		{

			if (search.cur_vob_convert)
			{
				zRELEASE(search.cur_vob_convert);
			}

			search.cur_vob_convert = (zCVob*)zCObject::CreateNewInstance(vobClass);
			SearchSelectVob(search.cur_vob_convert);
		}
		else
		{
			if (search.cur_vob)
			{
				zRELEASE(search.cur_vob);
			}

			search.cur_vob = (zCVob*)zCObject::CreateNewInstance(vobClass);
			SearchSelectVob(search.cur_vob);
		}



	}

	CString Characters(char ch, int anzahl)
	{
		CString newStr = "";
		for (int i = 0; i < anzahl; i++)
		{
			newStr += ch;
		}
		return newStr;
	}


	void BuildClassHierarchySearch(zCClassDef *parentClassDef, int depth)
	{
		static auto callFunc = (callVoidFunc)GetProcAddress(theApp.module, "AddSubClassConvert");

		depth++;
		CString baseName;
		CString s;

		// Abgeleitete Klassen von parentClassDef ermitteln
		for (int i = 0; i < zCClassDef::GetNum(); i++)
		{
			zCClassDef* classDef = zCClassDef::GetClassDefByIndex(i);

			if (classDef->GetBaseClassDef() == parentClassDef) {

				zBOOL baseOK;
				if (classDef->GetBaseClassDef())
				{
					baseName = A classDef->GetBaseClassDef()->GetClassName_();
					baseOK = (baseName == A classDef->GetBaseClassName());
				}
				else {
					baseOK = (classDef->GetBaseClassName() == "NULL");
				};

				s = A classDef->GetClassName_();
				if (!baseOK) s = s + A LIST_POSTFIX + A LIST_CORRUPT;

				// Eigenschaften
				if (classDef->IsAbstractClass()) s = s + A LIST_POSTFIX + A LIST_ABSTRACT;
				if (classDef->IsScriptedClass()) s = s + A LIST_POSTFIX + A LIST_SCRIPTED;
				// Einruecken
				s = A Characters(LIST_PREFIX, depth * 2 - 2) + s;
				//s = Characters(LIST_PREFIX, depth * 2 - 2) + s;
				// Den Klassenbezeichner der entsprechenden Listebox hinzufuegen

				Stack_PushString(s);
				callFunc();

				BuildClassHierarchySearch(classDef, depth);
			};
		};
	}

	void SpacerApp::SearchGetSubClasses(CString className)
	{

		static auto callFunc = (callVoidFunc)GetProcAddress(theApp.module, "AddSubClassConvert");

		if (search.cur_vob)
		{
			int c = 0;
			zBOOL found = FALSE;
			zCClassDef* vobClass = 0;
			while (c < zCClassDef::GetNum() && !found)
			{
				vobClass = zCClassDef::GetClassDefByIndex(c);
				found = zBOOL(A vobClass->GetClassName_() == className);
				if (!found) c++;
			}

			if (found)
			{
				int entryCount = 0;
				Stack_PushString(vobClass->GetClassName_());
				callFunc();
				entryCount++;
				Stack_PushString("--------up-classing-------");
				callFunc();
				entryCount++;

				int count = 0;
				zCClassDef* classDef = vobClass;
				while (classDef && classDef != zCVob::classDef)
				{
					classDef = classDef->GetBaseClassDef();
					if (classDef) count++;
				};

				int ins_pos = entryCount;
				classDef = vobClass;
				CString s;
				while (classDef && classDef != zCVob::classDef)
				{
					classDef = classDef->GetBaseClassDef();
					if (classDef)
					{
						count--;
						s = Characters(LIST_PREFIX, count * 2) + A classDef->GetClassName_();
						if (classDef->IsAbstractClass()) s = s + A LIST_ABSTRACT;

						entryCount++;
						Stack_PushString(s);
						callFunc();
					}
				}

				Stack_PushString("--------down-classing-------");
				callFunc();

				BuildClassHierarchySearch(vobClass, 1);
			}

		}
	}

	zCArray<zCVob*> resultFound;
	zCVob* currentConvertVob = NULL;

	bool SpacerApp::SearchHandleVob(zCVob*& vob, int selectedCount, int onlyVisualOrName)
	{

		static auto compare = (compareVobs)GetProcAddress(theApp.module, "CompareVobs");
		static auto compareByNameOrVisual = (compareVobs)GetProcAddress(theApp.module, "CompareByNameOrVisual");

		if (!vob) return false;

		//если выбрано 1 поле, то делаем быстрый поиск сначала
		if (onlyVisualOrName > 0)
		{
			zSTRING vobName = vob->GetVobName();
			zSTRING visualName = vob->GetVisual() ? vob->GetVisual()->GetVisualName() : "";


			//cmd << vobName << "|" << visualName << endl;
			
			Stack_PushString(vobName);
			Stack_PushString(visualName);

			if (compareByNameOrVisual())
			{
				resultFound.Insert(vob);
				return true;
			}
				
			

			return false;
		}

		zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, 0, 0);
		arch->SetStringEOL(zSTRING("\n"));
		arch->WriteObject(vob);
		zSTRING archBuffer;
		arch->GetBufferString(archBuffer);

		Stack_PushString(archBuffer);

		arch->Close();
		zRELEASE(arch);

		

		if (compare())
		{
			resultFound.Insert(vob);
		}

		return true;
	}

	void SpacerApp::SearchDoConvert(CString prop)
	{

		//cmd << prop << endl;

		zCBuffer zbuf(prop.ToChar(), prop.Length());

		zCArchiver* arch = zarcFactory->CreateArchiverRead(&zbuf, 0);
		arch->SetStringEOL(zSTRING("\n"));
		arch->ReadObject(currentConvertVob);
		arch->Close();
		zRELEASE(arch);
	}

	bool SpacerApp::SearchHandleConvert(zCVob*& vob)
	{
		static auto convert = (compareVobs)GetProcAddress(theApp.module, "ConvertVobs");

		if (!vob) return false;


		zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, 0, 0);
		arch->SetStringEOL(zSTRING("\n"));
		arch->WriteObject(vob);
		zSTRING archBuffer;
		arch->GetBufferString(archBuffer);

		Stack_PushString(archBuffer);

		arch->Close();
		zRELEASE(arch);

		currentConvertVob = vob;


		return convert();

	}

	static int Compare_VobNames(const void *arg1, const void *arg2)
	{
		zCVob* pVob1 = *((zCVob**)arg1);
		zCVob* pVob2 = *((zCVob**)arg2);

		//cmd << "Compare try: ";

		zSTRING name1 = pVob1->GetVobName();
		zSTRING name2 = pVob2->GetVobName();

		

		int result = strcmp(name1, name2);

		//cmd << name1 << ";" << name2 << " Result: " << result << endl;

		return result;

	}

	int SpacerApp::SearchFillVobClass(bool derived, bool hasChildren, int type, int selectedCount, int onlyVisualOrName, int matchNames)
	{
		static auto callFunc = (addToVobList)GetProcAddress(theApp.module, "AddSearchVobResult");
		int resultCount = 0;
		Array<uint> arr;


		//cmd << "SearchFillVobClass: derived " << derived << " hasChildren " << hasChildren << " type: " << type 
		//	<< " sel: " << selectedCount << " onlyVisOrName: " << onlyVisualOrName << endl;

		SearchVobType searchType = (SearchVobType)type;

		zCArray<zCVob*> result;
		resultFound.DeleteList();


		zCVob* curTempVob = NULL;

		curTempVob = search.cur_vob;

		if (!curTempVob)
		{
			//cmd << "can't search" << endl;
			return 0;
		}


		exports.toggleUIElement(UIElementType::UI_LIST_SEARCH_RESULT, FALSE);

		//cmd << "Search: " << curTempVob->GetClassDef()->className << endl;

		ogame->GetWorld()->SearchVobListByClass(curTempVob->GetClassDef(), result, 0);

		if (derived)
		{
			ogame->GetWorld()->SearchVobListByBaseClass(curTempVob->GetClassDef(), result, 0);
		}

		//cmd << "result: " << result.GetNum() << endl;

		result.RemoveDoubles();
		int num = result.GetNumInList();


		
		Common::Map<CString, zCVob*> vobsNamesMap;
		zCArraySort<zCVob*> resultSorted;

		for (int i = 0; i < num; i++)
		{
			auto pVobCur = result[i];

			if (!pVobCur)continue;

			if (dynamic_cast<zCVobLevelCompo*>(pVobCur))	continue;
			if (pVobCur == ogame->GetCamera()->GetVob())	continue;


			// поиск дублей имен
			if (matchNames)
			{
				

				auto name = pVobCur->GetVobName();

				if (name.Length() > 0)
				{
					auto& foundPair = vobsNamesMap[name];

					if (!foundPair.IsNull())
					{
						resultSorted.Insert(pVobCur);
						resultSorted.Insert(foundPair.GetValue());
					}
					else
					{
						vobsNamesMap.Insert(name, pVobCur);
					}

					
				}
				continue;
			}
		

			if (selectedCount > 0)
			{
				if (hasChildren)
				{
					if (result[i]->HasChildren())
					{
						SearchHandleVob(result[i], selectedCount, onlyVisualOrName);
						
					}
				}
				else
				{
					SearchHandleVob(result[i], selectedCount, onlyVisualOrName);
				}
				
			}
			else
			{
				if (hasChildren)
				{
					if (result[i]->HasChildren())
					{
						resultFound.Insert(result[i]);
					}
				}
				else
				{
					resultFound.Insert(result[i]);
				}
				
			}

		}

		if (matchNames && resultSorted.GetNumInList() > 0)
		{

			resultSorted.RemoveDoubles();

			//cmd << "found names: " << resultSorted.GetNumInList() << endl;

			

			if (resultSorted.GetNumInList() >= 2)
			{
				resultSorted.SetCompare(Compare_VobNames);

				resultSorted.BestSort();


				//cmd << "Sorting..." << endl;
			}
			
			resultFound.DeleteList();

			for (int i = 0; i < resultSorted.GetNumInList(); i++)
			{
				resultFound.InsertEnd(resultSorted[i]);

				//cmd << resultSorted[i]->GetVobName() << endl;
			}
		}

		// search
		if (searchType == SearchVobType::Search)
		{
			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				zCVob* vob = resultFound[i];

				if (vob && arr.SearchEqual((uint)vob) == Invalid)
				{
					Stack_PushString(GetVobName(vob));

					arr.Insert((uint)vob);

					callFunc((uint)vob);

					resultCount += 1;
				}


			}
		}

		//convert
		if (searchType == SearchVobType::Convert)
		{

		
			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				zCVob* vob = resultFound[i];

				if (vob && arr.SearchEqual((uint)vob) == Invalid)
				{
					//cmd << "Replace vob: " << GetVobName(vob) << endl;

					if (SearchHandleConvert(resultFound[i]))
					{
						resultCount += 1;
					}

					arr.Insert((uint)vob);
				}

			}

	

			//cmd << "Replace vobs: " << A resultCount << endl;
		}


		if (searchType == SearchVobType::ReplaceVobTree)
		{
			theApp.SetSelectedVob(NULL);

			zSTRING path = Stack_PeekString();
			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);

			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				zCVob* vob = resultFound[i];

				zCZone* isZone = dynamic_cast<zCZone*>(vob);

				if (vob && !isZone && arr.SearchEqual((uint)vob) == Invalid)
				{
					zMAT4 trafo = vob->trafoObjToWorld;

					zCVob* parentVob = 0;
					if (vob->globalVobTreeNode && vob->globalVobTreeNode->GetParent())
					{
						parentVob = vob->globalVobTreeNode->GetParent()->GetData();
					}

					zCVob* newvob = ogame->GetWorld()->MergeVobSubtree(path, parentVob, zCWorld::zWLD_LOAD_MERGE_ADD);

					if (newvob)
					{
						zBOOL lcddyn = newvob->GetCollDetDyn();
						zBOOL lcdstat = newvob->GetCollDetStat();
						newvob->SetCollDet(FALSE);
						newvob->SetTrafoObjToWorld(trafo);
						newvob->SetCollDetDyn(lcddyn);
						newvob->SetCollDetStat(lcdstat);

					}
					else
					{

					};

					if (newvob) // vob loeschen, wenn ersetzt
					{
						vob->SetDrawBBox3D(FALSE);
						arr.Insert((uint)vob);
						RemoveVob(vob);
						vob = NULL;
						resultCount++;
					}
				}

			}

			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
			//MessageBox(0, "Замен: " + A resultFound.GetNum(), 0, 0);
		}



		// remove
		if (searchType == SearchVobType::Remove)
		{

			//cmd << "All found: " << A resultFound.GetNum() << endl;

			theApp.SetSelectedVob(NULL);
			zCVob* vob = NULL;
			zCZone* isZone = NULL;


			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);

			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				vob = resultFound[i];

				isZone = dynamic_cast<zCZone*>(vob);

				if (vob && !isZone && !arr.HasEqual((uint)vob) && IsValidZObject(vob))
				{
					//vob->SetDrawBBox3D(FALSE);
					arr.Insert((uint)vob);

					RemoveVob(vob);
					//vob->RemoveVobSubtreeFromWorld();
					vob = NULL;
					resultCount += 1;
				}

				isZone = NULL;
				vob = NULL;
			}


			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
		}

		if (searchType == SearchVobType::DynColl)
		{
			theApp.SetSelectedVob(NULL);
			zCVob* vob = NULL;
			zCZone* isZone = NULL;

			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				vob = resultFound[i];

				isZone = dynamic_cast<zCZone*>(vob);

				if (vob && !isZone && !arr.HasEqual((uint)vob) && IsValidZObject(vob))
				{
					
					arr.Insert((uint)vob);
					vob->SetCollDetDyn(!vob->GetCollDetDyn());
					resultCount += 1;
				}

				isZone = NULL;
				vob = NULL;
			}
		}


		exports.toggleUIElement(UIElementType::UI_LIST_SEARCH_RESULT, TRUE);

		//cmd << A resultCount << endl;
		return resultCount;
	}

	typedef zCParticleEmitter *(*fptr)(class zSTRING const &);

	zCArraySort<zCParticleEmitter*>&s_emitterPresetList = *(zCArraySort<zCParticleEmitter*>*)0x008D8E0C;
	oCParticleControl*& pfxcGlobal = *(oCParticleControl**)0x00AB088C;

	zCArray<oCVisualFX*>&s_worldVisFXList = *(zCArray<oCVisualFX*>*)0x008CE634;
	zCParser*& visualParser = *(zCParser**)0x008CE6EC;
	zCParser*& s_pfxParser = *(zCParser**)0x008D9234;
	fptr SearchParticleEmitter = (fptr)0x005ADDE0;


	zCParticleFX* m_pPfx = NULL;
	int instanceFieldSize = 0;
	zCVob* testVob = NULL;
	zCWorld*	m_pWorld;
	CString currentPFXName;


	void SpacerApp::GetAllPfx()
	{
		int size = s_emitterPresetList.GetNumInList();

		auto addPFX = (callVoidFunc)GetProcAddress(this->module, "AddPfxInstancemName");



		for (int i = 0; i < size; i++)
		{
			Stack_PushString(s_emitterPresetList.GetSafe(i)->particleFXName);
			addPFX();
		}
	}

	void SpacerApp::UpdatePFXField()
	{
		CString name = currentPFXName;
		CString pfxFieldName = Stack_PeekString();
		pfxFieldName.Upper();


		if (!m_pPfx || !m_pPfx->emitter)
		{
			return;
		}

		zCPar_Symbol* ps = s_pfxParser->GetSymbol(s_pfxParser->GetIndex(name));

		if (!ps)
		{
			Message::Box("No PFX found with the name: " + name);
			return;
		}

		int index = s_pfxParser->GetIndex(name);

		int baseClassIndex = s_pfxParser->GetBaseClass(index);
		int indClass = s_pfxParser->GetIndex(name);

		zCPar_Symbol* base = s_pfxParser->GetSymbol(baseClassIndex);
		zCPar_Symbol* pfx = s_pfxParser->GetSymbol(indClass);

		void* addr = m_pPfx->emitter;
		int type = 0;

		if (!addr)
		{
			printWin("PFX instance pointer is NULL");
			return;
		}
		bool found = false;


		instanceFieldSize = base->ele;
		//Message::Box("FieldsSize: " + ToStr instanceFieldSize);

		//MessageBox(0, zSTRING(base->m_sName) + zSTRING(base->f.tNumber), 0, 0);
		for (int i = 0; i < instanceFieldSize; i++)
		{
			// берем следующие base->f.tNumber символов, они и являются полями инстанции
			zCPar_Symbol* param = s_pfxParser->GetSymbol(baseClassIndex + i + 1);

			if (!param)
			{
				continue;
			}

			zSTRING sName = param->name.Upper();

			int pos = sName.Search(".", 1);
			//RX_FIX
			//if (pos > 0)
				//sName = param->name.Substr(pos + 1, 255);

			type = param->type;

			if (CString(sName) == pfxFieldName)
			{
				if (type == zPAR_TYPE_FLOAT)
				{
					float val = Stack_PeekFloat();
					*((float*)addr + param->GetOffset()) = val;
					cmd << "UpdateField: " << sName << " " << type << " " << zSTRING(val, 20) << endl;
				}
				else if (type == zPAR_TYPE_INT)
				{
					int val = Stack_PeekInt();
					*((int*)addr + param->GetOffset()) = val;
					cmd << "UpdateField: " << sName << " " << type << " " << zSTRING(val) << endl;
				}
				else if (type == zPAR_TYPE_STRING)
				{
					CString val = Stack_PeekString();
					*(zSTRING*)((BYTE*)addr + param->GetOffset()) = val.ToChar();
					cmd << "UpdateField: " << sName << " " << type << " " << val << endl;
				}

				


				if (m_pPfx && m_pPfx->emitter)
				{
					m_pPfx->emitter->UpdateInternals();
				}
				
				found = true;
				break;
			}
		}

		if (!found)
		{
			Message::Box("UpdateInternals fail! No field found");
		}
	}

	
	void SpacerApp::GetPFXInstanceProps(CString name)
	{
		//auto addPFXAddr = (callVoidFunc)GetProcAddress(this->module, "AddPfxAddr");

		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();
			RELEASE_OBJECT(m_pPfx);
		}

		if (testVob)
		{
			testVob->RemoveVobFromWorld();
		}

		testVob = new zCVob();
		m_pWorld = ogame->GetGameWorld();
		testVob->SetVobName("Vob_PFX_Editor");
		m_pWorld->AddVob(testVob);
		testVob->SetCollDet(FALSE);
		zVEC3 pos = ogame->GetCamera()->connectedVob->GetAtVectorWorld() * 250 + ogame->GetCamera()->connectedVob->GetPositionWorld();
		testVob->Move(pos[0], pos[1], pos[2]);


		
		m_pPfx = new zCParticleFX();
		m_pPfx->SetEmitter(SearchParticleEmitter(name), FALSE);
		testVob->SetVisual(m_pPfx);

		if (testVob && m_pPfx && m_pPfx->emitter)
		{
			m_pPfx->SetAndStartEmitter(SearchParticleEmitter(name.Upper()), FALSE);
			m_pPfx->dontKillPFXWhenDone = true;
		}

		zCPar_Symbol* ps = s_pfxParser->GetSymbol(s_pfxParser->GetIndex(name));
		currentPFXName = name;

		if (!ps)
		{
			Message::Box("No PFX found with the name: " + name);
			return;
		}

		

		int index = s_pfxParser->GetIndex(name);

		int baseClassIndex = s_pfxParser->GetBaseClass(index);
		int indClass = s_pfxParser->GetIndex(name);

		zCPar_Symbol* base = s_pfxParser->GetSymbol(baseClassIndex);
		zCPar_Symbol* pfx = s_pfxParser->GetSymbol(indClass);

		void* addr = m_pPfx->emitter;
		int type = 0;

		if (!addr)
		{
			printWin("PFX instance pointer is NULL");
			return;
		}



		instanceFieldSize = base->ele;
		//Message::Box("FieldsSize: " + ToStr instanceFieldSize);

		//MessageBox(0, zSTRING(base->m_sName) + zSTRING(base->f.tNumber), 0, 0);
		for (int i = 0; i < instanceFieldSize; i++)
		{
			// берем следующие base->f.tNumber символов, они и являются полями инстанции
			zCPar_Symbol* param = s_pfxParser->GetSymbol(baseClassIndex + i + 1);

			if (!param)
			{
				continue;
			}

			zSTRING sName = param->name;

			int pos = sName.Search(".", 1);
			//RX_FIX
			//if (pos > 0)
				//sName = param->name.Substr(pos + 1, 255);

			type = param->type;

			BYTE* addrVal = (BYTE*)addr + param->GetOffset();

			if (param->type == zPAR_TYPE_FLOAT)
			{
				float val = *((float*)addrVal);
				Stack_PushFloat(val);
				//cmd << sName << " " << type << " " << val << endl;
			}
			else if (param->type == zPAR_TYPE_INT)
			{
				int val = *((int*)addrVal);
				Stack_PushInt(val);

				//cmd << sName << " " << type << " " << val << endl;
			}
			else if (param->type == zPAR_TYPE_STRING)
			{
				zSTRING value = *((zSTRING*)((BYTE*)addr + param->GetOffset()));
				Stack_PushString(value);
				//cmd << sName << " " << type << " " << value << endl;
			}
			else
			{
				continue;
			}

			//Stack_PushInt((int)param->type);
			//Stack_PushString(sName);

			//prop->name = sName;
			//prop->type = type;

			//prop->addr = (BYTE*)addr + param->GetOffset();

			//Stack_PushUInt(*((BYTE*)addr + param->GetOffset()));
		
		}
		//Stack_PushInt(instanceFieldSize);
	}





}