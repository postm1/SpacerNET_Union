// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	
	void ErrorReport::CheckVobs()
	{
		zCArray<zCVob*> activeVobList;

		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, activeVobList, 0);

		int count = activeVobList.GetNumInList();


		zCArray<zSTRING> moversNames;
		zCArray<zSTRING> wpNames;
		zCArray<zSTRING> fpNames;
		zCArray<zSTRING> containerNames;

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

						entry->SetErrorType(ERROR_REPORT_TYPE_INFO);
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
				
				if (vob->visual)
				{
					zSTRING visName = vob->visual->GetVisualName();
					zSTRING name = vob->GetVobName();
					

					auto pItem = vob->CastTo<oCItem>();
					

					if (!pItem && name.Length() > 0)
					{
						size_t lastindex = visName.SearchReverse(".");
						zSTRING visNameWork = visName;

						zSTRING rawname = visNameWork.Cut(lastindex, visNameWork.Length() - lastindex);

						//cmd << visName << ";" << name << ";" << rawname << endl;

						if (visName == name || name == rawname)
						{
							

							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_INFO);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_NAME_IS_VISUAL);
							entry->SetObject((uint)vob);
							entry->SetVobName(name);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
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


					if (auto zTrigger = vob->CastTo<zCTrigger>())
					{
						auto triggerTarget = zTrigger->triggerTarget;

						if (triggerTarget.Length() > 0)
						{
							if (!FindTriggerByName(triggerTarget))
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_TRIGGER);
								entry->SetObject((uint)vob);
								entry->SetVobName(triggerTarget);
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
						}
					}
					
				}

				if (auto pMob = vob->CastTo<oCMobContainer>())
				{
					if (pMob->focusNameIndex == 0 || pMob->focusNameIndex == -1 || pMob->name.IsEmpty())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_INFO);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME_MOB_FOCUS);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->_GetClassDef()->className);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (auto pMobInter = vob->CastTo<oCMobInter>())
				{
					auto useWithItem = pMobInter->useWithItem;


					if (useWithItem.Length() > 0)
					{
						if (!IsItemExistsInScript(useWithItem))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_USE_WITHITEM);
							entry->SetObject((uint)vob);
							entry->SetVobName(useWithItem);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}
					

					auto triggerName = pMobInter->triggerTarget;

					if (triggerName.Length() > 0)
					{
						if (!FindTriggerByName(triggerName))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_TRIGGER);
							entry->SetObject((uint)vob);
							entry->SetVobName(triggerName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}

					auto condFunc = pMobInter->conditionFunc;

					if (condFunc.Length() > 0)
					{
						if (!parser->GetSymbol(condFunc))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_COND_FUNC);
							entry->SetObject((uint)vob);
							entry->SetVobName(condFunc);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}
					

					auto onStateFunc = pMobInter->onStateFuncName;

					if (onStateFunc.Length() > 0)
					{
						if (!parser->GetSymbol(onStateFunc + "_S0")
							&& !parser->GetSymbol(onStateFunc + "_S1")
							&& !parser->GetSymbol(onStateFunc + "_S2")
							&& !parser->GetSymbol(onStateFunc + "_S3")
							&& !parser->GetSymbol(onStateFunc + "_S4")
						)
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_ON_STATE_FUNC);
							entry->SetObject((uint)vob);
							entry->SetVobName(onStateFunc);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}
				}

				if (auto pMobCommon = vob->CastTo<oCMOB>())
				{
					auto focusName = pMobCommon->name;

					if (!focusName.IsEmpty())
					{
						if (!parser->GetSymbol(focusName))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_FOCUS_NAME);
							entry->SetObject((uint)vob);
							entry->SetVobName(focusName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}
				}

				if (auto pLock = vob->CastTo<oCMobLockable>())
				{
					auto keyName = pLock->keyInstance;

					if (keyName.Length() > 0)
					{
						bool flagNotItem = false;
						auto itemSymbol = parser->GetSymbol(keyName);

						if (itemSymbol)
						{
							int base = parser->GetBase(parser->GetIndex(keyName));
							int item = parser->GetIndex(zSTRING("C_Item"));

							if (base != item)
							{
								flagNotItem = true;
							}
						}

						if (!itemSymbol || flagNotItem)
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_KEY_INSTANCE);
							entry->SetObject((uint)vob);
							entry->SetVobName(keyName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}

					
				}



				if (auto pSound = vob->CastTo<zCVobSound>())
				{
					if (pSound->soundName.IsEmpty())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_SOUNDNAME);
						entry->SetObject((uint)vob);
						entry->SetVobName(pSound->GetVobName());
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
					else
					{
						if (!parserSoundFX->GetSymbol(pSound->soundName))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_SOUNDNAME_NOINST);
							entry->SetObject((uint)vob);
							entry->SetVobName(pSound->soundName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}
				}

				if (auto pMob = vob->CastTo<oCMobBed>())
				{
					if (pMob->focusNameIndex == 0 || pMob->focusNameIndex == -1 || pMob->name.IsEmpty())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_INFO);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_EMPTY_NAME_MOB_FOCUS);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->_GetClassDef()->className);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (vob->GetVobName().contains(' ') && !vob->GetVobName().contains("SPELLFX"))
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

				if (vob->GetVobName().Length() > 0)
				{
					Stack_PushString(vob->GetVobName());

					static auto callFunc = (intFuncPointer)GetProcAddress(theApp.module, "Utils_IsOnlyLatin");

					if (callFunc() == FALSE)
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_NAME_SYMBOLS);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->GetVobName());
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}


				

				if (auto fp = vob->CastTo<zCVobSpot>())
				{
					auto fpName = fp->GetVobName();

					if (!fpNames.IsInList(fpName))
					{
						fpNames.InsertEnd(fpName);
					}
					else
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_NOT_UNIQ_NAME);
						entry->SetObject((uint)vob);
						entry->SetVobName(fpName);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}
				else if (auto wp = vob->CastTo<zCVobWaypoint>())
				{
					auto wpName = wp->GetVobName();

					if (!wpNames.IsInList(wpName))
					{
						wpNames.InsertEnd(wpName);
					}
					else
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_NOT_UNIQ_NAME);
						entry->SetObject((uint)vob);
						entry->SetVobName(wpName);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}
				else if (auto mobCont = vob->CastTo<oCMobContainer>())
				{
					auto mobName = mobCont->GetVobName();

					if (mobName.Length() > 0)
					{
						if (!containerNames.IsInList(mobName))
						{
							containerNames.InsertEnd(mobName);
						}
						else
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_NOT_UNIQ_NAME);
							entry->SetObject((uint)vob);
							entry->SetVobName(mobName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
					}

					

					auto contains = mobCont->contains;

					if (contains.Length() > 0)
					{
						if (contains.HasWord(" "))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_SPACES_CONTAINER);
							entry->SetObject((uint)vob);
							entry->SetVobName(mobName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}


						if (contains.HasWord(",,"))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINER_BAD_SYMBOLS);
							entry->SetObject((uint)vob);
							entry->SetVobName(",,");
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
						else if (contains.HasWord(";"))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINER_BAD_SYMBOLS);
							entry->SetObject((uint)vob);
							entry->SetVobName(";");
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
						else if (contains.HasWord("."))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINER_BAD_SYMBOLS);
							entry->SetObject((uint)vob);
							entry->SetVobName(".");
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}


						if (!contains.HasWord(','))
						{
							if (!contains.HasWord(':'))
							{
								if (!IsItemExistsInScript(contains))
								{
									auto entry = new ErrorReportEntry();

									entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
									entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINTER_ITEM);
									entry->SetObject((uint)vob);
									entry->SetVobName(contains);
									entry->SetMaterialName("");
									entry->SetTextureName("");

									AddEntry(entry);
								}
							}
							else
							{
								auto innerSplit = Split(contains, ':');

								if (innerSplit.GetNum() == 2)
								{
									auto word = innerSplit[0];

									//cmd << "Check item: '" << word << "'" << endl;

									if (!IsItemExistsInScript(word))
									{
										//cmd << "\tNOT FOUND" << endl;

										auto entry = new ErrorReportEntry();

										entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
										entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINTER_ITEM);
										entry->SetObject((uint)vob);
										entry->SetVobName(word);
										entry->SetMaterialName("");
										entry->SetTextureName("");

										AddEntry(entry);
									}
								}
							}
							
						}
						else
						{
							auto split = Split(contains, ',');
							
							for (int j = 0; j < split.GetNumInList(); j++)
							{
								auto str = split[j];

								str.TrimLeft(' ');
								str.TrimRight(' ');

								if (str.HasWord(':'))
								{
									auto innerSplit = Split(str, ':');

									if (innerSplit.GetNum() == 2)
									{
										auto word = innerSplit[0];

										if (!IsItemExistsInScript(word))
										{
											auto entry = new ErrorReportEntry();

											entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
											entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINTER_ITEM);
											entry->SetObject((uint)vob);
											entry->SetVobName(word);
											entry->SetMaterialName("");
											entry->SetTextureName("");

											AddEntry(entry);
										}
									}

								}
								else
								{
									if (!IsItemExistsInScript(str))
									{
										auto entry = new ErrorReportEntry();

										entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
										entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CONTAINTER_ITEM);
										entry->SetObject((uint)vob);
										entry->SetVobName(str);
										entry->SetMaterialName("");
										entry->SetTextureName("");

										AddEntry(entry);
									}
								}
							}					
						}
					}
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