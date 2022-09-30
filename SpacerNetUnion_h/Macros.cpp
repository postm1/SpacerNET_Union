// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct MacrosEntry
	{
		CString arg;
		MacrosType type;
		CString raw;
	};

	zCArray<MacrosEntry*> pList;

	void SpacerApp::Macros_Clean()
	{
		//cmd << "Macros_Clean" << endl;
		pList.DeleteListDatas();
	}


	void SpacerApp::Macros_Run()
	{
		cmd << "Macros_Run" << endl;

		static auto pointer = (callVoidFunc)GetProcAddress(theApp.module, "InfoWin_AddText");


		Stack_PushString("\n[Macros START]\n");
		pointer();

		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			auto entry = pList.GetSafe(i);

			if (entry)
			{


				Stack_PushString(Z(i + 1) + ") " + Z entry->raw + "\n");
				pointer();

				cmd << "Execute " << entry->type << " " << entry->raw  << " arg " << entry->arg << endl;

				switch (entry->type)
				{
					case MacrosType::MT_RESET: this->Reset(); break;

					case MacrosType::MT_COMPILE_LIGHT_HIGH: this->DoCompileLight(3, 0); break;
					case MacrosType::MT_COMPILE_LIGHT_MID: this->DoCompileLight(2, 0); break;
					case MacrosType::MT_COMPILE_LIGHT_LOW: this->DoCompileLight(1, 0); break;
					case MacrosType::MT_COMPILE_LIGHT_VERTEX: this->DoCompileLight(0, 0); break;

					case MacrosType::MT_COMPILE_WORLD_OUTDOOR: this->DoCompileWorld(1); break;
					case MacrosType::MT_COMPILE_WORLD_INDOOR: this->DoCompileWorld(0); break;

					case MacrosType::MT_LOAD_MESH:
					{
						if (IsAWorldLoaded())
						{
							//cmd << " MergeMesh" << endl;
							this->MergeMesh(entry->arg);
						
						}
						else
						{

							//cmd << " LoadMesh" << endl;
							this->LoadMesh(entry->arg);

						}
						break;
					}
				

					case MacrosType::MT_LOAD_WORLD: 
					{
						if (IsAWorldLoaded())
						{
							//cmd << " MergeVobSubtree" << endl;
							//ogame->GetGameWorld()->MergeVobSubtree(Z entry->arg, 0, zCWorld::zWLD_LOAD_MERGE_ADD);
							this->MergeZen(entry->arg, true); break;
						}
						else
						{

							//cmd << " MergeZen" << endl;
							this->LoadWorld(entry->arg, LOAD_ZEN_UC, true);

						}
						break;
						
					}
				}
			}
		}

		if (ogame->GetWorld())
		{
			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();
			if (waynet)
			{
				zSTRING result = waynet->MergeWaypoints();
				if (!result.IsEmpty())
				{
					//AfxMessageBox(zSTRING(zSTRING("Merged the following waypoints: \r\n") + result).ToChar(), MB_OK | MB_TOPMOST);
				};
			};
		}

		Stack_PushString("[Macros FINISHED]\n");
		pointer();

		PlaySoundGame(ToStr "CS_IAI_ME_ME");
		Macros_Clean();
	}


	void SpacerApp::Macros_Add(CString command, CString raw, MacrosType type)
	{
		//cmd << command << " " << type << " " << raw << endl;

		auto entry = new MacrosEntry();
		entry->type = type;
		entry->arg = command;
		entry->raw = raw;

		pList.Insert(entry);
	}


}