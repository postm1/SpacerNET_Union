// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	// Add your code here . . .
	// Построение списка в с#
	void CreateTree()
	{
		//MessageBox(0, "Crate in c++", 0, 0);
		static auto addNode = (voidFuncPointer)GetProcAddress(theApp.module, "CreateTree");
		addNode();
	}




	void CreateBaseVobTree(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		
		static auto addEntry = (AddGlobalEntryPointer)GetProcAddress(theApp.module, "AddGlobalEntry");
		

		if (!vob)
		{
			return;
		}

		if (auto pItem = vob->CastTo<oCItem>())
		{
			//итемы что нет в скриптах помечаем отдельно
			if (pItem && (pItem->GetInstanceName() == "" || pItem->GetInstance() == -1))
			{
				Stack_PushString("oCItem (Broken)");
			}
			else
			{
				Stack_PushString(vob->_GetClassDef()->className.ToChar());
			}
			
		}
		else
		{
			Stack_PushString(vob->_GetClassDef()->className.ToChar());
		}

		
		Stack_PushString(GetVobName(vob));

		addEntry((uint)vob, vob->GetParent());



		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateBaseVobTree(node);
			node = node->GetNextChild();
		}
	}



	zSTRING base_class_name = "";
	int parentId = 0;

	const CString PRESET_ENTRY_NEWOBJECT = "[NEW]";
	const CString POSTFIX_CLASS_SCRIPTED = "scripted";
	const CString POSTFIX_CLASS_ABSTRACT = "abstract";
	const CString POSTFIX_CLASS_CURRUPT = "currupt";

	void BuildClassHierarchy(zCClassDef *parentClassDef)
	{
		zSTRING baseName;
		zSTRING s;

		static auto addNode = (callVoidFunc)GetProcAddress(theApp.module, "AddClassNode");

		for (int i = 0; i<zCClassDef::GetNum(); i++)
		{
			zCClassDef* classDef = zCClassDef::GetClassDefByIndex(i);

			if (classDef->GetBaseClassDef() == parentClassDef) {

				zBOOL baseOK;
				if (classDef->GetBaseClassDef())
				{
					baseName = classDef->GetBaseClassDef()->GetClassName_();
					baseOK = (baseName == classDef->GetBaseClassName());
				}
				else {
					baseOK = (classDef->GetBaseClassName() == "NULL");
				};

				s = classDef->GetClassName_();

				CString postfix = "";
				CString postfixStr = "";
				
				if (!baseOK)
				{
				if (!postfix.IsEmpty()) postfix += ",";
				postfix += POSTFIX_CLASS_CURRUPT;
				}
				if (classDef->IsScriptedClass())
				{
				if (!postfix.IsEmpty()) postfix += ",";
				postfix += POSTFIX_CLASS_SCRIPTED;
				}
				if (classDef->IsAbstractClass())
				{
				if (!postfix.IsEmpty()) postfix += ",";
				postfix += POSTFIX_CLASS_ABSTRACT;
				}
				if (!postfix.IsEmpty()) postfixStr = postfixStr + " (" + postfix + ")";
				

				//HTREEITEM newITEM = ctrl_classtree.InsertItem(s.ToChar(), parent);


				

				if (addNode)
				{
					Stack_PushString(baseName);
					Stack_PushString(s);
					Stack_PushString(postfixStr);
					//MessageBox(0, s + " [3] " + zSTRING(parentId), 0, 0);
					addNode();
				}

				//parentId++;
				BuildClassHierarchy(classDef);

			}
		}
	}

	zCClassDef* GetBaseEngineClassDef(CString classTypeName)
	{
		int c = 0;
		BOOL found = FALSE;
		zCClassDef* vobClass = NULL;

		while (c<zCClassDef::GetNum() && !found)
		{
			vobClass = zCClassDef::GetClassDefByIndex(c);
			found = (A vobClass->GetClassName_() == classTypeName);
			if (!found) c++;
		}
		return vobClass;
	}


	void GetClassesRecursive(zCClassDef* parentClassDef)
	{
		static auto addNode = (callVoidFunc)GetProcAddress(theApp.module, "AddClassNode");

		CString baseName;
		CString s;

		for (int i = 0; i<zCClassDef::GetNum(); i++)
		{
			zCClassDef* classDef = zCClassDef::GetClassDefByIndex(i);

			if (classDef->GetBaseClassDef() == parentClassDef) {


				CString postfix = "";
				CString postfixStr = "";

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

				

				if (!baseOK)
				{
					if (!postfix.IsEmpty()) postfix += ",";
					postfix += POSTFIX_CLASS_CURRUPT;
				}
				if (classDef->IsScriptedClass())
				{
					if (!postfix.IsEmpty()) postfix += ",";
					postfix += POSTFIX_CLASS_SCRIPTED;
				}
				if (classDef->IsAbstractClass())
				{
					if (!postfix.IsEmpty()) postfix += ",";
					postfix += POSTFIX_CLASS_ABSTRACT;
				}
				if (!postfix.IsEmpty()) postfixStr = postfixStr + " (" + postfix + ")";


				//cmd << "baseName: " << baseName << " s: " << s << " postfixStr: " << postfixStr << endl;

				if (addNode)
				{
					Stack_PushString(baseName);
					Stack_PushString(s);
					Stack_PushString(postfixStr);
					addNode();
				}

				BuildClassHierarchy(classDef);

			}
		}

	}

	void BuildClassHierarchy()
	{
		zCClassDef* baseClassDef = GetBaseEngineClassDef("zCVob");

		cmd << "BuildClassHierarchy" << endl;

		if (!baseClassDef)
		{
			cmd << "GetBaseEngineClassDef fail!" << endl;
			return;
		}

		static auto addNode = (callVoidFunc)GetProcAddress(theApp.module, "AddClassNode");

		Stack_PushString("");
		Stack_PushString("zCVob");
		Stack_PushString("");
		addNode();


		GetClassesRecursive(baseClassDef);

		(callVoidFunc)GetProcAddress(theApp.module, "FillClassNodes")();
		

		/*

		zSTRING base_class_name3 = "zCVob";

		if (base_class_name != base_class_name3)
		{
			c = 0;
			found = FALSE;
			vobClass = 0;
			while (c<zCClassDef::GetNum() && !found)
			{
				vobClass = zCClassDef::GetClassDefByIndex(c);
				found = (vobClass->GetClassName_() == base_class_name3);
				if (!found) c++;
			}

			if (found)
			{
				zSTRING s = vobClass->GetClassName_();
				CString prefix = "";
				//newITEM2 = ctrl_classtree.InsertItem(s.ToChar(), TVI_ROOT);
				//MessageBox(0, "Found", 0, 0);

				

				if (addNode)
				{
					//MessageBox(0, s + " [2] " + zSTRING(parentId), 0, 0);

					Stack_PushString(base_class_name3);
					Stack_PushString(s);

					addNode(parentId);
				}


				BuildClassHierarchy(parentId, vobClass);

			}
		}


		/*
		zSTRING base_class_name2 = "zCEventMessage";

		if (base_class_name != base_class_name2)
		{
		c = 0;
		found = FALSE;
		vobClass = 0;
		while (c<zCClassDef::GetNum() && !found)
		{
		vobClass = zCClassDef::GetClassDefByIndex(c);
		found = (vobClass->GetClassName_() == base_class_name2);
		if (!found) c++;
		}

		if (found)
		{
		zSTRING s = vobClass->GetClassName_();

		//newITEM2 = ctrl_classtree.InsertItem(s.ToChar(), TVI_ROOT);


		auto addNode = (strFuncPoiter)GetProcAddress(theApp.module, "AddClassNode");

		if (addNode)
		{
		//MessageBox(0, s + " [2] " + zSTRING(parentId), 0, 0);

		addNode(s.ToChar(), parentId, base_class_name2.ToChar());
		}


		BuildClassHierarchy(parentId, vobClass);

		}
		}
		*/



	}

}