// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	// Построение списка в с#
	void CreateTree()
	{
		//MessageBox(0, "Crate in c++", 0, 0);
		auto addNode = (voidFuncPointer)GetProcAddress(theApp.module, "CreateTree");
		addNode();
	}


	AddGlobalEntryPointer addEntry = NULL;

	void CreateBaseVobTree(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		if (!addEntry)
		{
			addEntry = (AddGlobalEntryPointer)GetProcAddress(theApp.module, "AddGlobalEntry");
		}

		Stack_PushString(vob->_GetClassDef()->className.ToChar());
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



	void BuildClassHierarchy(int parentId, zCClassDef *parentClassDef)
	{
		zSTRING baseName;
		zSTRING s;
		parentId++;

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

				zSTRING postfix = "";
				/*
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
				if (!postfix.IsEmpty()) s = s + " (" + postfix + ")";
				*/

				//HTREEITEM newITEM = ctrl_classtree.InsertItem(s.ToChar(), parent);


				auto addNode = (addClassNode)GetProcAddress(theApp.module, "AddClassNode");

				if (addNode)
				{
					Stack_PushString(baseName);
					Stack_PushString(s);
					//MessageBox(0, s + " [3] " + zSTRING(parentId), 0, 0);
					addNode(parentId);
				}

				//parentId++;
				BuildClassHierarchy(parentId, classDef);

			}
		}
	}

	bool SetBaseClass(zSTRING class_name)
	{
		if (base_class_name != class_name)
		{
			base_class_name = class_name;

			BuildClassHierarchy();

			return true; // changed
		}
		else
		{
			return false; // not changed
		};
	}

	void BuildClassHierarchy()
	{
		int c = 0;
		BOOL found = FALSE;
		zCClassDef* vobClass = 0;
		while (c<zCClassDef::GetNum() && !found)
		{
			vobClass = zCClassDef::GetClassDefByIndex(c);
			found = (vobClass->GetClassName_() == base_class_name);
			if (!found) c++;
		}
		if (found)
		{
			zSTRING s = vobClass->GetClassName_();
			//newITEM1 = ctrl_classtree.InsertItem(s.ToChar(), TVI_ROOT);

			auto addNode = (addClassNode)GetProcAddress(theApp.module, "AddClassNode");

			if (addNode)
			{
				Stack_PushString("");
				Stack_PushString(s);

				//MessageBox(0, s + " [1] " + zSTRING(parentId), 0, 0);
				addNode(parentId);
			}

			BuildClassHierarchy(parentId, vobClass);

		}


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

				//newITEM2 = ctrl_classtree.InsertItem(s.ToChar(), TVI_ROOT);
				//MessageBox(0, "Found", 0, 0);

				auto addNode = (addClassNode)GetProcAddress(theApp.module, "AddClassNode");

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