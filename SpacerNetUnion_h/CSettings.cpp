// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	CSetting::CSetting(CSettingType type, zSTRING section, zSTRING name, zSTRING default_value)
	{
		this->type = type;
		this->section = section;
		this->name = name;
		this->default_value = default_value;
	}



	float CSettings::GetFloatVal(zSTRING key)
	{
		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set && set->name == key)
			{
				return set->value_float;
			}
		}

		return 0;
	}

	int CSettings::GetIntVal(zSTRING key)
	{
		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set && set->name == key)
			{
				return set->value_int;
			}
		}

		return 0;
	}

	zSTRING CSettings::GetVal(zSTRING key)
	{
		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set && set->name == key)
			{
				//std::cout << "CSettings::GetVal key: " << key << " value: " << set->value_string << std::endl;
				return set->value_string;
			}
		}

		return zSTRING("");
	}

	void CSettings::SetFloatVal(zSTRING key, float val)
	{
		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set && set->name == key)
			{
				set->value_float = val;
				set->value_string = zSTRING(val, 6);
				break;
			}
		}
	}

	void CSettings::SetIntVal(zSTRING key, int val)
	{
		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set && set->name == key)
			{
				set->value_int = val;
				set->value_string = zSTRING(val);
				break;
			}
		}
	}

	void CSettings::SetStringVal(zSTRING key, zSTRING val)
	{
		//std::cout << "CSettings::SetStringVal!!!!!! key: " << key << " value: " << val << std::endl;

		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set && set->name == key)
			{
				//std::cout << "CSettings::SetStringVal key: " << key << " value: " << val << std::endl;
				set->value_string = val;
				break;
			}
		}
	}



	void CSettings::Load()
	{
		CString value;

		WriteLine("Reading settings...");

		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set)
			{
				theApp.spcOpt.Read(value, set->section, set->name, set->default_value);

				set->value_string = value.ToChar();

				if (set->type == TYPE_INT)
				{
					set->value_int = value.ToInt32();
				}
				else if (set->type == TYPE_FLOAT)
				{
					set->value_float = value.ToReal32();
				}
			}

		}
	}

	void CSettings::Save()
	{
		CString value;


		for (int i = 0; i < list.GetNumInList(); i++)
		{
			CSetting* set = list.Get(i);

			if (set)
			{
				theApp.spcOpt.Write(set->value_string, set->section, set->name);
			}
		}

		theApp.spcOpt.SaveAll();
	}

	void CSettings::Apply()
	{

		if (ogame->GetWorld())
		{
			zCVob::s_renderVobs = GetIntVal("showVobs");

			ogame->GetWorld()->showWaynet = GetIntVal("showWaynet");

			zCVobLight::renderLightVisuals = GetIntVal("showHelpVobs");		// Включает визуалы для источников света
			zCVob::SetShowHelperVisuals(zCVobLight::renderLightVisuals);          // Включает визуалы для тригерров, источников звука и прочего

			ogame->GetWorld()->bspTree.drawVobBBox3D = GetIntVal("drawBBoxGlobal");

			int maxFps = GetIntVal("maxFPS");

			if (maxFps > 0)
			{
				ztimer->LimitFPS(maxFps);
			}

			if (zmusic) zmusic->SetVolume((float)GetIntVal("musicVolume") / 100.0f);


			theApp.turnWpMode = (TurnWpMode)GetIntVal("wpTurnOn");
			SetRangeVobs();
			SetRangeWorld();
		}

		this->Save();

	}



	CSettings::~CSettings()
	{

		list.DeleteList();
	}

	void CSettings::Init()
	{

		CSetting* set = NULL;


		set = new CSetting(TYPE_INT, "CAMERA", "camTransSpeed", "16");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "camRotSpeed", "16");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "rangeWorld", "2000");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "rangeVobs", "2000");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "showFps", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "showTris", "1");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "CAMERA", "showVobsCount", "1");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "CAMERA", "showWaypointsCount", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "showCamCoords", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "showVobDist", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "showInvisibleVobs", "0");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "CAMERA", "hideCamWindows", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CAMERA", "showWPNames", "1");
		list.Insert(set);



		set = new CSetting(TYPE_INT, "SPACER", "showVobs", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "showWaynet", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "showHelpVobs", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "drawBBoxGlobal", "0");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "addDatePrefix", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "maxFPS", "0");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "vobListRadius", "150");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "showModelPreview", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "searchOnly3DS", "0");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "musicVolume", "100");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "musicZenOff", "0");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "askExitZen", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "lightCompileType", "0");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "SPACER", "lightCompileRegionOn", "0");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "lightCompileRadius", "2000");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "SPACER", "worldCompileType", "0");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "SPACER", "addWPToNet", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "addWPAutoName", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "downFPToGround", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "addFPAutoName", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "SPACER", "fullPathTitle", "0");
		list.Insert(set);


		set = new CSetting(TYPE_STRING, "PATH", "treeVobPath", "");
		list.Insert(set);
		set = new CSetting(TYPE_STRING, "PATH", "meshPath", "");
		list.Insert(set);
		set = new CSetting(TYPE_STRING, "PATH", "zenzPath", "");
		list.Insert(set);
		set = new CSetting(TYPE_STRING, "PATH", "vobsPath", "");
		list.Insert(set);
		set = new CSetting(TYPE_STRING, "PATH", "vobResPath", "");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "PATH", "openLastZen", "0");
		list.Insert(set);

		set = new CSetting(TYPE_STRING, "PATH", "openLastZenPath", "");
		list.Insert(set);



		set = new CSetting(TYPE_INT, "CONTROLS", "vobTransSpeed", "60");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobRotSpeed", "32");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertItemLevel", "1");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertVobRotRand", "0");
		list.Insert(set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertHierarchy", "1");
		list.Insert(set);


		set = new CSetting(TYPE_INT, "CONTROLS", "wpTurnOn", "0");
		list.Insert(set);

	}
}


