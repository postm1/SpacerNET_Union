// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	
	void ErrorReport::CheckVobs(int autoFix)
	{
		zCArray<zCVob*> activeVobList;

		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, activeVobList, 0);

		int count = activeVobList.GetNumInList();


		zCArray<zSTRING> moversNames;
		zCArray<zSTRING> wpNames;
		zCArray<zSTRING> fpNames;
		zCArray<zSTRING> containerNames;

		zCWayNet* waynet = ogame->GetWorld()->wayNet;

		if (autoFix)
		{
			cmd << endl << "<===== AUTO FIX MOD ACTIVATED =====>" << endl;
		}

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

#if ENGINE >= Engine_G2

				else if (vob->m_fVobFarClipZScale < 0.0f || vob->m_fVobFarClipZScale >= 3.01f)
				{

					if (autoFix)
					{
						if (vob->m_fVobFarClipZScale > 3)
						{
							vob->m_fVobFarClipZScale = 2.0f;
							cmd << "Fixing vobFarClipZScale, set to 2.0: " << WHEX32((int)vob) << " " << vob->GetVobName() << endl;
						}
						else
						{
							vob->m_fVobFarClipZScale = 1.0f;
							cmd << "Fixing vobFarClipZScale, set to 1.0: " << WHEX32((int)vob) << " " << vob->GetVobName() << endl;
						}
						
						
					}
					else
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_ZFARVOB);
						entry->SetObject((uint)vob);
						entry->SetVobName(vob->GetVobName());
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
					
				}
