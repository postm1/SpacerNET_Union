// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	std::vector<CustomLocSettings> customLocSettingsList;


	CompileLightMod GetLightCompileTypeByString(zSTRING input)
	{
		if (input == "VERTEX")
		{
			return CompileLightMod::VertexOnly;
		}
		else if (input == "LOW")
		{
			return CompileLightMod::Low;
		}
		if (input == "MIDDLE")
		{
			return CompileLightMod::Mid;
		}
		if (input == "HIGH")
		{
			return CompileLightMod::High;
		}
	}

	void GetThisLocationTypeByCustomFile(CString checkName, int& worldCompileType)
	{
		cmd << ">>> GetThisLocationTypeByCustomFile: " << checkName << endl;

		for (auto& entry : customLocSettingsList)
		{
			if (checkName.HasWord(entry.locationName))
			{
				worldCompileType = entry.isOutdoor;
				cmd << "!!! FOUND: " << worldCompileType << endl;
				return;
			}
		}
	}

	void GetLightTypeByCustomFile(CString checkName, CompileLightMod& lightType)
	{
		cmd << ">>> GetLightTypeByCustomFile: " << checkName << endl;

		for (auto& entry : customLocSettingsList)
		{
			if (checkName.HasWord(entry.locationName))
			{
				lightType = entry.lightMod;
				cmd << "!!! FOUND_Light: " << entry.lightMod << endl;
				return;
			}
		}
	}


	void LoadCustomLocationSettings()
	{
		static std::ifstream file;

		file.open("./system/SpacerNet_Locations.txt");

		customLocSettingsList.clear();

		std::string s;
		int count = 0;

		if (file.is_open())
		{

			while (std::getline(file, s))
			{
				zSTRING str = zSTRING(s.c_str()).Upper();

				str.TrimRight(' ');
				str.TrimLeft(' ');

				if (str.Length() == 0 || str == " " || str.StartWith("//"))
				{
					continue;
				}

				count++;
				//cmd << "\"" << str << "\"" << endl;

				auto arraySplit = Split(str, '|');

				if (arraySplit.GetNum() < 3)
				{
					continue;
				}

				CustomLocSettings entry;

				entry.locationName = arraySplit[0].Upper().ToChar();
				entry.isOutdoor = arraySplit[1] == "OUTDOOR";
				entry.lightMod = GetLightCompileTypeByString(arraySplit[2]);

				customLocSettingsList.push_back(entry);

				//entry.Print();
			}

			cmd << "Loaded " << count << " custom Location Settings" << endl;

			file.close();


		}
	}

}