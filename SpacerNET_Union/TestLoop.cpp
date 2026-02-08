// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .



	void ScaleVob(zCVob* pVob, float scaleFactor)
	{
		zMAT4& trafo = pVob->GetNewTrafoObjToWorld();


		//trafo.MakeIdentity();
		//trafo.PreScale(0.97f);
		/*
		auto rot = trafo.ExtractRotation();

		trafo.ResetRotation();
		trafo.PostScale(scaleFactor);
		trafo *= rot;
		*/


		cmd << "PRE: " << trafo.ToString().c_str() << endl;

		print.PrintGreen("SCALE");
		trafo.PostScaleNew(scaleFactor);

		pVob->SetPositionWorld(pVob->GetPositionWorld());

		cmd << "AFTER: " << trafo.ToString().c_str() << endl;
	}


	std::unordered_map<std::string, float> mapValues;

	void ReadFileData()
	{
		cmd << "Reading from file ./system/WindObjects.txt" << endl;

		std::ifstream inputFile("./system/WindObjects.txt");

		if (!inputFile.is_open())
		{
			cmd << "Error: no file found in system folder" << endl;
			return;
		}


		std::string line;
		while (std::getline(inputFile, line)) {

			if (line.empty() || (line.substr(0, 2) == "//")) {
				continue;
			}


			size_t delimiterPos = line.find('|');
			if (delimiterPos == std::string::npos) {
				continue;
			}

			std::string name = line.substr(0, delimiterPos);
			std::string valueStr = line.substr(delimiterPos + 1);

			
			if (mapValues.find(name) != mapValues.end())
			{
				cmd << "BAD VALUE DUBLI: " << name.c_str() << endl;

			}
			else
			{
				float value = std::stof(valueStr);
				mapValues[name] = value;
			}
		}

		inputFile.close();


		//cmd << "Reading OK: " << mapValues.size() << endl;

		/*
		for (const auto& pair : mapValues)
		{
			cmd << pair.first.c_str() << " | " << pair.second << endl;
		}
		*/
	}
#if ENGINE == Engine_G2A
	void PrintMusicInfo()
	{
		zCArray<zCVob*> pList;

		ogame->GetWorld()->SearchVobListByClass(oCZoneMusicDefault::classDef, pList, 0);
		ogame->GetWorld()->SearchVobListByClass(oCZoneMusic::classDef, pList, 0);

		cmd << "\nLocation: " << ogame->GetWorld()->m_strlevelName << endl;

		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			if (auto pMusic = pList.GetSafe(i)->CastTo<oCZoneMusic>())
			{
				cmd << "Music: " << pMusic->GetVobName()
					<< " Loop: " << pMusic->loop
					<< " Priority: " << pMusic->priority
					<< " Default: " << bool(pMusic->CastTo<oCZoneMusicDefault>())
					<< endl
					;
			}
		}

	

		cmd << "=============================" << endl;
	}
#endif

	// test loop, some test code always here...
	void SpacerApp::TestLoop()
	{
		return;
		if (zinput->KeyPressed(KEY_F1))
		{
			zinput->ClearKeyBuffer();

			//PrintMusicInfo();
		}

		return;
		
		if (zinput->KeyPressed(KEY_F1))
		{
			zinput->ClearKeyBuffer();

			mapValues.clear();
			debug.CleanLines();
			ReadFileData();
			print.PrintGreen("OOOK");

			zCArray<zCVob*> pList;

			ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, pList, 0);

			int countRemoved = 0;

			for (int i = 0; i < pList.GetNumInList(); i++)
			{
				zCVob* pVob = pList.GetSafe(i);

				if (pVob) {

					if (pVob->CastTo<zCTriggerBase>() || pVob->CastTo<zCZone>())
					{
						continue;
					}

					if (pVob->GetVisual() && pVob->showVisual && !pVob->HasChildren())
					{
						if (pVob->GetVisual()->GetVisualName().Contains(".3DS"))
						{
							auto visualNameCheck = pVob->GetVisual()->GetVisualName();

							visualNameCheck.TrimLeft(' ');
							visualNameCheck.TrimRight(' ');

							if (mapValues.find(visualNameCheck.ToChar()) != mapValues.end())
							{
								RemoveVob(pVob);
								countRemoved++;
							}
						}
					}
				}
			}
			
			cmd << "Removed: " << countRemoved << endl;

			std::unordered_set<std::string> foundVobs;

			int count = 0;

			for (int i = 0; i < pList.GetNumInList(); i++) {

				zCVob* pVob = pList.GetSafe(i);

				if (pVob) {

					if (pVob->GetVisual() && pVob->showVisual)
					{
						if (pVob->GetVisual()->GetVisualName().Contains(".3DS"))
						{
							if (mapValues.find(pVob->GetVisual()->GetVisualName().ToChar()) == mapValues.end() && foundVobs.find(pVob->GetVisual()->GetVisualName().ToChar()) == foundVobs.end())
							{
								foundVobs.insert(pVob->GetVisual()->GetVisualName().ToChar());

								auto pos = pVob->GetPositionWorld();

								debug.AddLine(pos, pos + zVEC3(0, 5000, 0), GFX_RED, 1000e3);
								
							}
						}
					}

				}
			}
		}

		/*if (auto pVob = GetSelectedVob())
		{
			if (zinput->KeyPressed(KEY_F1))
			{
				ScaleVob(pVob, 0.95f);
			}

			if (zinput->KeyPressed(KEY_F2))
			{
				ScaleVob(pVob, 1.10f);
			}
		}*/
		
		return;
	}
	
}