#endif //
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
					 
					// NOPE IT IS OK, ITEM CAN BE IN PARENT
					/*
					if (pItem->HasParentVob())
					{
						if (auto pParentVob =  pItem->GetParentVob())
						{
							auto pTrigger = pParentVob->CastTo<zCMover>();
							auto pCompo = pParentVob->CastTo<zCVobLevelCompo>();

							if (!pTrigger && !pCompo)
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_ITEM_PARENT);
								entry->SetObject((uint)vob);
								entry->SetVobName(pItem->GetInstanceName());
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
							
						}
					
					}
					*/
				}
				else if (vob->GetClassDef() == zCVob::classDef)
				{
					if (!vob->visual)
					{

						if (vob->GetVobName() == "VOB_SPACER_CAMERA_START")
						{
							continue;
						}

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

							if (vob->GetCollDetDyn())
							{

								if (autoFix)
								{
									vob->SetCollDetDyn(FALSE);
									cmd << "Fixing DynColl for PFX: " << WHEX32((int)vob) << " " << vob->GetVobName() << endl;
								}
								else
								{
									auto entry = new ErrorReportEntry();

									entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
									entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_PFX_CANT_HAVE_DYNCOLL);
									entry->SetObject((uint)vob);
									entry->SetVobName(vob->visual->GetVisualName());
									entry->SetMaterialName("");
									entry->SetTextureName("");

									AddEntry(entry);
								}
								
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
							
							if (autoFix && !vob->CastTo<zCTrigger>())
							{
								cmd << "Fixing name == visual: " << WHEX32((int)vob) << " " << vob->GetVobName() << endl;
								vob->SetObjectName("");
							}
							else
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

					if (!pItem && visName.Length() > 0)
					{

						int lastIndexOf = visName.SearchReverse(".");
						int length = visName.Length();

						//cmd << "Visual: " << visName << " Index: " << lastIndexOf << " Length: " << length << endl;


						if (visName.Contains(" "))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_VISUAL_SPACE);
							entry->SetObject((uint)vob);
							entry->SetVobName(visName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
						else
						{
							Stack_PushString(visName);

							static auto callFunc = (intFuncPointer)GetProcAddress(theApp.module, "Utils_IsOnlyLatin");

							if (callFunc() == FALSE
								|| !visName.HasWord(".")
								|| (length - 1 - lastIndexOf) != 3
								|| visName.HasWord(";")
								|| visName.HasWord(",")
								|| visName.HasWord("!")
								|| visName.HasWord("=")
								)
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BAD_VISUAL_SYMBOLS);
								entry->SetObject((uint)vob);
								entry->SetVobName(visName);
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
						}

						if ((visName.Upper().Contains(".TGA")))
						{
							if (vob->GetCollDetDyn())
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_TGA_DYNCOLL);
								entry->SetObject((uint)vob);
								entry->SetVobName(vob->visual->GetVisualName());
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
							

							/*if (auto pVisual = vob->GetVisual())
							{
								if (auto pDecal = pVisual->CastTo<zCDecal>())
								{
									if (pDecal->decalMaterial)
									{
										if (pDecal->decalMaterial->GetAlphaBlendFunc() == zRND_ALPHA_FUNC_NONE)
										{
											auto entry = new ErrorReportEntry();

											entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
											entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_DECAL_BAD_BLEND);
											entry->SetObject((uint)vob);
											entry->SetVobName(GetVobName(vob));
											entry->SetMaterialName("");
											entry->SetTextureName("");

											AddEntry(entry);
										}
									}
								}
							}*/
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


				if (vob->_GetClassDef() == oCZoneMusic::classDef 
					|| vob->_GetClassDef() == zCVobSound::classDef 
					|| vob->_GetClassDef() == zCZoneZFog::classDef)
				{
					if (vob->bbox3D.GetMinExtent() < 1)
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_BBOX_AREA_WRONG_SIZE);
						entry->SetObject((uint)vob);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (auto zTriggerBase = vob->CastTo<zCTriggerBase>())
				{
					auto triggerTarget = zTriggerBase->triggerTarget;

					if (triggerTarget.Length() > 0)
					{
						if (!FindVobByName(triggerTarget))
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

					if (pMob->pickLockStr.Length() > 0)
					{
						std::string strCheck = pMob->pickLockStr.Upper();

						if (ContainsSymbolsOtherThanRL(strCheck))
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MOBCONT_BAD_LOCKSTRING);
							entry->SetObject((uint)vob);
							entry->SetVobName(vob->_GetClassDef()->className);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
						
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
						if (!FindVobByName(triggerName))
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

						if (theApp.isNewBalanceMod && onStateFunc == "RX_AUTOCHEST")
						{
							continue;
						}

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

					if (!pMobCommon->collDetectionDynamic && pMobCommon->showVisual && !pMobCommon->CastTo<oCMobSwitch>())
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MOB_NO_COLL);
						entry->SetObject((uint)vob);
						entry->SetVobName(GetVobNameSafe(pMobCommon));
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
					}
				}

				if (auto pLock = vob->CastTo<oCMobLockable>())
				{
					auto keyName = pLock->keyInstance;

					if (keyName.Length() > 0)
					{
						if (!IsItemExistsInScript(keyName))
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

				

				if (vob->GetVobName().Length() > 0)
				{

					if (vob->GetVobName().Contains(' ') && !vob->GetVobName().Contains("SPELLFX"))
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
					else
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
					
				}

				if (auto pTrigScript = vob->CastTo<oCTriggerScript>())
				{
					auto funcName = pTrigScript->scriptFunc;

					if (funcName.Length() > 0 && parser->GetIndex(funcName) == -1)
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_TRIGGER_SCRIPT_FUNC);
						entry->SetObject((uint)vob);
						entry->SetVobName(funcName);
						entry->SetMaterialName("");
						entry->SetTextureName("");

						AddEntry(entry);
						
					}
				}

				if (auto pTriggerLevelChange = vob->CastTo<oCTriggerChangeLevel>())
				{
					if (pTriggerLevelChange->levelName.Contains(" "))
					{
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_CHANGELEVEL_SPACE);
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

					if (waynet)
					{
						zCWaypoint* wpNet = waynet->SearchWaypoint(wp);

						if (!wpNet)
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_WP_NOT_IN_WAYNET);
							entry->SetObject((uint)vob);
							entry->SetVobName(wpName);
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
						else
						{
							if (wpNet->GetNumberOfWays() == 0 && wp->GetVobName() != "TOT")
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_WP_NO_CONNECTIONS);
								entry->SetObject((uint)vob);
								entry->SetVobName(wpName);
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
						}
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


		// find close light sources (double)
		ogame->GetWorld()->SearchVobListByClass(zCVobLight::classDef, resultList, 0);
		{
			float threshHoldDouble = 8.5f;
			std::unordered_set<int> setFoundIndexes;

			setFoundIndexes.reserve(100);

			//cmd << "zCVobLight: " << resultList.GetNumInList() << endl;

			if (resultList.GetNumInList() > 1)
			{
				for (int i = 0; i < resultList.GetNumInList(); i++)
				{
					if (auto pVob = resultList.GetSafe(i))
					{
						for (int j = 0; j < resultList.GetNumInList(); j++)
						{
							if (auto pVob2 = resultList.GetSafe(j))
							{
								if (i != j)
								{
									if (setFoundIndexes.find(j) == setFoundIndexes.end())
									{
										//cmd << "setFoundIndexes: " << j << endl;

										if (pVob->GetPositionWorld().Distance(pVob2->GetPositionWorld()) <= threshHoldDouble)
										{
											//cmd << "Insert: " << j << endl;
											setFoundIndexes.insert(j);

											auto entry = new ErrorReportEntry();

											entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
											entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_LIGHT_CLOSE_TOGETHER);
											entry->SetObject((uint)pVob2);
											entry->SetVobName("");
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

			auto bsp = ogame->GetGameWorld()->GetBspTree();
			bool searchBadLightNoPortals = bsp && bsp->bspTreeMode == zBSP_MODE_OUTDOOR;

			for (int i = 0; i < resultList.GetNumInList(); i++)
			{
				if (auto pVob = resultList.GetSafe(i))
				{
					if (auto pLight = pVob->CastTo<zCVobLight>())
					{
						if (pLight->GetCollDetDyn())
						{
							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_LIGHT_DYNCOLL);
							entry->SetObject((uint)pLight);
							entry->SetVobName("");
							entry->SetMaterialName("");
							entry->SetTextureName("");

							AddEntry(entry);
						}
						

						if (searchBadLightNoPortals && pLight->lightData.isStatic)
						{
							zSTRING* name = (zSTRING*)bsp->GetSectorNameVobIsIn(pLight);

							if (!name)
							{
								auto entry = new ErrorReportEntry();

								entry->SetErrorType(ERROR_REPORT_TYPE_WARNING);
								entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_STATIC_LIGHT_NO_PORTAL);
								entry->SetObject((uint)pLight);
								entry->SetVobName("");
								entry->SetMaterialName("");
								entry->SetTextureName("");

								AddEntry(entry);
							}
						}
					}
				}
			}
			
			
		}

		if (autoFix)
		{
			cmd << endl << "<AUTO FIX MOD END>" << endl;
			print.PrintRed(GetLang("REPORT_ALL_CONSOLE").ToChar());
		}
	}
}