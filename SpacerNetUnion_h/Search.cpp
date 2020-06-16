

// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void SpacerApp::SearchSelectVob(zCVob * pickedVob)
	{
		if (!pickedVob || dynamic_cast<zCVobLevelCompo*>(pickedVob))
		{
			// dialog leer
			search.cur_vob = NULL;
			Stack_PushString("");
		}
		else
		{
			search.cur_vob = pickedVob;

			zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, 0, 0);
			arch->SetStringEOL(zSTRING("\n"));
			arch->WriteObject(search.cur_vob);
			zSTRING archBuffer;
			arch->GetBufferString(archBuffer);

			Stack_PushString(archBuffer);

			arch->Close();
			zRELEASE(arch);

		}
	}

	void SpacerApp::SearchFillVobClass(CString vobClass)
	{

		if (search.cur_vob)
		{
			zRELEASE(search.cur_vob);
		}
		
		search.cur_vob = (zCVob*)zCObject::CreateNewInstance(vobClass);

		SearchSelectVob(search.cur_vob);
	}


	zCArray<zCVob*> resultFound;

	bool SpacerApp::SearchHandleVob(zCVob*& vob)
	{
		if (!vob) return false;


		zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, 0, 0);
		arch->SetStringEOL(zSTRING("\n"));
		arch->WriteObject(vob);
		zSTRING archBuffer;
		arch->GetBufferString(archBuffer);

		Stack_PushString(archBuffer);

		arch->Close();
		zRELEASE(arch);

		static auto compare = (compareVobs)GetProcAddress(theApp.module, "CompareVobs");

		if (compare())
		{
			resultFound.Insert(vob);
		}

		return true;
	}

	void SpacerApp::SearchFillVobClass(bool derived)
	{

		zCArray<zCVob*> result;
		resultFound.DeleteList();

		ogame->GetWorld()->SearchVobListByClass(search.cur_vob->GetClassDef(), result, 0);

		if (derived)
		{
			ogame->GetWorld()->SearchVobListByBaseClass(search.cur_vob->GetClassDef(), result, 0);
		}

		int num = result.GetNumInList();

		for (int i = 0; i<num; i++)
		{
			if (dynamic_cast<zCVobLevelCompo*>(result[i]))	continue;
			if (result[i] == ogame->GetCamera()->GetVob())	continue;

			SearchHandleVob(result[i]);
		}

		static auto callFunc = (addToVobList)GetProcAddress(theApp.module, "AddSearchVobResult");
		
		for (int i = 0; i < resultFound.GetNum(); i++)
		{
			Stack_PushString(GetVobName(resultFound[i]));

			callFunc((uint)resultFound[i]);
		}
	}
}