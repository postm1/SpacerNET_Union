// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	typedef void(*createTriggerForm)(int, int, int, int);
	typedef void(*addTargetListToForm)(int);
	typedef void(*selectMoversTab)();
	typedef void(*addActionTypeMover)(char*);


	class zCVobCallback_CollectEventList : public zCVobCallback
	{
		zCVob*					vob;
		zCList<zCVob>*			sources;
	public:

		zCVobCallback_CollectEventList() { sources = 0; };
		virtual ~zCVobCallback_CollectEventList() { sources = 0; };

		void Init(zCList<zCVob>* _sources, zCVob* _vob)
		{
			vob = _vob;
			sources = _sources;
		};


		virtual void HandleVob(zCVob* checkVob, void *callbackData)
		{
			if (!vob) return;


			zSTRING triggerTarget = "";
			if (checkVob->GetTriggerTarget(0)) triggerTarget = checkVob->GetTriggerTarget(0)->ToChar();

			if (sources && !triggerTarget.IsEmpty() && triggerTarget == vob->GetVobName())
			{
				checkVob->AddRef();
				sources->Insert(checkVob);
			};
		};
	};

	void SpacerApp::TriggerLoop()
	{
		if (!moverVob) return;


		m_kf_pos = moverVob->GetActKeyframe();
		
		auto callFunc = (callIntInt)GetProcAddress(theApp.module, "Trigger_UpdateKeys");
		callFunc(m_kf_pos, moverVob->keyframeList.GetNumInList());
	}

	void SpacerApp::SetToKeyPos()
	{
		if (!moverVob) return;
		zREAL position((float)m_kf_pos);
		if (
			moverVob->keyframeList.GetNumInList()>0 &&
			m_kf_pos<moverVob->keyframeList.GetNumInList()
			)
		{
			moverVob->SetToKeyframe(position, 0);
		}
	}


	void SpacerApp::OnKeyRemove()
	{
		if (moverVob->keyframeList.GetNumInList() == 0) return;

		moverVob->keyframeList.RemoveOrderIndex(m_kf_pos);

		if (m_kf_pos>0)
		{
			m_kf_pos--;
			SetToKeyPos();
		}

		//if (moverVob->keyframeList.GetNumInList() == 0)
		//ctrl_kf_edit.EnableWindow(false);
	}

	void SpacerApp::SetTriggerToKey(int actionIndex)
	{
		if (!pickedVob) return;

		zCMover* pMover = dynamic_cast<zCMover*>(pickedVob);

		if (pMover)
		{
			zREAL position((float)actionIndex);
			if (
				pMover->keyframeList.GetNumInList()>0 &&
				actionIndex<pMover->keyframeList.GetNumInList()
				)
			{
				pMover->SetToKeyframe(position, 0);
				m_kf_pos = position;
			}
		}
	}

	void SpacerApp::SendTrigger(int actionIndex)
	{
		if (!event_sourcevob)
		{
			event_sourcevob = new zCVob;
			event_sourcevob->SetVobName("Event-TestVob");
			event_sourcevob->SetCollDet(TRUE);
			event_sourcevob->SetPhysicsEnabled(TRUE);
		};

		if (!pickedVob) return;

		int index = actionIndex;
		if (index<0) return;

		zCEventCore* eventCore = new zCEventCore((zCEventCore::zTEventCoreSubType)index, event_sourcevob, event_sourcevob, 10000.0f, 0, zVEC3(0));

		pickedVob->GetEventManager()->OnMessage(eventCore, event_sourcevob);

		zCMover* mov = dynamic_cast<zCMover*>(pickedVob);
		if (mov)
		{
			m_kf_pos = mov->GetActKeyframe();
		}
	}

	void SpacerApp::OnAddMoverKey(int mode)
	{
		if (moverVob)
		{
			zCMover::zTMov_Keyframe KF = zCMover::GetKeyframe(moverVob);
			int numInList = moverVob->keyframeList.GetNumInList();

			//std::cout << "Union: OnAddMoverKey, mode is " << mode << std::endl;

			if (numInList == 0)
			{
				moverVob->keyframeList.Insert(KF);
			}
			else
			{
				switch (mode)
				{
				case 0: // Insert After
					if (m_kf_pos + 1 == numInList)
						moverVob->keyframeList.Insert(KF);
					else
						moverVob->keyframeList.InsertAtPos(KF, m_kf_pos + 1);
					m_kf_pos++;
					break;
				case 1: // Insert Before
					moverVob->keyframeList.InsertAtPos(KF, m_kf_pos);
					break;
				case 2: // Overwrite			
					moverVob->keyframeList[m_kf_pos] = KF;
					break;
				};
			}
		}
	}


	void SpacerApp::SetCurrentKey(int key)
	{
		m_kf_pos = key;
	}

	void SpacerApp::SetMover()
	{

		CollectTargetListTrigger();

		if (options.GetIntVal("selectVobTab") && (dynamic_cast<zCTriggerBase*>(pickedVob) || dynamic_cast<zCPFXControler*>(pickedVob)))
		{
			(selectMoversTab)GetProcAddress(theApp.module, "SelectMoversTab")();
		}

		if (moverVob)
		{

			m_kf_pos = moverVob->GetActKeyframe();
			auto createTrigger = (createTriggerForm)GetProcAddress(theApp.module, "CreateTriggerForm");

			Stack_PushString(GetVobName(moverVob));
			createTrigger(m_kf_pos, moverVob->keyframeList.GetNumInList(), moverVob->GetCollDetDyn(), moverVob->GetCollDetStat());
		}
		else if (triggerVob)
		{
			auto createTrigger2 = (callVoidFunc)GetProcAddress(theApp.module, "CreateTriggerFormEmpty");


			Stack_PushString(GetVobName(triggerVob));
			createTrigger2();
			
		}
		else if (pickedVob->CastTo<oCMobInter>() || pickedVob->CastTo<zCTriggerUntouch>() || pickedVob->CastTo<zCPFXControler>()
			||  pickedVob->CastTo<zCMessageFilter>())
		{
			auto createTrigger2 = (callVoidFunc)GetProcAddress(theApp.module, "CreateTriggerFormEmpty");


			Stack_PushString(GetVobName(triggerVob));
			createTrigger2();
		}
		else
		{
			auto cleanTriggerForm = (voidFuncPointer)GetProcAddress(theApp.module, "CleanTriggerForm");
			cleanTriggerForm();
			return;
		}


		auto addActionType = (callVoidFunc)GetProcAddress(theApp.module, "AddActionTypeMover");


		zCEventCore* eventCore = new zCEventCore((zCEventCore::zTEventCoreSubType)0);
		int num_types = eventCore->MD_GetNumOfSubTypes();
		zSTRING s;
		for (int i = 0; i<num_types; i++)
		{
			s = eventCore->MD_GetSubTypeString(i);
			Stack_PushString(s);
			addActionType();
		}

		zRELEASE(eventCore);



		auto addTargetToList = (addTargetListToForm)GetProcAddress(theApp.module, "AddTargetToList");

		//std::cout << "Union: targetVobList count: " << targetVobList.GetNumInList() << std::endl;


		if (targetVobList.GetNumInList() > 0)
		{
			for (int i = targetVobList.GetNumInList() - 1; i >= 0; i--)
			{
				Stack_PushString(GetVobName(targetVobList[i]));
				addTargetToList((uint)targetVobList[i]);
			}
		}







	}

	void SpacerApp::RemoveTargetListTrigger()
	{
		zCVob* vob = 0;



		for (int i = 0; i<sourceVobList.GetNumInList(); i++)
		{
			vob = sourceVobList.Get(i);

			if (vob)
			{
				//std::cout << "sourceVobList: " << vob->GetVobName() << " " << vob->refCtr << std::endl;
				zRELEASE(vob);

			}

		};
		sourceVobList.DeleteList();



		for (int i = 0; i<targetVobList.GetNumInList(); i++)
		{
			vob = targetVobList.Get(i);

			if (vob)
			{
				//std::cout << "targetVobList: " << vob->GetVobName() << " " << vob->refCtr << std::endl;
				zRELEASE(vob);

			}

		};

		targetVobList.DeleteList();

	}






	void SpacerApp::CollectTargetListTrigger()
	{

		//std::cout << "Union: CollectTargetListTrigger " << std::endl;

		RemoveTargetListTrigger();

		if (!pickedVob)
		{
			return;
		}

		zCWorld* world = ogame->GetWorld();
		zCVob* vob = NULL;

		if (pickedVob->GetTriggerTarget(0))
		{
			zCArray<zCVob*> targetList;

			int ttz = 0;
			while (pickedVob->GetTriggerTarget(ttz))
			{
				if (!pickedVob->GetTriggerTarget(ttz)->IsEmpty())
				{
					targetList.EmptyList();
					world->SearchVobListByName(*(pickedVob->GetTriggerTarget(ttz)), targetList);
					for (int i = 0; i<targetList.GetNumInList(); i++)
					{
						vob = targetList.GetSafe(i);
						if (vob) { vob->AddRef(); targetVobList.Insert(vob); }
					}
				}
				ttz++;
			}
		}




		if (targetVobList.GetNumInList() > 0)
		{
			for (int i = targetVobList.GetNumInList() - 1; i >= 0; i--)
			{
				zCVob* pVob = targetVobList.Get(i);

				if (pVob)
				{
					zSTRING triggerTarget = "";
					if (pVob->GetTriggerTarget(0)) triggerTarget = pVob->GetTriggerTarget(0)->ToChar();

					if (!triggerTarget.IsEmpty() && triggerTarget == vob->GetVobName())
					{
						pVob->AddRef();
						sourceVobList.Insert(pVob);
					};
				}

			}
		}


	}


	void SpacerApp::CollectSourcerTriggerList()
	{
		if (!pickedVob)
		{
			return;
		}


		zCVob* vob = 0;

		for (int i = 0; i<sourceVobList.GetNumInList(); i++)
		{
			vob = sourceVobList.Get(i);

			if (vob)
			{
				zRELEASE(vob);
			}

		};

		sourceVobList.DeleteList();

		zCVobCallback_CollectEventList vobCallback;



		vobCallback.Init(&sourceVobList, pickedVob);
		ogame->GetWorld()->TraverseVobTree(vobCallback, pickedVob, 0);

		auto addSourcesToList = (addTargetListToForm)GetProcAddress(theApp.module, "AddSourcesToList");

		if (sourceVobList.GetNumInList() > 0)
		{
			for (int i = sourceVobList.GetNumInList() - 1; i >= 0; i--)
			{
				Stack_PushString(GetVobName(sourceVobList[i]));
				addSourcesToList((uint)sourceVobList[i]);
			}
		}
	}

}