// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void MatManager::CreateMatTree()
	{
		static auto addEntryMat = (callIntFunc)GetProcAddress(theApp.module, "AddGlobalEntryMat");


		mm.CleanSelection();

		zCClassDef* matDef = zCMaterial::classDef;

		cmd << "All MatList count: " << matDef->objectList.GetNum() << endl;

		int countAdded = 0;

		for (int i = 0; i < matDef->objectList.GetNum(); i++)
		{
			auto mat = dynamic_cast<zCMaterial*>(matDef->objectList[i]);
			if (mat && mat->matUsage == zCMaterial::zMAT_USAGE_LEVEL)
			{
				//cmd << mat->GetName() << endl;

				zSTRING name = mat->GetName();

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

		cmd << "MatList count added: " << countAdded << endl;

	}
}