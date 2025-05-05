

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

	

	std::map<CString, SearchVobEntry> searchEntries;
	bool regExOn = false;

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

				if (classDef->IsAbstractClass()) s = s + A LIST_POSTFIX + A LIST_ABSTRACT;
				if (classDef->IsScriptedClass()) s = s + A LIST_POSTFIX + A LIST_SCRIPTED;

				s = A Characters(LIST_PREFIX, depth * 2 - 2) + s;


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




	void SpacerApp::PrepareSearchEntries(int regExOnFromUI)
	{
		searchEntries.clear();
		regExOn = regExOnFromUI;
		//cmd << "=== PrepareSearchEntries. Regex: " << regExOn << endl;
	}

	void SpacerApp::AddSearchEntry(CString fieldName, CString groupName, TPropEditType type, CString value, TSearchNumberType numberSearchType)
	{
		SearchVobEntry newEntry;

		newEntry.fieldName = fieldName;
		newEntry.groupName = groupName;
		newEntry.type = type;
		newEntry.value = value;
		newEntry.patternSearch = std::regex(value, std::regex_constants::icase | std::regex_constants::optimize);
		newEntry.fastCheckValueInt = 0;
		newEntry.fastCheckValueFloat = 0.0f;
		newEntry.numberSearchType = numberSearchType;

		// if we have bool or int, just convert input value (string) for real value (int) for fast checking in future
		if (type == PETbool || type == PETint || type == PETenum)
		{
			newEntry.fastCheckValueInt = newEntry.value.ToInt32();
		}
		else if (type == PETfloat)
		{
			newEntry.fastCheckValueFloat = newEntry.value.ToReal32();
		}
		
		cmd << fieldName << " | " << groupName << " | TPropEditType: " << type 
			<< " | Value: " << value 
			<< " | FastValueInt: " << newEntry.fastCheckValueInt
			<< " | FastValueFloat: " << newEntry.fastCheckValueFloat
			<< " | numberSearchType: " << newEntry.numberSearchType
			<< endl;
		

		searchEntries[fieldName] = newEntry;
	}

	bool SpacerApp::FastSearchVob_String(SearchVobEntry& entry, std::string value)
	{
		std::string valueSearch = entry.value;

		if (regExOn)
		{
			if (!std::regex_search(value, entry.patternSearch))
			{
				return false;
			}
		}
		else
		{
			//cmd << "'" << vobName.c_str() << "' | '" << valueSearch.c_str() << "'" << endl;

			if (value != valueSearch)
			{
				return false;
			}
		}

		return true;
	}

	// Fast checking of MOST usable fields in Search
	bool SpacerApp::SearchHandleVobByFastMethod(zCVob*& vob, int selectedCount, int fastSearchTypeMask)
	{
		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_NAME) != 0)
		{
			auto& entry = searchEntries["vobName"];
			std::string value = vob->GetVobName();

			if (!FastSearchVob_String(entry, value))
			{
				return false;
			}
		}

		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_VISUAL) != 0)
		{
			auto& entry = searchEntries["visual"];
			std::string value = vob->GetVisual() ? vob->GetVisual()->GetVisualName() : "";

			if (!FastSearchVob_String(entry, value))
			{
				return false;
			}
		}

		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_SHOW_VISUAL) != 0)
		{
			auto& entry = searchEntries["showVisual"];
			
			if (vob->showVisual != entry.fastCheckValueInt)
			{
				return false;
			}
		}

		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_CD_STATIC) != 0)
		{
			auto& entry = searchEntries["cdStatic"];

			if (vob->collDetectionStatic != entry.fastCheckValueInt)
			{
				return false;
			}
		}

		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_CD_DYNYAMIC) != 0)
		{
			auto& entry = searchEntries["cdDyn"];

			if (vob->collDetectionDynamic != entry.fastCheckValueInt)
			{
				return false;
			}
		}

		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_STATIC_VOB) != 0)
		{
			auto& entry = searchEntries["staticVob"];

			if (vob->staticVob != entry.fastCheckValueInt)
			{
				return false;
			}
		}
		
		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_ANI_MODE) != 0)
		{
			auto& entry = searchEntries["visualAniMode"];

			if (vob->m_AniMode != entry.fastCheckValueInt)
			{
				return false;
			}
		}

		if ((fastSearchTypeMask & FAST_SEARCH_FIELD_ANI_MODE_STR) != 0)
		{
			auto& entry = searchEntries["visualAniModeStrength"];

			// ==
			if (entry.numberSearchType == TS_EQUALS) 
			{
				if (vob->m_aniModeStrength != entry.fastCheckValueFloat)
				{
					return false;
				}
			}
			// <=
			else if (entry.numberSearchType == TS_LESSTHAN)
			{
				if (vob->m_aniModeStrength >= entry.fastCheckValueFloat)
				{
					return false;
				}
			}
			//>=
			else if (entry.numberSearchType == TS_MORETHAN)
			{
				if (vob->m_aniModeStrength < entry.fastCheckValueFloat)
				{
					return false;
				}
			}
				
		};

		resultFound.Insert(vob);
		return true;

	}

	bool SpacerApp::SearchHandleVob(zCVob*& vob, int selectedCount, int fastSearchTypeMask)
	{

		static auto compare = (compareVobs)GetProcAddress(theApp.module, "CompareVobs");
		static auto compareByNameOrVisual = (compareVobs)GetProcAddress(theApp.module, "CompareByNameOrVisual");

		if (!vob) return false;

		//if fastSearchTypeMask exists => Fast search
		if (fastSearchTypeMask > 0)
		{
			return SearchHandleVobByFastMethod(vob, selectedCount, fastSearchTypeMask);
			
			/*Stack_PushString(vobName);
			Stack_PushString(visualName);

			if (compareByNameOrVisual())
			{
				resultFound.Insert(vob);
				return true;
			}*/
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

	int SpacerApp::SearchFillVobClass(bool derived, bool hasChildren, int type, int selectedCount, int fastSearchTypeMask, int matchNames, int searchOCItem, int radius)
	{
		static auto callFunc = (addToVobList)GetProcAddress(theApp.module, "AddSearchVobResult");
		static auto callFuncOnEnd = (callVoidFunc)GetProcAddress(theApp.module, "OnSearchResultEnd");

		
		int resultCount = 0;
		Array<uint> arr;


		//cmd << "SearchFillVobClass: derived " << derived << " hasChildren " << hasChildren << " type: " << type 
			//<< " sel: " << selectedCount << " onlyVisOrName: " << onlyVisualOrName << endl;

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

		std::unordered_set<zCVob*> pHashVobs;
		int dub = 0;


		for (int i = 0; i < result.GetNumInList(); i++)
		{
			auto pVob = result.GetSafe(i);

			// don't add SpacerNET vobs in search result
			if (IsSpacerVob(pVob))
			{
				continue;
			}

			auto it = pHashVobs.find(pVob);

			if (it == pHashVobs.end())
			{
				pHashVobs.insert(pVob);
			}
			else
			{
				//cmd << "Found dublicate: " << (dub++) << endl;
			}
		}

		result.DeleteList();

		for (auto& entry : pHashVobs)
		{
			result.InsertEnd(entry);
		}


		//cmd << "result: " << result.GetNum() << endl;

		//result.RemoveDoubles();
		int num = result.GetNumInList();

		auto camPos = ogame->GetCameraVob()->GetPositionWorld();

		//cmd << "SearchWithRadius: " << radius << endl;
		
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
						SearchHandleVob(result[i], selectedCount, fastSearchTypeMask);
						
					}
				}
				else
				{
					SearchHandleVob(result[i], selectedCount, fastSearchTypeMask);
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

		//search by name of vob (item) in chests
		if (searchOCItem)
		{
			zCArray<zCVob*> pListMobCont;

			ogame->GetWorld()->SearchVobListByClass(oCMobContainer::classDef, pListMobCont, 0);

			zSTRING itemNameSearch = theApp.search.searchVobNameGlobal.Upper();

			if (itemNameSearch.Length() > 0)
			{
				for (int i = 0; i < pListMobCont.GetNumInList(); i++)
				{
					auto pVob = pListMobCont.GetSafe(i);

					if (pVob)
					{
						if (auto pCont = pVob->CastTo<oCMobContainer>())
						{
							if (pCont && pCont->contains.Upper().Contains(itemNameSearch))
							{
								resultFound.Insert(pVob);

							}
						}
					}


				}
			}

			
		}

		// look for duplicate vob names on the map
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

			//RX_Begin(10);
			
			// sorting search result by vob names
			std::vector<std::pair<std::string, zCVob*>> vobsWithNames;
			vobsWithNames.reserve(resultFound.GetNumInList());
			
			for (int i = 0; i < resultFound.GetNumInList(); i++)
			{
				if (auto pVob = resultFound.GetSafe(i))
				{
					zSTRING name = GetVobName(pVob);
					vobsWithNames.emplace_back(name, pVob);
				}
				
			}

			resultFound.DeleteList();

			std::sort(vobsWithNames.begin(), vobsWithNames.end(),
				[](const auto& a, const auto& b) { return a.first < b.first; });

			
			for (size_t i = 0; i < vobsWithNames.size(); ++i) {
				resultFound.InsertEnd(vobsWithNames[i].second);
			}
		
			//RX_End(10);

			//cmd << "SORT: " << RX_PerfString(10) << endl;

			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				zCVob* vob = resultFound[i];

				if (vob)
				{

					if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
					{
						continue;
					}

					Stack_PushString(GetVobName(vob));

					callFunc((uint)vob);

					resultCount += 1;
				}


			}
		}

		if (searchType == SearchVobType::SearchSingleWP)
		{

			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

			if (waynet)
			{
				for (int i = 0; i < resultFound.GetNum(); i++)
				{
					zCVob* vob = resultFound[i];

					if (vob)
					{
						if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
						{
							continue;
						}

						if (auto curWp = vob->CastTo<zCVobWaypoint>())
						{
							if (auto realWp = waynet->GetWaypoint(curWp->GetVobName()))
							{
								if (realWp->GetNumberOfWays() == 0)
								{
									if (arr.SearchEqual((uint)vob) == Invalid)
									{
										Stack_PushString(GetVobName(vob));

										arr.Insert((uint)vob);

										callFunc((uint)vob);

										resultCount += 1;
									}
								}
								
							}
								
						}
					}
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
					if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
					{
						continue;
					}

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

			zSTRING path = theApp.search.replaceZenPath;
			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);

			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				zCVob* vob = resultFound[i];

				zCZone* isZone = dynamic_cast<zCZone*>(vob);

				if (vob && !isZone && arr.SearchEqual((uint)vob) == Invalid)
				{

					if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
					{
						continue;
					}

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

					if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
					{
						continue;
					}

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
					if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
					{
						continue;
					}
					
					arr.Insert((uint)vob);
					vob->SetCollDetDyn(!vob->GetCollDetDyn());
					resultCount += 1;
				}

				isZone = NULL;
				vob = NULL;
			}
		}

		if (searchType == SearchVobType::Rename)
		{
			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);

			int type = theApp.renameOptions.type;
			int startNum = theApp.renameOptions.startNumber;


			for (int i = 0; i < resultFound.GetNum(); i++)
			{
				if (auto vob = resultFound.GetSafe(i))
				{

					if (vob->CastTo<oCItem>())
					{
						continue;
					}

					if (radius != -1 && camPos.Distance(vob->GetPositionWorld()) > radius)
					{
						continue;
					}

					zSTRING lastName = vob->GetObjectName();

					zSTRING name = "";
					
					// empty name
					if (type == 1)
					{
						//do nothing

					}
					else if (type == 2)
					{
						name = theApp.renameOptions.allName;
					}
					else if (type == 3)
					{
						name = theApp.renameOptions.prefixName + Z (startNum++);
					}

					//protect waypoints when name is empty
					if (auto wp = dynamic_cast<zCVobWaypoint*>(vob))
					{
						if (type == 1)
						{
							continue;
						}
					}
					
					
					vob->SetVobName(name);
					resultCount += 1;

					RecalcWPBBox(vob);

					if (auto wp = dynamic_cast<zCVobWaypoint*>(vob))
					{
						wp->SetVobName(name);
						auto wpObj = ogame->GetWorld()->wayNet->GetWaypoint(lastName);

						if (wpObj)
						{
							wpObj->SetName(name);
						}
					}

					if (auto pKey = dynamic_cast<zCCamTrj_KeyFrame*>(vob))
					{
						if (pKey->cscam)
						{
							int index = -1;

							if (pKey->type == KF_CAM)
							{
								index = pKey->cscam->SearchCamKey(pKey);

								//cmd << "Rename: " << index << " all: " << pKey->cscam->GetNumCamKeys() << endl;
								if (index != -1)
								{
									camMan.OnRenameSplineKey(index, name);
								}

							}
							else
							{
								index = pKey->cscam->SearchTargetKey(pKey);

								if (index != -1)
								{
									camMan.OnRenameTargetKey(index, name);
								}
							}


						}
					}

					auto updateName = (onUpdateVobName)GetProcAddress(theApp.module, "UpdateVobName");
					Stack_PushString(GetVobName(vob));
					updateName((uint)vob);
				}

			}

			



			theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
		}


		callFuncOnEnd();

		exports.toggleUIElement(UIElementType::UI_LIST_SEARCH_RESULT, TRUE);

		//cmd << A resultCount << endl;
		return resultCount;
	}

	





}