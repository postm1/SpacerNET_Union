// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void MatManager::CreateMatTree()
	{
		static auto addEntryMat = (callIntFunc)GetProcAddress(theApp.module, "AddGlobalEntryMat");
		static auto toggle = (callIntFunc)GetProcAddress(theApp.module, "SetObjTreeMat_VisibleToggle");

		mm.CleanSelection();
		pMaterialsMap.Clear();

		zCClassDef* matDef = zCMaterial::classDef;

		cmd << "All MatList count: " << matDef->objectList.GetNum() << endl;

		int countAdded = 0;

		
		toggle(0);

		for (int i = 0; i < matDef->objectList.GetNum(); i++)
		{
			auto mat = dynamic_cast<zCMaterial*>(matDef->objectList[i]);

			if (mat)
			{
				pMaterialsMap.Insert(mat->GetName(), mat);
			}

			if (mat)
			{

				zSTRING name = mat->GetName();

				if (mat->matUsage == zCMaterial::zMAT_USAGE_LEVEL)
				{

				}
				else
				{
					//name += "_NOUSAGE";
					continue;
				}
				//cmd << mat->GetName() << endl;

				

				if (name.Length() == 0 && mat->texture)
				{
					name = mat->texture->objectName;
				}
				
				Stack_PushString(name);
				Stack_PushString(mat->GetMatGroupString());

				addEntryMat((uint)mat);
				
				countAdded += 1;

			}
				
		}

		toggle(1);
		cmd << "MatList zMAT_USAGE_LEVEL count added: " << countAdded << endl;
		cmd << "MatList all materials count added: " << pMaterialsMap.GetNum() << endl;
	}
}