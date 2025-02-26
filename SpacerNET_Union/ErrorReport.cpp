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

		SortReports();

		SendAll();
	}

	bool CompareElements(const ErrorReportEntry* elem1, const ErrorReportEntry* elem2) {

		// Сравниваем значения enum
		return elem1->problemType < elem2->problemType;
	}

	void ErrorReport::SortReports()
	{
		std::vector<ErrorReportEntry*> pElems;
		pElems.reserve(pList.GetNumInList());

		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			pElems.push_back(pList.GetSafe(i));
		}
		
		std::sort(pElems.begin(), pElems.end(), CompareElements);

		pList.DeleteList();

		for (const auto& elem : pElems) 
		{
			pList.InsertEnd(elem);
			//cmd << elem->problemType << ": " << elem->vobName << endl;
		}
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

	

	void ErrorReport::DebugClearVisualsList()
	{
		pDebugList.DeleteList();
	}

	void ErrorReport::DebugAddVisualInList(zSTRING visual)
	{
		if (!pDebugList.IsInList(visual))
		{
			pDebugList.InsertEnd(visual);
		}
	}


	void ErrorReport::DebugRemoveVisuals()
	{
		cmd << "DebugRemoveVisuals: " << pDebugList.GetNumInList() << endl;

		zCArray<zCVob*> activeVobList;

		ogame->GetWorld()->SearchVobListByClass(zCVob::classDef, activeVobList, 0);

		int count = activeVobList.GetNumInList();


		theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);

		for (int i = 0; i < count; i++)
		{
			zCVob* vob = activeVobList.GetSafe(i);

			if (vob)
			{
				if (IsSpacerVob(vob))
				{
					continue;
				}

				if (auto visual = vob->GetVisual())
				{
					if (visual->GetVisualName().Length() > 0)
					{
						if (pDebugList.IsInList(visual->GetVisualName()))
						{
							//cmd << "Remove: " << visual->GetVisualName() << endl;

							theApp.RemoveVob(vob);
						}
					}
					
				}
			}
		}


		theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
	}
}