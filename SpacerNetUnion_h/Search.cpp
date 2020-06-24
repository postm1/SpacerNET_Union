

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
		for (int i = 0; i<zCClassDef::GetNum(); i++)
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
			while (c<zCClassDef::GetNum() && !found)
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

	bool SpacerApp::SearchHandleVob(zCVob*& vob)
	{
		if (!vob) return false;


		zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, 0, 0);
		arch->SetStringEOL(zSTRING("\n"));
		arch->WriteObject(vob);
		zSTRING archBuffer;
		arch->GetBufferString(archBuffer);

		Stack_PushString(archBuffer);

		arch->Close();
		zRELEASE(arch);

		static auto compare = (compareVobs)GetProcAddress(theApp.module, "CompareVobs");

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

	int SpacerApp::SearchFillVobClass(bool derived, int type, int selectedCount)
	{
		static auto callFunc = (addToVobList)GetProcAddress(theApp.module, "AddSearchVobResult");
		int resultCount = 0;
		Array<uint> arr;

		SearchVobType searchType = (SearchVobType)type;

		zCArray<zCVob*> result;
		resultFound.DeleteList();


		zCVob* curTempVob = NULL;

		curTempVob = search.cur_vob;


		ogame->GetWorld()->SearchVobListByClass(curTempVob->GetClassDef(), result, 0);

		if (derived)
		{
			ogame->GetWorld()->SearchVobListByBaseClass(curTempVob->GetClassDef(), result, 0);
		}
		

		int num = result.GetNumInList();

		for (int i = 0; i<num; i++)
		{
			if (dynamic_cast<zCVobLevelCompo*>(result[i]))	continue;
			if (result[i] == ogame->GetCamera()->GetVob())	continue;
			if (!result[i])	continue;

			if (selectedCount > 0)
			{
				SearchHandleVob(result[i]);
			}
			else
			{
				resultFound.Insert(result[i]);
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

			//MessageBox(0, "Замен: " + A resultFound.GetNum(), 0, 0);
		}


		
		// remove
		if (searchType == SearchVobType::Remove)
		{

			//cmd << "All found: " << A resultFound.GetNum() << endl;

			theApp.SetSelectedVob(NULL);
			zCVob* vob = NULL;
			zCZone* isZone = NULL;

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
		}
		
		//cmd << A resultCount << endl;
		return resultCount;
	}
}