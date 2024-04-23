// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void ErrorReport::Init()
	{

	}

	

	void ErrorReport::ClearAll()
	{
		pList.DeleteListDatas();
	}

	void ErrorReport::AddEntry(ErrorReportEntry* pEntry)
	{
		pList.InsertEnd(pEntry);
	}


	void ErrorReport::SearchAll()
	{
		ClearAll();
		CheckPolygons();
		CheckVobs();

		PrintReport();
		SendAll();
	}


	void ErrorReport::SendAll()
	{
		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			if (auto pEntry = pList.GetSafe(i))
			{
				SendReport(pEntry);
			}
		}
	}

	void ErrorReport::SendReport(ErrorReportEntry* pEntry)
	{
		Stack_PushString(pEntry->textureName);
		Stack_PushString(pEntry->materialName);
		Stack_PushString(pEntry->vobName);
		Stack_PushUInt(pEntry->objectAddr);
		Stack_PushInt(pEntry->problemType);
		Stack_PushInt(pEntry->errorType);


		static auto call = (callVoidFunc)GetProcAddress(theApp.module, "Report_AddReport");

		call();
	}


	void ErrorReport::PrintReport()
	{
		//cmd << "\nErrors report. Found: " << pList.GetNumInList() << endl;

		return;

		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			if (auto pEntry = pList.GetSafe(i))
			{
				cmd << pEntry->vobName << " " << pEntry->errorType << " " << pEntry->problemType << endl;
			}
		}
	}

	

	


}