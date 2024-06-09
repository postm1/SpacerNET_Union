// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	//zCLASS_DEFINITION(zCObjPreset, zCObject, 0, 1)

	zCObject* zCObjPresetLib::GetObjectByName(zSTRING& pName)
	{
		for (int i = 0; i < this->GetNumInList(); i++)
		{
			if (auto pEntry = this->GetSafe(i))
			{
				if (pEntry->presetName == pName)
				{
					return pEntry;
				}
			}
		}

		return NULL;
	}

	int zCObjPresetLib::Load()
	{
		int result = 0;
		DeleteList();
		//zERR_MESSAGE(4, zERR_BEGIN, "B: SPC: Loading ObjectPresets ...");

		cmd << "Loading ObjectPresets" << endl;

		zoptions->ChangeDir(DIR_TOOLS_DATA);
		zFILE_FILE subdir(PRESET_SUBDIR);
		subdir.ChangeDir(false);

		// Alle OPF-Dateien (sprich: "Views") suchen...
		zFILE_FILE presetFile;
		bool go = presetFile.FindFirst(zSTRING("*.opf"));
		while (go)
		{
			result = Load(presetFile.GetFile());
			go = presetFile.FindNext();
		}

		cmd << "Loading ObjectPresets Done..." << endl;

		return result;
	}


	int zCObjPresetLib::Load(zSTRING& filename)
	{
		//DeleteList();
		zoptions->ChangeDir(DIR_TOOLS_DATA);
		zFILE_FILE subdir(PRESET_SUBDIR);
		subdir.ChangeDir(false);

		zCObjPreset* preset;
		zCArchiver* arch = zarcFactory->CreateArchiverRead(filename, 0);
		if (!arch) return 1;

		cmd << "Read ObjectPresets \"" + filename + "\"" << endl;

		//zERR_MESSAGE(4, zERR_BEGIN, "B: SPC: Read ObjectPresets \"" + filename + "\"");
		while (!arch->EndOfArchive())
		{
			// preset = new zCObjPreset();		
			preset = dynamic_cast<zCObjPreset*>(arch->ReadObject(0));
			if (preset)
			{
				if (!GetObjectByName(preset->presetName))
				{
					InsertEnd(preset);

					cmd << "Inserted preset " << preset->presetName << endl;
					//zERR_MESSAGE(10, 0, "B: SPC: Inserted preset " + preset->presetName);
				}
				else
				{
					cmd << "Preset already exists : " << preset->presetName << endl;
					//zERR_WARNING("B: SPC: Preset already exists: " + preset->presetName);
					zRELEASE(preset);
				};
			}
		}


		//zERR_MESSAGE(4, zERR_END, "");
		arch->Close();
		zRELEASE(arch);
		return 0;
	}


	void zCObjPreset::Archive(zCArchiver& arc)
	{
		zCObject::Archive(arc);
		arc.WriteString("presetName", presetName);
		arc.WriteObject("preset", obj);
	}
	void zCObjPreset::Unarchive(zCArchiver& arc)
	{
		zCObject::Unarchive(arc);
		presetName = arc.ReadString("presetName");
		obj = arc.ReadObject("preset", 0);
	}

	void zCObjPresetLib::Dispose()
	{
		for (int z = 0; z < GetNumInList(); z++)
		{
			if (auto pEntry = this->GetSafe(z))
			{
				if (pEntry->obj) zRELEASE(pEntry->obj);

				zRELEASE(pEntry);
			}

		}

		this->DeleteList();
	}

	zCObjPresetLib::~zCObjPresetLib()
	{
		Dispose();

		//for (int z = 0; z < viewPointer.GetNumInList(); z++)
			//delete viewPointer[z];
	}
}