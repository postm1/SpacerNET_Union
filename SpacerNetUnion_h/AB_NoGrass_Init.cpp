// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void AB_NoGrass::AppendFromFile()
	{
		static std::ifstream file;

		file.open("./system/SpacerNet_HideList.txt");

		std::string s;
		int count = 0;

		if (file.is_open())
		{

			while (std::getline(file, s))
			{
				zSTRING str = zSTRING(s.c_str()).Upper();


				

				
				str.TrimRight(' ');
				str.TrimLeft(' ');

				if (str.Length() == 0 || str == " ")
				{
					continue;
				}
				count++;
				//cmd << "\"" << str << "\"" << endl;

				grassList.Insert(str);
			}

			cmd << "Loaded " << count << " custom model names for NoGrass hiding" << endl;

			file.close();


		}
	}

	void AB_NoGrass::Init()
	{

		// список вобов, которые будем скрывать
		grassList.Insert("NW_NATURE_GRASSGROUP_01.3DS");
		grassList.Insert("NW_NATURE_GRASSGROUP_02.3DS");

		grassList.Insert("NW_NATURE_WATERGRASS_56P.3DS");
		grassList.Insert("NW_GRASSGROUP_01.3DS");
		grassList.Insert("NW_NATURE_GREAT_WEED_XGROUP.3DS");
		grassList.Insert("NW_GRASSGROUP_01.3DS");
		grassList.Insert("NW_NATURE_WATERGRASS_56W.3DS");


		grassList.Insert("AV_NATURE_BUSH_01.3DS");
		grassList.Insert("AV_NATURE_BUSH_02.3DS");
		grassList.Insert("ADDON_PLANTS_BUSH_01_36P.3DS");
		grassList.Insert("AV_NATURE_BUSH_02.3DS");


		grassList.Insert("NW_NATURE_FARNTEPPICH_306P.3DS");
		grassList.Insert("NW_NATURE_FARN_102P.3DS");
		grassList.Insert("NW_NATURE_FARNTEPPICH_612P.3DS");
		grassList.Insert("OW_NATURE_BUSH_BIG_01.3DS");
		grassList.Insert("OW_NATURE_BUSH_02.3DS"); // да, это тоже трава, название такое
		grassList.Insert("OW_NATURE_BUSH_03.3DS"); // да, это тоже трава, название такое
		grassList.Insert("OZ_ON_CANYONPLANT_BUSHES_03_168P.3DS"); // да, это тоже трава, название такое


		grassList.Insert("NW_CAVE_PLANTS_01.3DS");
		grassList.Insert("NW_CAVE_PLANTS_02.3DS");
		grassList.Insert("NW_CAVE_PLANTS_03.3DS");
		grassList.Insert("NW_CAVE_PLANTS_04.3DS");
		grassList.Insert("NW_CAVE_PLANTS_05.3DS");

		grassList.Insert("XW_NATURE_BUSH_BIG_01.3DS");
		grassList.Insert("OW_NATURE_BUSH_BIG_02.3DS");


		grassList.Insert("GRASS_WUESTE1.3DS");
		grassList.Insert("GRASS_WUESTE2.3DS");

		grassList.Insert("NW_FERN_FOREST_CAVE_02_HL.3DS");
		grassList.Insert("KB_UNTERHOLZ_01.3DS");
		grassList.Insert("KB_UNTERHOLZ_02.3DS");
		grassList.Insert("KB_UNTERHOLZ_03.3DS");
		grassList.Insert("KB_UNTERHOLZ_04.3DS");
		grassList.Insert("NW_NATURE_SMALLWEED_24P.3DS");

		grassList.Insert("NW_FERN_FOREST_CAVE_02.3DS");
		grassList.Insert("NW_FERN_FOREST_CAVE_01.3DS");
		grassList.Insert("NW_FERN_FOREST_CAVE_03.3DS");
		grassList.Insert("NW_NATURE_WATERGRASS_02_100P_NEW.3DS");
		grassList.Insert("OW_LOB_BUSH_V8.3DS");
		grassList.Insert("NW_NATURE_SIDEPLANT_01.3DS");
		grassList.Insert("NW_NATURE_SIDEPLANT_BIG_01.3DS");
		grassList.Insert("OW_CAVEWEBS_V1.3DS");


		//=============================================================

		bushList.Insert("NW_NATURE_LONG_BUSH_360P.3DS");
		bushList.Insert("NW_NATURE_LONG_BUSH_370P.3DS");
		bushList.Insert("GRASS_01.3DS");
		bushList.Insert("NW_NATURE_WATERGRASS_56P_NEW.3DS");
		bushList.Insert("NW_NATURE_PLANT_02.3DS");
		bushList.Insert("OW_LOB_BUSH_REED_V1.3DS");
		bushList.Insert("NW_NATURE_WATERGRASS_02_100P.3DS");
		bushList.Insert("NW_NATURE_WATERGRASS_02_900P.3DS");
		bushList.Insert("NW_NATURE_GRASSGROUP_01LPS_NEW.3DS");
		bushList.Insert("OW_LOB_BUSH_V7.3DS");
		bushList.Insert("NW_GROUNDCLOVER03.3DS");
		bushList.Insert("OW_BUSHES_01.3DS");

		bushList.Insert("OW_LOB_BUSH_V4.3DS");
		bushList.Insert("OW_LOB_BUSH_V5.3DS");
		bushList.Insert("OW_BUSHES_01.3DS");

		bushList.Insert("ADDON_CANYONPLANT_BUSHES_02_240P.3DS");
		bushList.Insert("ADDON_CANYONPLANT_BUSHES_01_120P.3DS");
		bushList.Insert("ADDON_CANYONPLANT_BUSHES_05_168P.3DS");
		bushList.Insert("ADDON_CANYONPLANT_BUSHES_04_240P.3DS");

		bushList.Insert("OW_LOB_PSIBUSHES_V1.3DS");
		bushList.Insert("NW_CAVEPLANTS_06.3DS");
		bushList.Insert("OW_CAVEWEBS_V2.3DS");
		hiddenAmount = 0;
		bariCenter = zVEC3(0, 0, 0);
		percentGrass = 100;
		percentBush = 100;
		hideBush = true;
		hideGrass = true;
		hideActive = false;


		AppendFromFile();
	}


	void AB_NoGrass::SetPercentFromSettings(bool forceUpdate)
	{

		//cmd << "SetPercentFromSettings" << endl;
		
		bool flagUpdate = false;

		if (forceUpdate)
		{
			flagUpdate = true;
		}

		if (flagUpdate)
		{
			UpdateSettings();
		}

	}

	void AB_NoGrass::Loop()
	{


		//SetPercentFromSettings();

		return;
		/*
		if (zKeyPressed(KEY_F1)) {
		INP_CLR;

		RestoreObjects();
		CollectVobsFromLocation();

		}

		if (zKeyPressed(KEY_F2)) {
		INP_CLR;

		RestoreObjects();

		}
		*/

		
		PrintDebug("Filtered list: " + Z filteredList.GetNum());
		PrintDebug("GrassList: " + Z hiddenAmount);
		PrintDebug("HiddenList: " + Z hiddenList.GetNum());
		PrintDebug("GrassHide: " + Z percentGrass + "%");
		PrintDebug("BushHide: " + Z percentBush + "%");


	}
}