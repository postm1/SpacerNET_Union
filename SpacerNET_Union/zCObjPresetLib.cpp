// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	

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

	

	//=============================================================
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
		zoptions->ChangeDir(DIR_TOOLS_DATA);
		zFILE_FILE subdir(PRESET_SUBDIR);
		subdir.ChangeDir(false);

		zCObjPreset* preset = NULL;

		zCArchiver* arch = zarcFactory->CreateArchiverRead(filename, 0);
		if (!arch) return 1;

		cmd << "Read ObjectPresets \"" + filename + "\"" << endl;

		while (!arch->EndOfArchive())
		{
			zCObject* object = arch->ReadObject(0);

			preset = object->CastTo<zCObjPreset>();

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
			else
			{
				cmd << "Can't dynamic_cast<zCObjPreset*>" << endl;
			}
		}


		//zERR_MESSAGE(4, zERR_END, "");
		arch->Close();
		zRELEASE(arch);
		return 0;
	}

	void zCObjPresetLib::InsertIntoWorld(zSTRING name)
	{
		for (int z = 0; z < GetNumInList(); z++)
		{
			if (auto pEntry = this->GetSafe(z))
			{
				if (pEntry->obj && pEntry->presetName == name)
				{

					break;
				}
			}

		}
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