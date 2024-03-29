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
		Stack_PushString("#FF00000");
		pointer();

		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			auto entry = pList.GetSafe(i);

			if (entry)
			{


				Stack_PushString(Z(i + 1) + ") " + Z entry->raw + "\n");
				Stack_PushString("");
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
					case MacrosType::MT_SAVE_MESH:
					{
						//cmd << "Export mesh: " << entry->arg << endl;
						//cmd << "Export raw: " << entry->raw << endl;
						this->ExportWorldMesh(entry->arg);
						break;
					}

					case MacrosType::MT_SAVE_WORLD_BIN:
					{
						this->SaveWorld(entry->arg, SAVE_ZEN_BIN, 1);
						break;
					}

					case MacrosType::MT_SAVE_WORLD_UNCOMPILED_VOBS:
					{
						this->SaveWorld(entry->arg, SAVE_ZEN_UC, 1);
						break;
					}

					case MacrosType::MT_SAVE_WORLD_COMPILED_ASCII:
					{
						this->SaveWorld(entry->arg, SAVE_ZEN, 1);
						break;
					}

					case MacrosType::MT_LOAD_WORLD: 
					{
						if (IsAWorldLoaded())
						{
							
							this->MergeZen(entry->arg, true); break;
						}
						else
						{

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
					
				};
			};

			
		}

		Stack_PushString("[Macros FINISHED]\n");
		Stack_PushString("#FF00000");
		pointer();

		PlaySoundGame(ToStr "CS_IAI_ME_ME");
		Macros_Clean();


		if (ogame->GetGameWorld())
		{
			Stack_PushString(ogame->GetGameWorld()->GetWorldFilename().GetWord("\\", -1));

			(callVoidFunc)GetProcAddress(theApp.module, "UpdateSpacerTitle")();
		}
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