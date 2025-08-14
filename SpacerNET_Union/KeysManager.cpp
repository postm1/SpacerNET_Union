
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	Common::Map<CString, KeyEntry*> keysMap;
	Common::Map<CString, CString> defaultValues;

	bool KeysManager::KeyPressed(CString key, bool clearKey, bool ignoreMod)
	{
		if ((GetForegroundWindow() != hWndApp && GetForegroundWindow() != theApp.mainWin && GetForegroundWindow() != theApp.vobsWin))
		{
			//print.PrintRed("blocked");
			return false;
		}

		CString value;
		CString temp_str;

		//std::string keyType = key;

		int code = 0;
		int mod = 0;

		auto& foundPair = keysMap[key];

		//if (keysMap.SearchKeyIndex(keyType) != -1)
		if (!foundPair.IsNull())
		{
			//Pair<CString, KeyEntry*> found = keysMap.SearchKey(keyType);

			code = foundPair.GetValue()->keyCode;
			mod = foundPair.GetValue()->modifier;


			//cmd << "Pressed: code: " << code << " mod: " << mod << endl;

			
			//if (found != 0)
			{
				//foundKey.GetKey();
				//code = found[0]->keyCode;
				//mod = found[0]->modifier;
			}
		}
		else
		{



			auto pair = defaultValues[key];


			if (!pair.IsNull())
			{

				theApp.spcOpt.Read(value, "KEYS", pair.GetKey(), pair.GetValue());
			}
			else
			{

				theApp.spcOpt.Read(value, "KEYS", key, "");
			}
			

			//cmd << "Read ini for: " << key << " " << value << endl;

			temp_str = value;
			mod = atoi(temp_str.GetWord('_', 1));
			temp_str = value;
			code = atoi(value.GetWord('_', 2));

			KeyEntry* entry = new KeyEntry();


			entry->keyCode = code;
			entry->modifier = mod;
			keysMap.Insert(key, entry);

		}


		bool modCondition = true;
		bool keyKodeMatch = zinput->KeyPressed(code);

		// fix for single keys like shift/ctrl/alt, if only one is pressed
		if (code == 1)
		{
			keyKodeMatch = true;
		}

		if (keyKodeMatch)
		{

			if (((mod & KeyMod::LShift) != 0) && !zinput->KeyPressed(KEY_LSHIFT)) modCondition = false;
			if (((mod & KeyMod::LControl) != 0) && !zinput->KeyPressed(KEY_LCONTROL)) modCondition = false;
			if (((mod & KeyMod::Alt) != 0) && !zinput->KeyPressed(KEY_LALT)) modCondition = false;


			if (((mod & KeyMod::RShift) != 0) && !zinput->KeyPressed(KEY_RSHIFT)) modCondition = false;
			if (((mod & KeyMod::RCtrl) != 0) && !zinput->KeyPressed(KEY_RCONTROL)) modCondition = false;
			if (((mod & KeyMod::RAlt) != 0) && !zinput->KeyPressed(KEY_RALT)) modCondition = false;

			if (!ignoreMod)
			{
				if (mod == 0 && zinput->KeyPressed(KEY_LSHIFT))  modCondition = false;
				if (mod == 0 && zinput->KeyPressed(KEY_LCONTROL))  modCondition = false;
				if (mod == 0 && zinput->KeyPressed(KEY_LALT))  modCondition = false;

				if (mod == 0 && zinput->KeyPressed(KEY_RSHIFT))  modCondition = false;
				if (mod == 0 && zinput->KeyPressed(KEY_RCONTROL))  modCondition = false;
				if (mod == 0 && zinput->KeyPressed(KEY_RALT))  modCondition = false;
			}
		}

		bool result = modCondition && keyKodeMatch;

		if (result && clearKey)
		{
			zinput->ClearKey(code);
		}

		return result;

	}

	void KeysManager::InitValues()
	{
		cmd << "KeysManager: InitValues" << endl;

		defaultValues.Insert("CAMERA_TRANS_FORWARD", "0_17");
		defaultValues.Insert("CAMERA_TRANS_BACKWARD", "0_31");
		defaultValues.Insert("CAMERA_TRANS_RIGHT", "0_32");
		defaultValues.Insert("CAMERA_TRANS_LEFT", "0_30");
		defaultValues.Insert("CAMERA_TRANS_UP", "0_57");
		defaultValues.Insert("CAMERA_TRANS_DOWN", "0_45");
		defaultValues.Insert("VOB_COPY", "2_46");
		defaultValues.Insert("VOB_INSERT", "2_47");
		defaultValues.Insert("VOB_CUT", "2_44");
		defaultValues.Insert("VOB_SET_ZERO_PARENT", "8_44");
		defaultValues.Insert("VOB_NEAR_CAM", "0_6");
		defaultValues.Insert("VOB_TRANSLATE", "0_2");
		defaultValues.Insert("VOB_ROTATE", "0_3");
		defaultValues.Insert("WP_TOGGLE", "0_4");
		defaultValues.Insert("VOB_ONEMODE", "0_50");
		defaultValues.Insert("VOB_DISABLE_SELECT", "0_5");
		defaultValues.Insert("VOB_FLOOR", "8_33");
		defaultValues.Insert("VOB_RESET_AXIS", "8_19");
		defaultValues.Insert("VOB_TRANS_FORWARD", "0_17");
		defaultValues.Insert("VOB_TRANS_BACKWARD", "0_31");
		defaultValues.Insert("VOB_TRANS_LEFT", "0_30");
		defaultValues.Insert("VOB_TRANS_RIGHT", "0_32");
		defaultValues.Insert("VOB_TRANS_UP", "0_57");
		defaultValues.Insert("VOB_TRANS_DOWN", "0_45");
		defaultValues.Insert("CAM_SPEED_X10", "8_1");
		defaultValues.Insert("CAM_SPEED_MINUS_10", "2_1");
		defaultValues.Insert("VOB_SPEED_X10", "8_1");
		defaultValues.Insert("VOB_SPEED_MINUS_10", "2_1");
		defaultValues.Insert("VOB_DELETE", "0_211");
		defaultValues.Insert("VOB_ROT_VERT_RIGHT", "0_18");
		defaultValues.Insert("VOB_ROT_VERT_LEFT", "0_16");
		defaultValues.Insert("VOB_ROT_FORWARD", "0_17");
		defaultValues.Insert("VOB_ROT_BACK", "0_31");
		defaultValues.Insert("VOB_ROT_RIGHT", "0_32");
		defaultValues.Insert("VOB_ROT_LEFT", "0_30");
		defaultValues.Insert("VOBLIST_COLLECT", "0_59");
		defaultValues.Insert("WP_CREATEFAST", "0_60");
		defaultValues.Insert("WIN_HIDEALL", "0_61");
		defaultValues.Insert("GAME_MODE", "0_63");
		defaultValues.Insert("OPEN_CONTAINER", "8_20");
		defaultValues.Insert("TOGGLE_VOBS", "8_2");
		defaultValues.Insert("TOGGLE_WAYNET", "8_3");
		defaultValues.Insert("TOGGLE_HELPERS", "8_4");
		defaultValues.Insert("TOGGLE_BBOX", "8_5");
		defaultValues.Insert("TOGGLE_INVIS", "8_6");
		defaultValues.Insert("TOGGLE_MUSIC", "0_22");
		defaultValues.Insert("TOGGLE_SCREENINFO", "0_23");
		defaultValues.Insert("LIGHT_RAD_INC", "0_78");
		defaultValues.Insert("LIGHT_RAD_DEC", "0_74");
		defaultValues.Insert("LIGHT_RAD_ZERO", "0_55");


		defaultValues.Insert("TOOL_BBOX_CHANGE", "0_7");

		defaultValues.Insert("TOOL_BBOX_MAXS", "0_2");
		defaultValues.Insert("TOOL_BBOX_MINS", "0_3");
		
		//defaultValues.Insert("TEST_MOVE_LC", "0_6");

		

		defaultValues.Insert("VOB_RESTORE_POS", "2_19");
		defaultValues.Insert("VOB_SET_UPSIDE_DOWN", "8_22"); // SHIFT+U
		defaultValues.Insert("VOB_ADD_FAV", "8_34");
		defaultValues.Insert("CAMERA_SYNC_VOB_MOD", "8_35");//SHIFT+H
		defaultValues.Insert("ROT_MOD_CHANGE", "8_20");//SHIFT+T
		defaultValues.Insert("FAST_FILTER_IGNOREPFX", "8_47");//SHIFT+V
		defaultValues.Insert("CHANGE_VOB_COMPLEX_MOVE", "8_7");//SHIFT+6

		defaultValues.Insert("FASTKEY_OPEN_ZEN", "2_24");//CTRL+O
		defaultValues.Insert("FASTKEY_OPEN_MESH", "2_50");//CTRL+M
		
		
		auto arr = defaultValues.GetArray();

		CString value = "";

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{
				theApp.spcOpt.Read(value, "KEYS", pair->GetKey(), pair->GetValue());
			}
			
		}

		
	}

	void KeysManager::UpdateKey(CString keyType, int code, int mod)
	{

		cmd << "Union: UpdateKey " << keyType << " Code: " << code << " mod: " << mod << endl;


		auto& foundPair = keysMap[keyType];

		if (!foundPair.IsNull() && code >= 0)
		{
			foundPair.GetValue()->keyCode = code;
			foundPair.GetValue()->modifier = mod;
		}
		else
		{
			KeyEntry* entry = new KeyEntry();

			entry->keyCode = code;
			entry->modifier = mod;
			keysMap.Insert(keyType, entry);
		}

		CString value = CString(mod) + "_" + CString(code);

		theApp.spcOpt.Write(value, "KEYS", keyType);

		//cmd << "Union: Update keys ok: " << keyType << endl;

	}


	CString KeysManager::GetIniValue(CString key)
	{
		CString value = "";


		auto pair = defaultValues[key];

		if (!pair.IsNull())
		{
			theApp.spcOpt.Read(value, "KEYS", pair.GetKey(), pair.GetValue());
		}
		else
		{
			theApp.spcOpt.Read(value, "KEYS", key, "");
		}


		return value;
	}


	void KeysManager::ResetDefault()
	{
		DeleteAndClearMap(keysMap);

		auto arr = defaultValues.GetArray();

		CString value = "";

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{
				theApp.spcOpt.Write(pair->GetValue(), "KEYS", pair->GetKey());
			}

		}

	}

}