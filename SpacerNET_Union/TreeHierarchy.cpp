// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	// Add your code here . . .
	// Building a list in C#
	void CreateTree()
	{
		//MessageBox(0, "Crate in c++", 0, 0);
		static auto addNode = (voidFuncPointer)GetProcAddress(theApp.module, "CreateTree");
		addNode();
	}




	void CreateBaseVobTreeBuildTree(zCTree<zCVob>* node, bool& foundBadItem)
	{
		zCVob* vob = node->GetData();

		
		static auto addEntry = (AddGlobalEntryPointer)GetProcAddress(theApp.module, "AddGlobalEntry");
		

		if (!vob)
		{
			return;
		}

		if (auto pItem = vob->CastTo<oCItem>())
		{
			//items that are not in the scripts are marked separately
			if (pItem && (pItem->GetInstanceName() == "" || pItem->GetInstance() == -1))
			{
				Stack_PushString("oCItem (Broken)");
				foundBadItem = true;
			}
			else
			{
				Stack_PushString(vob->_GetClassDef()->className.ToChar());
			}
			
		}
		else if (vob == ogame->GetCameraVob() 
			|| vob->GetVobName() == "SPACER_VOB_HELPER_FLOOR"
			|| vob->GetVobName() == "SPACER_VOB_BBOX_MINS"
			|| vob->GetVobName() == "SPACER_VOB_BBOX_MAXS"
			)
		{
			Stack_PushString("Spacer vobs");
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
			CreateBaseVobTreeBuildTree(node, foundBadItem);
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

		theApp.exports.toggleUIElement(UIElementType::UI_MAIN_CLASSES, FALSE);

		Stack_PushString("");
		Stack_PushString("zCVob");
		Stack_PushString("");
		addNode();


		GetClassesRecursive(baseClassDef);

		(callVoidFunc)GetProcAddress(theApp.module, "FillClassNodes")();
		
		theApp.exports.toggleUIElement(UIElementType::UI_MAIN_CLASSES, TRUE);

	}

}