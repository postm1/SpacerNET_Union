// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	typedef void(*sendKey)(int);

	// ESC key crashfix
	int __fastcall Ivk_CGameManager_HandleEvent(CGameManager*, void*, int);
	CInvoke <int(__thiscall *) (CGameManager*, int)> pIvk_CGameManager_HandleEvent(0x0042AD50, Ivk_CGameManager_HandleEvent, IVK_AUTO);
	int __fastcall Ivk_CGameManager_HandleEvent(CGameManager* _this, void*, int key)
	{
		static auto keySender = (sendKey)GetProcAddress(theApp.module, "AcceptKey");

		//keys.SetKeyString(key);

		keySender(key);

		if (key == KEY_ESCAPE)
			return FALSE;

		return pIvk_CGameManager_HandleEvent(_this, key);
	}

	// CS Camera EV_PLAY crashfix
	int __fastcall Ivk_CheckUnderWaterFX(zCAICamera*, void*);
	CInvoke <int(__thiscall *) (zCAICamera*)> pInvk_CheckUnderWaterFX(0x004BD2E0, Ivk_CheckUnderWaterFX, IVK_AUTO);
	int __fastcall Ivk_CheckUnderWaterFX(zCAICamera*, void*)
	{
		return FALSE;
	}

	// Def vobname Fix
	HOOK Ivk_zCVob_SetVobPresetName   AS(0x005FE940, &zCVob::SetVobPresetName_Hook);
	void zCVob::SetVobPresetName_Hook(class zSTRING const &presetName)
	{
		if (!vobPresetName) vobPresetName = zNEW(zSTRING(presetName));
		else (*vobPresetName) = presetName;
	}

	// Start screen fix
	void __fastcall Invk_InitScreen_Open(CGameManager*, void*);
	CInvoke <void(__thiscall *) (CGameManager*)> pInvk_InitScreen_Open(0x00425F60, Invk_InitScreen_Open, IVK_AUTO);
	void __fastcall Invk_InitScreen_Open(CGameManager*, void*)
	{
		return;
	}

	// VOB visual fix
	void __fastcall Invk_zCParticleFX(zCParticleFX* _this, void*);
	CInvoke<void(__thiscall*)(zCParticleFX*)> pInvk_zCParticleFX(0x005ACF70, Invk_zCParticleFX);
	void __fastcall Invk_zCParticleFX(zCParticleFX* _this, void*)
	{
		pInvk_zCParticleFX(_this);
		_this->dontKillPFXWhenDone = TRUE;
	}

	// PFX never die FIX
	void __fastcall Invk_zCVobArchive(zCVob* _this, void*, class zCArchiver &);
	CInvoke<void(__thiscall*)(zCVob* _this, class zCArchiver &)> pzCVobArchive(0x005FF140, Invk_zCVobArchive);
	void __fastcall Invk_zCVobArchive(zCVob* _this, void*, class zCArchiver & arc)
	{
		if (zDYNAMIC_CAST<zCParticleFX>(_this->GetVisual()))
			zDYNAMIC_CAST<zCParticleFX>(_this->GetVisual())->m_bVisualNeverDies = TRUE;
		pzCVobArchive(_this, arc);
	}

	// Save world Waynet FIX
	int __fastcall Invk_oCWorld_SaveWorld(oCWorld* _this, void*, class zSTRING const & fileName, enum zCWorld::zTWorldSaveMode saveMode, int writeBinary, int _saveLevelMesh);
	CInvoke<int(__thiscall*)(oCWorld*, class zSTRING const &, enum zCWorld::zTWorldSaveMode, int, int)> poCWorldSave(0x0077FA50, Invk_oCWorld_SaveWorld);
	int __fastcall Invk_oCWorld_SaveWorld(oCWorld* _this, void*, class zSTRING const & fileName, enum zCWorld::zTWorldSaveMode saveMode, int writeBinary, int _saveLevelMesh)
	{
		if (_this->GetWayNet())
		{
			if (_this->GetWayNet()) _this->GetWayNet()->UpdateVobDependencies();
			_this->GetWayNet()->ClearVobDependencies();
		}

		int result = poCWorldSave(_this, fileName, saveMode, writeBinary, _saveLevelMesh);


		if (_this->GetWayNet()) _this->GetWayNet()->CreateVobDependencies(_this);

		return result;
	}

	// ‘икс вейнет
	//0x007B2200 public: virtual void __thiscall zCWayNet::Unarchive(class zCArchiver &)
	void __fastcall zCWayNet_Unarchive(zCWayNet* _this, void*, class zCArchiver &);
	CInvoke <void(__thiscall*) (zCWayNet* _this, class zCArchiver & arc)> pzCWayNet_Unarchive(0x007B2200, zCWayNet_Unarchive, IVK_AUTO);
	void __fastcall zCWayNet_Unarchive(zCWayNet* _this, void*, class zCArchiver & arc)
	{
		pzCWayNet_Unarchive(_this, arc);

		_this->CreateVobDependencies(_this->world);
	}


	void __fastcall Invk_InitScreen_Close(CGameManager*, void*);
	CInvoke <void(__thiscall *) (CGameManager*)> pInvk_InitScreen_Close(0x00426330, Invk_InitScreen_Close, IVK_AUTO);
	void __fastcall Invk_InitScreen_Close(CGameManager*, void*)
	{
		return;
	}


	//0x00582800 public: virtual int __thiscall zCModel::TraceRay(class zVEC3 const &,class zVEC3 const &,int,struct zTTraceRayReport &)
	int __fastcall Invk_TraceRay(zCModel* _this, void*, class zVEC3 const &, class zVEC3 const &, int, struct zTTraceRayReport &);
	CInvoke<int(__thiscall*)(zCModel* _this, class zVEC3 const &, class zVEC3 const &, int, struct zTTraceRayReport &)> pInvk_TraceRay(0x00582800, Invk_TraceRay);
	int __fastcall Invk_TraceRay(zCModel* _this, void*, class zVEC3 const & rayOrigin, class zVEC3 const & ray, int traceFlags, struct zTTraceRayReport &report)
	{
		if (_this->meshSoftSkinList.GetNum()>0)
		{
			report.Clear();
			report.foundHit = TRUE;
			return TRUE;
		};

		return pInvk_TraceRay(_this, rayOrigin, ray, traceFlags, report);
	}



	static int& s_iCurrentCollReport = *(int*)0x008D82EC;
	static zCCollisionReport	s_aoCollReportList[64];

	//0x0054C7D0 public: static class zCCollisionReport * __cdecl zCCollisionDetector::S_GetNextFreeCollReport(void)
	zCCollisionReport* __cdecl Invk_S_GetNextFreeCollReport(zCCollisionDetector*);
	CInvoke <zCCollisionReport*(__cdecl *) (zCCollisionDetector*)> pInvk_S_GetNextFreeCollReport(0x0054C7D0, Invk_S_GetNextFreeCollReport, IVK_AUTO);
	zCCollisionReport* __cdecl Invk_S_GetNextFreeCollReport(zCCollisionDetector* _this)
	{
		if (s_iCurrentCollReport >= 64)
		{
			//zERR_WARNING("D: CD: generic zCCollisionReport pool too small!");
			s_iCurrentCollReport = 0;
		};
		return &(s_aoCollReportList[s_iCurrentCollReport++]);
	}



	//0x005FFC70 protected: virtual void __thiscall zCVob::Unarchive(class zCArchiver &)
	void __fastcall Invk_zCVobUnarchive(zCVob* _this, void*, class zCArchiver &);
	CInvoke<void(__thiscall*)(zCVob* _this, class zCArchiver &)> pInvk_zCVobUnarchive(0x005FFC70, Invk_zCVobUnarchive);
	void __fastcall Invk_zCVobUnarchive(zCVob* _this, void*, class zCArchiver & arc)
	{
		_this->zCObject::Unarchive(arc);

		// einen sauberen Zustand herstellen
		_this->ResetOnTimer();

		// Packing DWord Trailer lesen und auswerten
		{
			int	pack = 0;	// 0 WICHTIG!
			arc.ReadInt("pack", pack);
			if (pack == 0)			_this->UnarchiveVerbose(arc);
			else					_this->UnarchivePacked(arc, pack);
		};

		// SaveGame
		if (arc.InSaveGame())
		{
			_this->SetSleepingMode(zTVobSleepingMode(arc.ReadByte("sleepMode")));
			// nextOnTimer: ist absolut, muss aber relativ gespeichert werden
			zREAL nextOnTimerRel = _this->nextOnTimer;
			arc.ReadFloat("nextOnTimer", nextOnTimerRel);
			_this->nextOnTimer = (nextOnTimerRel >= zREAL_MAX) ? nextOnTimerRel : (ztimer->totalTimeFloat + nextOnTimerRel);
			if (_this->GetPhysicsEnabled()) {
				_this->GetRigidBody()->Unarchive(arc);
			};
		};

		if (!arc.InProperties())
		{
			_this->RepairIllegalFloats(_this->trafoObjToWorld);
			if (!_this->trafoObjToWorld.IsUpper3x3Orthonormal())
				_this->trafoObjToWorld.MakeOrthonormal();
			_this->CorrectTrafo();
		};

		//
		_this->UpdateVisualDependencies(TRUE);

		const zREAL VOB_IGNORE_MIN_EXTEND = 10;
		zREAL minExtend = _this->GetBBox3DWorld().GetMinExtent();
		if (minExtend > VOB_IGNORE_MIN_EXTEND) _this->m_zBias = 0;


		RecalcWPBBox(_this);
	}

	//убивает определенные триггеры, которые запускаютс€ автоматом, иначе в игре они будут не в нужном состо€нии
	void __fastcall Invk_PostLoad(zCTriggerWorldStart*, void*);
	CInvoke <void(__thiscall *) (zCTriggerWorldStart*)> pInvk_PostLoad(0x0061A4E0, Invk_PostLoad, IVK_AUTO);
	void __fastcall Invk_PostLoad(zCTriggerWorldStart*, void*)
	{

	}

	//0x006C2630 public: virtual void __thiscall oCGame::CloseSavescreen(void)
	void __fastcall Invk_CloseSavescreen(oCGame*, void*);
	CInvoke <void(__thiscall *) (oCGame*)> pInvk_CloseSavescreen(0x006C2630, Invk_CloseSavescreen, IVK_AUTO);
	void __fastcall Invk_CloseSavescreen(oCGame*, void*)
	{

	}

	//0x006C2250 public: virtual void __thiscall oCGame::OpenSavescreen(bool)
	void __fastcall Invk_OpenSavescreen(oCGame*, void*, bool);
	CInvoke <void(__thiscall *) (oCGame*, bool)> pInvk_OpenSavescreen(0x006C2250, Invk_OpenSavescreen, IVK_AUTO);
	void __fastcall Invk_OpenSavescreen(oCGame*, void*, bool)
	{

	}




	// ’ук на камеру, чтобы она летала по кадрам в инструменте работы с камерой
	//0x004BFBC0 public: virtual void __thiscall zCCSCamera::Unarchive(class zCArchiver &)
	void __fastcall Invk_zCCSCamera_Unarchive(zCCSCamera* _this, void*, class zCArchiver &);
	CInvoke <void(__thiscall*) (zCCSCamera* _this, class zCArchiver &)> pInvk_zCCSCamera_Unarchive(0x004BFBC0, Invk_zCCSCamera_Unarchive, IVK_AUTO);
	void __fastcall Invk_zCCSCamera_Unarchive(zCCSCamera* _this, void*, class zCArchiver & arch)
	{
		pInvk_zCCSCamera_Unarchive(_this, arch);

		if (!arch.InProperties())
		{
			_this->SetSleeping(FALSE);
		}
	}


	//”бивает внутриигровое меню
	void __fastcall Invk_InitScreen_Menu(CGameManager* _this, void*);
	CInvoke <void(__thiscall*) (CGameManager* _this)> pInvk_InitScreen_Menu(0x00426210, Invk_InitScreen_Menu, IVK_AUTO);
	void __fastcall Invk_InitScreen_Menu(CGameManager* _this, void*)
	{
		return;
	}


	//ѕроверка на инициализацию парсера дл€ oCVisualFX, в спейсере именной такой код
	void __cdecl InitParser();
	CInvoke <void(__cdecl*) ()> pInitParser(0x0048B3F0, InitParser, IVK_AUTO);
	void __cdecl InitParser()
	{
		if (oCVisualFX::fxParser != NULL)
		{
			return;
		}

		pInitParser();
	}




	// ”бивает запуск контроллера PFX в спейсере
	//0x006148B0 public: virtual void __thiscall zCPFXControler::PostLoad(void)
	void __fastcall zCPFXControler_PostLoad(zCPFXControler* _this, void*);
	CInvoke <void(__thiscall*) (zCPFXControler* _this)> pzCPFXControler_PostLoad(0x006148B0, zCPFXControler_PostLoad, IVK_AUTO);
	void __fastcall zCPFXControler_PostLoad(zCPFXControler* _this, void*)
	{
		return;
	}
	//0x00614960 public: virtual void __thiscall zCPFXControler::OnUntrigger(class zCVob *,class zCVob *)
	void __fastcall zCPFXControler_OnUntrigger(zCPFXControler* _this, void*, class zCVob *, class zCVob *);
	CInvoke <void(__thiscall*) (zCPFXControler* _this, class zCVob *, class zCVob *)> pzCPFXControler_OnUntrigger(0x00614960, zCPFXControler_OnUntrigger, IVK_AUTO);
	void __fastcall zCPFXControler_OnUntrigger(zCPFXControler* _this, void*, class zCVob* otherVob, class zCVob* vobInstigator)
	{
		zCParticleFX *pfx = dynamic_cast<zCParticleFX*>(_this->GetVisual());
		if (!pfx) return;

		pfx->StopEmitterOutput();
		// если убрать визуал, то PPX-дочерние не будут показывать (динамеческие, которые добавл€ютс€ при активации PFX)
		//_this->SetVisual(0);
	}



	//¬ключает добавление зон в мир
	//0x0061FA40 public: __thiscall zCWorld::zCWorld(void)
	zCWorld* __fastcall zCWorld_Hooked(zCWorld* _this, void*);
	CInvoke <zCWorld* (__thiscall*) (zCWorld* _this)> pzCWorld_Hooked(0x0061FA40, zCWorld_Hooked, IVK_AUTO);
	zCWorld* __fastcall zCWorld_Hooked(zCWorld* _this, void*)
	{
		pzCWorld_Hooked(_this);
		_this->addZonesToWorld = TRUE;
		_this->SetWaveAnisEnabled(zoptions->ReadBool("SPACER", "zSpacerWaterAniEnabled", FALSE));
		return _this;
	}

	// ‘икс звука после сохранени€ звукового воба
	//0x0063E3D0 protected: virtual void __thiscall zCVobSound::Archive(class zCArchiver &)
	void __fastcall zCVobSound_Archive(zCVobSound* _this, void*, class zCArchiver &);
	CInvoke <void(__thiscall*) (zCVobSound* _this, class zCArchiver & arc)> pzCVobSound_Archive(0x0063E3D0, zCVobSound_Archive, IVK_AUTO);
	void __fastcall zCVobSound_Archive(zCVobSound* _this, void*, class zCArchiver & arc)
	{
		pzCVobSound_Archive(_this, arc);

		if (arc.InProperties())
		{
			_this->StopSound();
			_this->StartSound(TRUE);
		};
	}

	// ‘икс звука после загрузки звукового воба
	//0x0063E540 protected: virtual void __thiscall zCVobSound::Unarchive(class zCArchiver &)
	void __fastcall zCVobSound_Unarchive(zCVobSound* _this, void*, class zCArchiver &);
	CInvoke <void(__thiscall*) (zCVobSound* _this, class zCArchiver & arc)> pzCVobSound_Unarchive(0x0063E540, zCVobSound_Unarchive, IVK_AUTO);
	void __fastcall zCVobSound_Unarchive(zCVobSound* _this, void*, class zCArchiver & arc)
	{
		pzCVobSound_Unarchive(_this, arc);

		if (arc.InProperties())
		{
			_this->StopSound();
			_this->StartSound(TRUE);
		};
	}



	//0x00643F20 private: int __thiscall zCRnd_D3D::XD3D_InitPerDX(long,int,int,int,int)
	int __fastcall XD3D_InitPerDX(zCRnd_D3D*, void*, long, int, int, int, int);
	CInvoke <int(__thiscall *) (zCRnd_D3D*, long, int, int, int, int)> pXD3D_InitPerDX(0x00643F20, XD3D_InitPerDX, IVK_AUTO);
	int __fastcall XD3D_InitPerDX(zCRnd_D3D* _this, void*, long flags, int x, int y, int bpp, int id)
	{
		int result = pXD3D_InitPerDX(_this, 0, x, y, bpp, id);
		return result;
	}



	// ”бирает лишний код в обработке звука по зонам, нужно дл€ спейсера
	//0x0063E8D0 public: virtual void __thiscall zCVobSound::
	//ProcessZoneList(class zCArraySort<class zCZone *> const &,class zCArraySort<class zCZone *> const &,class zCWorld *)
	void __fastcall ProcessZoneList(zCVobSound* _this, void*, class zCArraySort<class zCZone *> const &, class zCArraySort<class zCZone *> const &, class zCWorld *);
	CInvoke <void(__thiscall*) (zCVobSound* _this, class zCArchiver & arc)> pProcessZoneList(0x0063E8D0, ProcessZoneList, IVK_AUTO);
	void __fastcall ProcessZoneList(zCVobSound* _this, void*, class zCArraySort<class zCZone *> const & zoneList, class zCArraySort<class zCZone *> const & zoneDeactivateList, class zCWorld * homeWorld)
	{
		for (int i = 0; i<zoneList.GetNum(); i++)
		{
			zCVobSound *vobSound = ((zCVobSound*)zoneList[i]);

			if (vobSound->soundAllowedToRun)
			{
				if (vobSound->soundAutoStart)
				{
					vobSound->soundAutoStart = FALSE;
					vobSound->StartSound(TRUE);
				};
				vobSound->DoSoundUpdate(1.0f);
			}
			else {
				if (vobSound->soundIsRunning)
					vobSound->StopSound();
			};
		};
	}


	//0x00531040 public: void __thiscall zCBspTree::AddVob(class zCVob *)
	void __fastcall AddVob(zCBspTree*, void*, class zCVob *);
	CInvoke <void(__thiscall *) (zCBspTree*, class zCVob *)> pAddVob(0x00531040, AddVob, IVK_AUTO);
	void __fastcall AddVob(zCBspTree* _this, void*, class zCVob * vob)
	{
		if (vob)
		{
			if (theApp.treeIsReady && !theApp.nextInsertBlocked)
			{
				if (vob->GetVobName() != "Vob_PFX_Editor")
				{
					OutFile("zCBspTree::AddVob: (" + AHEX32((uint)vob) + ") Parent: " + AHEX32(vob->GetParent()) + " NodesCount: " + A(ogame->GetWorld()->globalVobTree.CountNodes() - 1), false);
					theApp.OnCreateVob(vob, theApp.selectNextVob);
					theApp.selectNextVob = false;
				}

			}
		}

		pAddVob(_this, vob);
	}

	//0x004BE400 public: static class zCVob * __cdecl zCCSCamera::GetPlayerVob(void)
	zCVob * __cdecl zCCSCamera_GetPlayerVob(zCCSCamera*);
	CInvoke <zCVob *(__cdecl *) (zCCSCamera*)> pzCCSCamera_GetPlayerVob(0x004BE400, zCCSCamera_GetPlayerVob, IVK_AUTO);
	zCVob * __cdecl zCCSCamera_GetPlayerVob(zCCSCamera* _this)
	{
		return NULL;
	}

	//0x006015D0 public: virtual int __fastcall zCVob::Render(struct zTRenderContext &)
	int  __fastcall zCVob_Render(zCVob*, struct zTRenderContext &);
	//CInvoke <int (__fastcall *) (zCVob*, struct zTRenderContext &)> pzCVob_Render(0x006015D0, zCVob_Render, IVK_AUTO);
	int  __fastcall zCVob_Render(zCVob* _this, struct zTRenderContext & context)
	{

		return 0;
		//return pzCVob_Render(_this, context);
	}

	//0x0052D0A0 private: void __thiscall zCBspTree::RenderVobList(void)
	void  __fastcall zCBspTree_RenderVobList(zCBspTree* _this, void*);
	CInvoke <void(__thiscall *) (zCBspTree* _this)> pzCBspTree_RenderVobList(0x0052D0A0, zCBspTree_RenderVobList, IVK_AUTO);
	void  __fastcall zCBspTree_RenderVobList(zCBspTree* _this, void*)
	{
		zCArray<zCVob*> namesToDraw;

		if (theApp.options.GetIntVal("showInvisibleVobs"))
		{
			for (int i = 0; i < _this->renderVobList.GetNum(); i++)
			{
				zCVob* vob = _this->renderVobList[i];

				if (vob && !vob->GetShowVisual() && vob->GetVisual() && ogame->GetCamera())
				{
					int dist = ogame->GetCamera()->GetVob()->GetPositionWorld().Distance(vob->GetPositionWorld());

					if (dist <= theApp.options.GetIntVal("rangeVobs") * 3)
					{
						vob->bbox3D.Draw(GFX_GREEN);
					}

				}
				/*

				if (theApp.options.GetIntVal("showWPNames"))
				{
				if (vob && (dynamic_cast<zCVobWaypoint*>(vob) || dynamic_cast<zCVobSpot*>(vob)))
				{
				zPOINT3 pos = zCCamera::activeCam->Transform(vob->GetPositionWorld());
				if (pos[VZ]>0)
				if (pos[VZ]<700)
				{
				namesToDraw.Insert(vob);
				};
				}
				}
				*/
			}
		}

		pzCBspTree_RenderVobList(_this);

		/*
		if (theApp.options.GetIntVal("showWPNames"))
		{
		for (int i = 0; i < namesToDraw.GetNumInList(); i++)
		{
		zCVob* vob = _this->renderVobList[i];
		if (vob)
		{
		zSTRING				name = vob->GetVobName();

		int xscr, yscr;
		zPOINT3 pos = zCCamera::activeCam->Transform(vob->GetPositionWorld());

		//if (pos[VZ]<200)	name = name + ":" + ((vob->GetVisual()) ? (vob->GetVisual()->GetVisualName()) : "%");
		zCCamera::activeCam->Project(&pos, xscr, yscr);
		screen->Print(screen->anx(xscr), screen->any(yscr), name);
		}
		}
		}
		*/
	}

	//0x00630580 void __cdecl zDieter_StartUp(struct HWND__ * *)
	void  __cdecl zDieter_StartUp(struct HWND__** handle);
	CInvoke <void(__cdecl *) (struct HWND__** handle)> pzDieter_StartUp(0x00630580, zDieter_StartUp, IVK_AUTO);
	void  __cdecl zDieter_StartUp(struct HWND__** handle)
	{
		int xRes = zoptions->ReadInt("VIDEO", "zVidResFullscreenX", 800);

		if (xRes <= 1000)
		{
			int x, y;
			GetDesktopResolution(x, y);

			zoptions->WriteInt("VIDEO", "zVidResFullscreenX", x, 0);
			zoptions->WriteInt("VIDEO", "zVidResFullscreenY", y, 0);
			zoptions->WriteInt("VIDEO", "zVidResFullscreenBPP", 32, 0);

		}

		pzDieter_StartUp(handle);
	}

	/*
	//0x0055F010 public: void __thiscall zCFile3DS::Save3DS(class zSTRING const &,class zCMesh *)
	void  __fastcall zCFile3DS_Save3DS(zCFile3DS* _this, void*, class zSTRING const &, class zCMesh *);
	CInvoke <void(__thiscall *) (zCFile3DS* _this, class zSTRING const &, class zCMesh *)> pzCFile3DS_Save3DS(0x0055F010, zCFile3DS_Save3DS, IVK_AUTO);
	void  __fastcall zCFile3DS_Save3DS(zCFile3DS* _this, void*, class zSTRING const & str, class zCMesh * mesh)
	{
	if (!mesh)
	{
	MessageBox(0, "Mesh is null!", 0, 0);
	}
	MessageBox(0, str, 0, 0);
	pzCFile3DS_Save3DS(_this, str, mesh);

	}
	*/

	//0x0055E8B0 public: void __thiscall zCBinTree3DS::DropMesh(class zCMesh *)




	//HOOK zCBinTree3DS_DropMesh   AS(0x0055E8B0, &zCBinTree3DS::DropMesh);
	//HOOK zCWorld_SaveWorld   AS(0x00627BB0, &zCWorld::SaveWorldNew);


	static	zCTree<zCVob>*& s_firstVobSaveWorld = *(zCTree<zCVob>**)(0x9A4410);

	//oCDoc*&                document         = *(oCDoc**)               0x00AB05D0;
	static	zBOOL&							s_saveLevelMesh = *(int*)0x9A4360;
	static	zBOOL&							s_saveWayNet = *(int*)0x9A436C;

	/*
	zBOOL zCWorld::SaveWorldNew(const zSTRING& fileName, const zTWorldSaveMode saveMode, const zBOOL writeBinary, const zBOOL _saveLevelMesh)
	{

		zCPolygon::S_ResetMorphedVerts();

		//
		//zERR_MESSAGE(5, 0, "D: WORLD: Saving World: \"" + fileName + "\"");

		cmd << Col16(CMD_YELLOW) << "Start saving world..." << endl;

		// CamVob entfernen
		zCVob *camVob = 0;
		if (zCCamera::activeCam)
			if (zCCamera::activeCam->GetVob())
			{
				camVob = zCCamera::activeCam->GetVob();
				zADDREF_ACCT(camVob);
				RemoveVob(camVob);
			};

		// Setup
		zBOOL saveGame;
		{
			// saveGame ?
			saveGame = (saveMode == zWLD_SAVE_GAME_SAVED_DYN);

			// fileIOMode
			s_worldSaveMode = saveMode;

			//

			s_firstVobSaveWorld = &globalVobTree;
			s_saveLevelMesh = _saveLevelMesh;
			s_saveWayNet = (s_worldSaveMode != zWLD_SAVE_COMPILED_ONLY);
		};

		// ArcMode ermitteln
		zTArchiveMode arcSaveMode = writeBinary ? zARC_MODE_BINARY_SAFE : zARC_MODE_ASCII;
		if ((writeBinary) && (saveGame))
		{
			// .ini auswerten: falls gewuenscht den schnellen/unsicheren Modus benutzen
			zBOOL fastBinSaveGames = zoptions->ReadBool("INTERNAL", "zFastSaveGames", TRUE);
			if (fastBinSaveGames)
				arcSaveMode = zARC_MODE_BINARY;
		};


		cmd << Col16(CMD_YELLOW) << "arcSaveMode set" << endl;

		// Archiv schreiben
		zoptions->ChangeDir(DIR_WORLD);
		zCArchiver *arc = zarcFactory->CreateArchiverWrite(fileName, arcSaveMode, saveGame, 0);


		cmd << Col16(CMD_YELLOW) << "CreateArchiverWrite..." << endl;


		zBOOL res = (arc != 0);
		if (arc) {
			arc->WriteObject(this);
			arc->Close();
			zRELEASE(arc);
		};

		// CamVob einfuegen
		if (camVob) {
			AddVob(camVob);
			zRELEASE_ACCT_NONULL(camVob);
		};

		// cleanup
		s_firstVobSaveWorld = 0;

		return res;
	}

	//



	/*
	COMPILING_GOTHIC =====================
	zCWorld::Render mainRenderWorld
	zBOOL spcCCSSequencer::InitRoleVob(zCVob *vob) return TRUE;
	zBOOL spcCCSSequencer::DeinitRoleVob(zCVob *vob)
	void spcCCSSequencer::OnCsStart()
	void spcCListCutsceneDlg::OnOuStart()
	void spcCListCutsceneDlg::OnTest()

	COMPILING_SPACER ======================
	int zCCSCamera :: EV_Play			(zCCSCamera_EventMsg *msg)
	int	zCCSCamera :: EV_Stop			(zCCSCamera_EventMsg *msg)
	zCCamTrj_KeyFrame virtual	void			EndMovement		 ();

	const int zMAX_POLY_CLIP_VERTS_SCENE	= 8192	*2;
	const int zMAX_POLY_VERT_LIST_SCENE		= 16384	*2;

	return 3 functins
	void zCAIPlayer::UpdateEffects ()
	void zCAIPlayer::AddBlood (const zPOINT3& rayOrg, const zVEC3& ray, zREAL size, zBOOL scaleUp, zSTRING* texName)
	void zCAIPlayer::AddWeaponTrailSegment (const zPOINT3& posUp, const zPOINT3& posDown)

	void zCCFASTCALL zCPolygon::LightClippedPoly (const zBOOL noStaticLightmaps)
	zBOOL ShouldActivatePortal	(zCPolygon *portal) if (zCBspTree::S_GetRenderAllPortals()) return TRUE;


	zCCollisionDetector enum							{ zNUM_COLL_REPORTS = 64 };
	*/


	//HOOK zCWorld_TraverseBspAddVobsToMesh   AS(0x00622A80, &zCWorld::TraverseBspAddVobsToMesh_New);

	//0x00622A80 private: int __thiscall zCWorld::TraverseBspAddVobsToMesh(class zCCBspTree *,class zCTree<class zCVob> *)
}