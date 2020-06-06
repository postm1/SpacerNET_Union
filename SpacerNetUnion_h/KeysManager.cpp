// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	Common::Map<CString, KeyEntry*> keysMap;

	bool KeysManager::KeyPressed(CString key, bool clearKey, bool ignoreMod)
	{
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
			//if (found != 0)
			{
				//foundKey.GetKey();
				//code = found[0]->keyCode;
				//mod = found[0]->modifier;
			}
		}
		else
		{

			theApp.spcOpt.Read(value, "KEYS", key, "");

			//cmd << "Read ini for: " << keyType << " " << value << endl;
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

		// фикс для одиночных клавиш типа shift/ctrl/alt, если нажата только она
		if (code == 1)
		{
			keyKodeMatch = true;
		}

		if (keyKodeMatch)
		{

			if (((mod & KeyMod::LShift) != 0) && !zinput->KeyPressed(KEY_LSHIFT)) modCondition = false;
			if (((mod & KeyMod::LControl) != 0) && !zinput->KeyPressed(KEY_LCONTROL)) modCondition = false;
			if (((mod & KeyMod::Alt) != 0) && !zinput->KeyPressed(KEY_LALT)) modCondition = false;

			if (!ignoreMod)
			{
				if (mod == 0 && zinput->KeyPressed(KEY_LSHIFT))  modCondition = false;
				if (mod == 0 && zinput->KeyPressed(KEY_LCONTROL))  modCondition = false;
				if (mod == 0 && zinput->KeyPressed(KEY_LALT))  modCondition = false;
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
		CString value = "";

		theApp.spcOpt.Read(value, "KEYS", "CAMERA_TRANS_FORWARD", "0_17");
		theApp.spcOpt.Read(value, "KEYS", "CAMERA_TRANS_BACKWARD", "0_31");
		theApp.spcOpt.Read(value, "KEYS", "CAMERA_TRANS_RIGHT", "0_32");
		theApp.spcOpt.Read(value, "KEYS", "CAMERA_TRANS_LEFT", "0_30");
		theApp.spcOpt.Read(value, "KEYS", "CAMERA_TRANS_UP", "0_57");
		theApp.spcOpt.Read(value, "KEYS", "CAMERA_TRANS_DOWN", "0_45");


		theApp.spcOpt.Read(value, "KEYS", "VOB_COPY", "2_46");
		theApp.spcOpt.Read(value, "KEYS", "VOB_INSERT", "2_47");
		theApp.spcOpt.Read(value, "KEYS", "VOB_CUT", "2_44");
		theApp.spcOpt.Read(value, "KEYS", "VOB_NEAR_CAM", "0_6");


		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANSLATE", "0_2");
		theApp.spcOpt.Read(value, "KEYS", "VOB_ROTATE", "0_3");
		theApp.spcOpt.Read(value, "KEYS", "WP_TOGGLE", "0_4");
		theApp.spcOpt.Read(value, "KEYS", "VOB_DISABLE_SELECT", "0_5");
		theApp.spcOpt.Read(value, "KEYS", "VOB_FLOOR", "8_33");
		theApp.spcOpt.Read(value, "KEYS", "VOB_RESET_AXIS", "8_19");


		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANS_FORWARD", "0_17");
		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANS_BACKWARD", "0_31");
		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANS_LEFT", "0_30");
		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANS_RIGHT", "0_32");

		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANS_UP", "0_57");
		theApp.spcOpt.Read(value, "KEYS", "VOB_TRANS_DOWN", "0_45");


		theApp.spcOpt.Read(value, "KEYS", "CAM_SPEED_X10", "8_1");
		theApp.spcOpt.Read(value, "KEYS", "CAM_SPEED_MINUS_10", "2_1");
		theApp.spcOpt.Read(value, "KEYS", "VOB_SPEED_X10", "8_1");
		theApp.spcOpt.Read(value, "KEYS", "VOB_SPEED_MINUS_10", "2_1");

		theApp.spcOpt.Read(value, "KEYS", "VOB_DELETE", "0_211");

		theApp.spcOpt.Read(value, "KEYS", "VOB_ROT_VERT_RIGHT", "0_18");
		theApp.spcOpt.Read(value, "KEYS", "VOB_ROT_VERT_LEFT", "0_16");

		theApp.spcOpt.Read(value, "KEYS", "VOB_ROT_FORWARD", "0_17");
		theApp.spcOpt.Read(value, "KEYS", "VOB_ROT_BACK", "0_31");
		theApp.spcOpt.Read(value, "KEYS", "VOB_ROT_RIGHT", "0_32");
		theApp.spcOpt.Read(value, "KEYS", "VOB_ROT_LEFT", "0_30");


		theApp.spcOpt.Read(value, "KEYS", "VOBLIST_COLLECT", "0_59");
		theApp.spcOpt.Read(value, "KEYS", "WP_CREATEFAST", "0_60");
		theApp.spcOpt.Read(value, "KEYS", "WIN_HIDEALL", "0_61");

		theApp.spcOpt.Read(value, "KEYS", "OPEN_CONTAINER", "8_20");

		theApp.spcOpt.Read(value, "KEYS", "TOGGLE_VOBS", "8_2");
		theApp.spcOpt.Read(value, "KEYS", "TOGGLE_WAYNET", "8_3");
		theApp.spcOpt.Read(value, "KEYS", "TOGGLE_HELPERS", "8_4");
		theApp.spcOpt.Read(value, "KEYS", "TOGGLE_BBOX", "8_5");
		theApp.spcOpt.Read(value, "KEYS", "TOGGLE_INVIS", "8_6");
	}

	void KeysManager::UpdateKey(CString keyType, int code, int mod)
	{

		cmd << "Union: UpdateKey " << keyType << " Code: " << code << " mod: " << mod << endl;


		auto& foundPair = keysMap[keyType];

		//if (keysMap.SearchKeyIndex(keyType) != -1)
		if (!foundPair.IsNull() && code >= 0)
		{
			foundPair.GetValue()->keyCode = code;
			foundPair.GetValue()->modifier = mod;
		}


		/*if (keysMap.SearchKeyIndex(keyType) != -1 && code >= 0)
		{
			Pair<CString, KeyEntry*> found = keysMap.SearchKey(keyType);

			if (found != 0)
			{
				found[0]->keyCode = code;
				found[0]->modifier = mod;

			}

		}
		*/
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

		theApp.spcOpt.Read(value, "KEYS", key, "");

		return value;
	}

}