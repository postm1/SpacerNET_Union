// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	
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

					if (pItem->HasChildren())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_ITEM_CANT_BE_PARENT);
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
					else
					{
						if (auto visual = vob->visual->CastTo<zCParticleFX>())
						{
							if (vob->HasChildren())
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_PFX_CANT_BE_PARENT);
								entry->SetObject((uint)vob);
								entry->SetVobName(vob->GetVobName());
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
						}
					}
				}
				
				if (vob->CastTo<zCTrigger>() || vob->CastTo<zCVobWaypoint>() || vob->CastTo<zCVobSpot>())
				{
					if (vob->GetVobName().IsEmpty())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->_GetClassDef()->className);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (auto pMob = vob->CastTo<oCMobContainer>())
				{
					if (pMob->focusNameIndex == 0 || pMob->focusNameIndex == -1 || pMob->name.IsEmpty())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME_MOB_FOCUS);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->_GetClassDef()->className);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (auto pMob = vob->CastTo<oCMobBed>())
				{
					if (pMob->focusNameIndex == 0 || pMob->focusNameIndex == -1 || pMob->name.IsEmpty())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME_MOB_FOCUS);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->_GetClassDef()->className);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (vob->GetVobName().contains(' '))
				{
					auto entry = new ErrorReportEntry();

					entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
					entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_NAME_SPACE);
					entry->SetObject((uint)vob);
					entry->SetVobName(vob->GetVobName());
					entry->SetMaterialName("");
					entry->SetTextureName("");

					AddEntry(entry);
				}
			}
		}


		zCArray<zCVob*> resultList;
		zCVob* vob = NULL;

		ogame->GetWorld()->SearchVobListByClass(zCZoneZFogDefault::classDef, resultList, 0);

		if (resultList.GetNumInList() != 1)
		{
			if (resultList.GetNumInList() > 0)
			{
				vob = resultList.GetSafe(0);
			}
			else
			{
				vob = NULL;
			}

			auto entry = new ErrorReportEntry();

			entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
			entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_FOG_ZONES);
			entry->SetObject((uint)vob);
			entry->SetVobName("");
			entry->SetMaterialName("");
			entry->SetTextureName("");

			AddEntry(entry);
		}

		resultList.DeleteList();


		ogame->GetWorld()->SearchVobListByClass(zCZoneVobFarPlaneDefault::classDef, resultList, 0);

		if (resultList.GetNumInList() != 1)
		{
			if (resultList.GetNumInList() > 0)
			{
				vob = resultList.GetSafe(0);
			}
			else
			{
				vob = NULL;
			}

			auto entry = new ErrorReportEntry();

			entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
			entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_VOB_ZONES);
			entry->SetObject((uint)vob);
			entry->SetVobName("");
			entry->SetMaterialName("");
			entry->SetTextureName("");

			AddEntry(entry);
		}

		resultList.DeleteList();
		

		ogame->GetWorld()->SearchVobListByClass(oCZoneMusicDefault::classDef, resultList, 0);

		if (resultList.GetNumInList() != 1)
		{

			if (resultList.GetNumInList() > 0)
			{
				vob = resultList.GetSafe(0);
			}
			else
			{
				vob = NULL;
			}

			auto entry = new ErrorReportEntry();

			entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
			entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MUSIC_ZONES);
			entry->SetObject((uint)vob);
			entry->SetVobName("");
			entry->SetMaterialName("");
			entry->SetTextureName("");

			AddEntry(entry);
		}

		resultList.DeleteList();

		ogame->GetWorld()->SearchVobListByClass(zCVobStartpoint::classDef, resultList, 0);

		if (resultList.GetNumInList() > 1)
		{
			if (resultList.GetNumInList() > 0)
			{
				vob = resultList.GetSafe(0);
			}
			else
			{
				vob = NULL;
			}

			auto entry = new ErrorReportEntry();

			entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
			entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_STARTPOINT);
			entry->SetObject((uint)vob);
			entry->SetVobName("");
			entry->SetMaterialName("");
			entry->SetTextureName("");

			AddEntry(entry);
		}

		resultList.DeleteList();
	}
}