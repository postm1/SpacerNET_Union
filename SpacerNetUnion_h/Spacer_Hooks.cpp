// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	
	/*

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

	*/

	//0x00624970 protected: virtual void __thiscall zCWorld::VobRemovedFromWorld(class zCVob *
	/*
	void __fastcall Invk_zVobRemovedFromWorld(zCWorld* _this, void*, class zCVob *);
	CInvoke<void(__thiscall*)(zCWorld* _this, class zCVob *)> pInvk_zCVobUnarchive(0x00624970, Invk_zVobRemovedFromWorld);
	void __fastcall Invk_zVobRemovedFromWorld(zCWorld* _this, void*, class zCVob* pVob)
	{


	}
	*/
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
#if ENGINE == Engine_G1
		_this->UpdateVisualDependencies();
#else
		_this->UpdateVisualDependencies(TRUE);
		const zREAL VOB_IGNORE_MIN_EXTEND = 10;
		zREAL minExtend = _this->GetBBox3DWorld().GetMinExtent();
		if (minExtend > VOB_IGNORE_MIN_EXTEND) _this->m_zBias = 0;


		RecalcWPBBox(_this);
#endif

	}

	



	/*



	//Проверка на инициализацию парсера для oCVisualFX, в спейсере именной такой код
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

	*/

	// Убирает лишний код в обработке звука по зонам, нужно для спейсера
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