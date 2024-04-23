// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	CSetting::CSetting(CSettingType type, CString section, CString name, CString default_value)
	{
		this->type = type;
		this->section = section;
		this->name = name;
		this->default_value = default_value;
	}



	float CSettings::GetFloatVal(CString key)
	{
		auto& pair = list[key];
		float result = 0.0f;
		if (!pair.IsNull())
		{
			result = pair.GetValue()->value_float;
		}

		return result;
	}

	int CSettings::GetIntVal(CString key)
	{
		auto& pair = list[key];
		int result = 0;
		if (!pair.IsNull())
		{
			result = pair.GetValue()->value_int;
		}

		return result;
	}

	CString CSettings::GetVal(CString key)
	{
		auto& pair = list[key];
		CString result = "";

		if (!pair.IsNull())
		{
			result = pair.GetValue()->value_string;
		}

		return result;
	}

	void CSettings::SetFloatVal(CString key, float val)
	{
		auto& pair = list[key];

		if (!pair.IsNull())
		{
			pair.GetValue()->value_float = val;
			pair.GetValue()->value_string = CString(val);
		}
	}

	void CSettings::SetIntVal(CString key, int val)
	{
		auto& pair = list[key];

		if (!pair.IsNull())
		{
			pair.GetValue()->value_int = val;
			pair.GetValue()->value_string = CString(val);
		}
	}

	void CSettings::SetStringVal(CString key, CString val)
	{
		auto& pair = list[key];

		if (!pair.IsNull())
		{
			pair.GetValue()->value_string = val;
		}
	}



	void CSettings::Load()
	{
		CString value;

		WriteLine("CSettings: reading settings...");

		auto arr = list.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			CSetting* set = arr.GetSafe(i)->GetValue();

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

		this->SetIntVal("bToggleWorkMode", 0);
	}

	void CSettings::Save()
	{
		CString value;

		auto arr = list.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			CSetting* set = arr.GetSafe(i)->GetValue();

			if (set)
			{
				theApp.spcOpt.Write(set->value_string, set->section, set->name);
			}
		}


		auto getVersion = (callVoidFunc)GetProcAddress(theApp.module, "GetSpacerVersion");
		getVersion();

		CString version = Stack_PeekString();

		theApp.spcOpt.Write(version, "INFO", "Version");

		theApp.spcOpt.SaveAll();
	}

	void CSettings::Apply()
	{

		if (ogame->GetWorld())
		{
			zCVob::s_renderVobs = GetIntVal("showVobs");

			ogame->GetWorld()->showWaynet = GetIntVal("showWaynet");

			zCVobLight::renderLightVisuals = GetIntVal("showHelpVobs");	
			zCVob::SetShowHelperVisuals(zCVobLight::renderLightVisuals); 

			ogame->GetWorld()->bspTree.drawVobBBox3D = GetIntVal("drawBBoxGlobal");	

			int maxFps = GetIntVal("maxFPS");

			if (maxFps >= 0)
			{
				//ztimer->LimitFPS(maxFps);
				Union.GetSysPackOption().Write(maxFps, "PARAMETERS", "FPS_Limit");
			}

			if (zmusic) zmusic->SetVolume((float)GetIntVal("musicVolume") / 100.0f);


			theApp.turnWpMode = (TurnWpMode)GetIntVal("wpTurnOn");
			SetRangeVobs();
			SetRangeWorld();


			theApp.SetHoldTime(theApp.options.GetIntVal("holdTime"));

			theApp.zSpyLevel = GetIntVal("infoWinzSpyLevel");
			theApp.zSpyActive = GetIntVal("infoWinzSpyEnabled");


			if (theApp.zSpyActive)
			{
				zerr->SetFilterLevel(theApp.zSpyLevel);
			}

		}

		this->Save();

	}



	
	void CSettings::Init()
	{

		theApp.spcOpt.Write("1", "OVERRIDES", "VIDEO.zStartupWindowed");

		CSetting* set = NULL;

		set = new CSetting(TYPE_INT, "CAMERA", "camTransSpeed", "16");
		list.Insert("camTransSpeed", set);

		set = new CSetting(TYPE_INT, "CAMERA", "camRotSpeed", "16");
		list.Insert("camRotSpeed", set);

		set = new CSetting(TYPE_INT, "CAMERA", "rangeWorld", "2000");
		list.Insert("rangeWorld", set);

		set = new CSetting(TYPE_INT, "CAMERA", "rangeVobs", "2000");
		list.Insert("rangeVobs", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showFps", "1");
		list.Insert("showFps", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showTris", "1");
		list.Insert("showTris", set);

		set = new CSetting(TYPE_INT, "CAMERA", "slerpRot", "0");
		list.Insert("slerpRot", set);


		set = new CSetting(TYPE_INT, "CAMERA", "showVobsCount", "1");
		list.Insert("showVobsCount", set);


		set = new CSetting(TYPE_INT, "CAMERA", "showWaypointsCount", "1");
		list.Insert("showWaypointsCount", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showCamCoords", "1");
		list.Insert("showCamCoords", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showVobDist", "1");
		list.Insert("showVobDist", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showInvisibleVobs", "0");
		list.Insert("showInvisibleVobs", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showPortalsInfo", "1");
		list.Insert("showPortalsInfo", set);



		set = new CSetting(TYPE_INT, "CAMERA", "hideCamWindows", "1");
		list.Insert("hideCamWindows", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showWPNames", "1");
		list.Insert("showWPNames", set);

		set = new CSetting(TYPE_INT, "CAMERA", "previewSpeed", "20");
		list.Insert("previewSpeed", set);

		



		set = new CSetting(TYPE_INT, "SPACER", "showVobs", "1");
		list.Insert("showVobs", set);

		set = new CSetting(TYPE_INT, "SPACER", "autoCompileWorldAfterLoad", "0");
		list.Insert("autoCompileWorldAfterLoad", set);

		set = new CSetting(TYPE_INT, "SPACER", "showWaynet", "1");
		list.Insert("showWaynet", set);

		set = new CSetting(TYPE_INT, "SPACER", "showHelpVobs", "1");
		list.Insert("showHelpVobs", set);

		set = new CSetting(TYPE_INT, "SPACER", "drawBBoxGlobal", "0");
		list.Insert("drawBBoxGlobal", set);

		set = new CSetting(TYPE_INT, "SPACER", "addDatePrefix", "1");
		list.Insert("addDatePrefix", set);

		set = new CSetting(TYPE_INT, "SPACER", "autoCompileWorldLight", "0");
		list.Insert("autoCompileWorldLight", set);

		set = new CSetting(TYPE_INT, "SPACER", "autoCompileWorldLightForUnc", "0");
		list.Insert("autoCompileWorldLightForUnc", set);

		set = new CSetting(TYPE_INT, "SPACER", "removeAllLevelCompos", "0");
		list.Insert("removeAllLevelCompos", set);

		set = new CSetting(TYPE_INT, "SPACER", "checkBoxAutoRemoveAllVisuals", "0");
		list.Insert("checkBoxAutoRemoveAllVisuals", set);

		set = new CSetting(TYPE_INT, "SPACER", "checkBoxSetNearestVobCam", "0");
		list.Insert("checkBoxSetNearestVobCam", set);
		
		set = new CSetting(TYPE_INT, "SPACER", "checkBoxSetNearestVobCam", "0");
		list.Insert("checkBoxSetNearestVobCam", set);

		set = new CSetting(TYPE_INT, "SPACER", "checkBoxShowPolysSort", "1");
		list.Insert("checkBoxShowPolysSort", set);


		set = new CSetting(TYPE_INT, "SPACER", "checkBoxOnlyLatinInInput", "1");
		list.Insert("checkBoxOnlyLatinInInput", set);


		set = new CSetting(TYPE_INT, "SPACER", "showLightRadiusVob", "1");
		list.Insert("showLightRadiusVob", set);

		set = new CSetting(TYPE_INT, "SPACER", "showMoverKeysVisually", "1");
		list.Insert("showMoverKeysVisually", set);
		
		//=======================================================

		set = new CSetting(TYPE_INT, "SPACER", "maxFPS", "0");
		list.Insert("maxFPS", set);

		set = new CSetting(TYPE_INT, "SPACER", "vobListRadius", "200");
		list.Insert("vobListRadius", set);

		set = new CSetting(TYPE_INT, "SPACER", "showModelPreview", "1");
		list.Insert("showModelPreview", set);

		set = new CSetting(TYPE_INT, "SPACER", "searchOnly3DS", "0");
		list.Insert("searchOnly3DS", set);

		set = new CSetting(TYPE_INT, "SPACER", "musicVolume", "100");
		list.Insert("musicVolume", set);

		set = new CSetting(TYPE_INT, "SPACER", "musicZenOff", "0");
		list.Insert("musicZenOff", set);


		set = new CSetting(TYPE_INT, "SPACER", "soundZenOff", "0");
		list.Insert("soundZenOff", set);

		set = new CSetting(TYPE_INT, "SPACER", "alwaysShutSounds", "0");
		list.Insert("alwaysShutSounds", set);

		set = new CSetting(TYPE_INT, "SPACER", "askExitZen", "1");
		list.Insert("askExitZen", set);

		set = new CSetting(TYPE_INT, "SPACER", "lightCompileType", "0");
		list.Insert("lightCompileType", set);


		set = new CSetting(TYPE_INT, "SPACER", "lightCompileRegionOn", "0");
		list.Insert("lightCompileRegionOn", set);

		set = new CSetting(TYPE_INT, "SPACER", "lightCompileRadius", "2000");
		list.Insert("lightCompileRadius", set);


		set = new CSetting(TYPE_INT, "SPACER", "worldCompileType", "0");
		list.Insert("worldCompileType", set);


		set = new CSetting(TYPE_INT, "SPACER", "addWPToNet", "1");
		list.Insert("addWPToNet", set);

		set = new CSetting(TYPE_INT, "SPACER", "addWPAutoName", "1");
		list.Insert("addWPAutoName", set);

		set = new CSetting(TYPE_INT, "SPACER", "downFPToGround", "1");
		list.Insert("downFPToGround", set);

		set = new CSetting(TYPE_INT, "SPACER", "addFPAutoName", "1");
		list.Insert("addFPAutoName", set);

		set = new CSetting(TYPE_INT, "SPACER", "fullPathTitle", "0");
		list.Insert("fullPathTitle", set);

		set = new CSetting(TYPE_INT, "SPACER", "holdTime", "0");
		list.Insert("holdTime", set);

		set = new CSetting(TYPE_INT, "SPACER", "showBadPluginsMsg", "1");
		list.Insert("showBadPluginsMsg", set);

		set = new CSetting(TYPE_INT, "SPACER", "itemLocatorRadius", "3000");
		list.Insert("itemLocatorRadius", set);

		set = new CSetting(TYPE_INT, "SPACER", "itemLocatorEnabled", "0");
		list.Insert("itemLocatorEnabled", set);


		set = new CSetting(TYPE_INT, "SPACER", "itemLocatorOnlySusp", "0");
		list.Insert("itemLocatorOnlySusp", set);

		set = new CSetting(TYPE_INT, "SPACER", "itemLocatorOnlyByName", "0");
		list.Insert("itemLocatorOnlyByName", set);


		set = new CSetting(TYPE_STRING, "SPACER", "itemLocatorNameSearch", "");
		list.Insert("itemLocatorNameSearch", set);
		
		

		set = new CSetting(TYPE_INT, "SPACER", "bToggleWorkMode", "0");
		list.Insert("bToggleWorkMode", set);

		set = new CSetting(TYPE_INT, "SPACER", "bToggleNewController", "0");
		list.Insert("bToggleNewController", set);

		set = new CSetting(TYPE_INT, "SPACER", "bAddPlayerForPlugins", "0");
		list.Insert("bAddPlayerForPlugins", set);

		set = new CSetting(TYPE_INT, "SPACER", "checkBoxBoldFontProps", "0");
		list.Insert("checkBoxBoldFontProps", set);

		set = new CSetting(TYPE_INT, "SPACER", "checkBoxFontUnderstroke", "0");
		list.Insert("checkBoxFontUnderstroke", set);
		
		set = new CSetting(TYPE_INT, "SPACER", "bBlockPlayeUseMobInter", "1");
		list.Insert("bBlockPlayeUseMobInter", set);


		set = new CSetting(TYPE_INT, "SPACER", "infoWinzSpyEnabled", "0");
		list.Insert("infoWinzSpyEnabled", set);


		set = new CSetting(TYPE_INT, "SPACER", "infoWinzSpyLevel", "5");
		list.Insert("infoWinzSpyLevel", set);

		set = new CSetting(TYPE_INT, "SPACER", "canCompileWorldAgain", "0");
		list.Insert("canCompileWorldAgain", set);
		

		set = new CSetting(TYPE_INT, "SPACER", "selectVobTab", "0");
		list.Insert("selectVobTab", set);

		set = new CSetting(TYPE_INT, "SPACER", "safeOneMode", "0");
		list.Insert("safeOneMode", set);

		set = new CSetting(TYPE_INT, "SPACER", "oneModeToggle", "0");
		list.Insert("oneModeToggle", set);

		set = new CSetting(TYPE_INT, "SPACER", "pfxRepeatAutoplay", "1");
		list.Insert("pfxRepeatAutoplay", set);

		set = new CSetting(TYPE_INT, "SPACER", "pfxSaveFullFields", "1");
		list.Insert("pfxSaveFullFields", set);


		set = new CSetting(TYPE_STRING, "PATH", "treeVobPath", "");
		list.Insert("treeVobPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "meshPath", "");
		list.Insert("meshPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "zenzPath", "");
		list.Insert("zenzPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "vobsPath", "");
		list.Insert("vobsPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "vobResPath", "");
		list.Insert("vobResPath", set);

		set = new CSetting(TYPE_STRING, "PATH", "pfxTexturesPath", "");
		list.Insert("pfxTexturesPath", set);


		set = new CSetting(TYPE_STRING, "PATH", "pfxSavesPath", "");
		list.Insert("pfxSavesPath", set);

		set = new CSetting(TYPE_STRING, "PATH", "savedVobsVisualPath", "");
		list.Insert("savedVobsVisualPath", set);

		set = new CSetting(TYPE_STRING, "PATH", "grassWinFilePath", "");
		list.Insert("grassWinFilePath", set);

		set = new CSetting(TYPE_INT, "PATH", "openLastZen", "0");
		list.Insert("openLastZen", set);

		set = new CSetting(TYPE_STRING, "PATH", "openLastZenPath", "");
		list.Insert("openLastZenPath", set);



		set = new CSetting(TYPE_INT, "CONTROLS", "vobTransSpeed", "60");
		list.Insert("vobTransSpeed", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobRotSpeed", "32");
		list.Insert("vobRotSpeed", set);


		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertItemLevel", "1");
		list.Insert("vobInsertItemLevel", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertVobRotRand", "0");
		list.Insert("vobInsertVobRotRand", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertHierarchy", "1");
		list.Insert("vobInsertHierarchy", set);



		set = new CSetting(TYPE_INT, "CONTROLS", "wpTurnOn", "0");
		list.Insert("wpTurnOn", set);


		set = new CSetting(TYPE_INT, "CONTROLS", "selectMoveWhenVobInsert", "0");
		list.Insert("selectMoveWhenVobInsert", set);


		set = new CSetting(TYPE_INT, "CONTROLS", "translateParentAlone", "0");
		list.Insert("translateParentAlone", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "checkBoxShowVobTraceFloor", "0");
		list.Insert("checkBoxShowVobTraceFloor", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "rotModStart", "2");
		list.Insert("rotModStart", set);

		//========================================================================
		set = new CSetting(TYPE_INT, "GRASS", "grassMinDist", "150");
		list.Insert("grassMinDist", set);

		
		set = new CSetting(TYPE_STRING, "GRASS", "grassModelName", "NW_NATURE_GRASSGROUP_01.3DS");
		list.Insert("grassModelName", set);


		set = new CSetting(TYPE_INT, "GRASS", "grassVertOffset", "0");
		list.Insert("grassVertOffset", set);
		
		
		set = new CSetting(TYPE_INT, "GRASS", "grassToolRemove", "0");
		list.Insert("grassToolRemove", set);

		set = new CSetting(TYPE_INT, "GRASS", "grassToolIsItem", "0");
		list.Insert("grassToolIsItem", set);


		set = new CSetting(TYPE_INT, "GRASS", "grassToolClearMouse", "0");
		list.Insert("grassToolClearMouse", set);

		set = new CSetting(TYPE_INT, "GRASS", "grassToolDynColl", "0");
		list.Insert("grassToolDynColl", set);


		set = new CSetting(TYPE_INT, "GRASS", "grassToolRotRandAngle", "0");
		list.Insert("grassToolRotRandAngle", set);

		set = new CSetting(TYPE_INT, "GRASS", "grassToolSetNormal", "0");
		list.Insert("grassToolSetNormal", set);

		set = new CSetting(TYPE_INT, "GRASS", "grassToolGlobalParent", "0");
		list.Insert("grassToolGlobalParent", set);


		set = new CSetting(TYPE_INT, "GRASS", "grassToolcomboBoxVisualCamAlignValue", "0");
		list.Insert("grassToolcomboBoxVisualCamAlignValue", set);

		set = new CSetting(TYPE_INT, "GRASS", "grassToolcomboBoxVisualAniModeValue", "0");
		list.Insert("grassToolcomboBoxVisualAniModeValue", set);
		

		set = new CSetting(TYPE_FLOAT, "GRASS", "grassToolvisualAniModeStrengthValue", "0.0");
		list.Insert("grassToolvisualAniModeStrengthValue", set);

		set = new CSetting(TYPE_FLOAT, "GRASS", "grassToolVobFarClipZScaleValue", "1.0");
		list.Insert("grassToolVobFarClipZScaleValue", set);


		set = new CSetting(TYPE_INT, "GRASS", "grassToolcdStaticValue", "0");
		list.Insert("grassToolcdStaticValue", set);

		set = new CSetting(TYPE_INT, "GRASS", "grassToolStaticVobValue", "0");
		list.Insert("grassToolStaticVobValue", set);
	}	

	CSettings::~CSettings()
	{
		DeleteAndClearMap(list);
	}

}


