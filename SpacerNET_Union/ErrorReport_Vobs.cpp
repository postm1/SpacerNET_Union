// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	bool IsSpacerVob(zCVob* pVob)
	{
		if (theApp.bboxMaxsVob == pVob
			|| theApp.bboxMinsVob == pVob
			|| ogame->GetCameraVob() == pVob
			|| theApp.currenItemRender == pVob
			|| theApp.currentVobRender == pVob
			
			|| pfxManager.testVob == pVob
			|| pfxEditorVob == pVob
			|| theApp.floorVob == pVob
		)
		{
			return true;
		}

		return false;
	}
	void ErrorReport::CheckVobs()
	{
		zCArray<zCVob*> activeVobList;

		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, activeVobList, 0);

		int count = activeVobList.GetNumInList();


		for (int i = 0; i < count; i++) 
		{
			zCVob* vob = activeVobList.GetSafe(i);

			if (vob)
			{
				if (IsSpacerVob(vob))
				{
					continue;
				}

				if (auto pBadVob = vob->CastTo<zCVobLevelCompo>())
				{
					continue;
				}
				else if (auto pItem = vob->CastTo<oCItem>())
				{
					if (!pItem->visual)
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_ITEM_NO_VISUAL);
						entry->SetObject((uint)vob);
						entry->SetVobName(pItem->GetInstanceName());
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}
				else if (vob->GetClassDef() == zCVob::classDef)
				{
					if (!vob->visual)
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_ZCVOB_EMPTY_VISUAL);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->GetVobName());
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}
			}
		}
	}
}