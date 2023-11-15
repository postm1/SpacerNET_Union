// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	//0x005FFC70 protected: virtual void __thiscall zCVob::Unarchive(class zCArchiver &)
	void __fastcall Invk_zCVobUnarchive(zCVob* _this, void*, class zCArchiver &);
	CInvoke<void(__thiscall*)(zCVob* _this, class zCArchiver &)> pInvk_zCVobUnarchive(0x005FFC70, Invk_zCVobUnarchive);
	void __fastcall Invk_zCVobUnarchive(zCVob* _this, void*, class zCArchiver & arc)
	{
		_this->zCObject::Unarchive(arc);

		_this->ResetOnTimer();

		{
			int	pack = 0;
			arc.ReadInt("pack", pack);
			if (pack == 0)			_this->UnarchiveVerbose(arc);
			else					_this->UnarchivePacked(arc, pack);
		};

		// SaveGame
		if (arc.InSaveGame())
		{
			_this->SetSleepingMode(zTVobSleepingMode(arc.ReadByte("sleepMode")));

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


	// Removes unnecessary code in sound processing by zone, needed for a spacer
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
	//rx_light
	//0x006015D0 public: virtual int __fastcall zCVob::Render(struct zTRenderContext &)
	int  __fastcall zCVob_Render(zCVob*, struct zTRenderContext &);
	CInvoke <int (__fastcall *) (zCVob*, struct zTRenderContext &)> pzCVob_Render(0x006015D0, zCVob_Render, IVK_AUTO);
	int  __fastcall zCVob_Render(zCVob* _this, struct zTRenderContext & renderContext)
	{

		if (theApp.s_pLightSphereMesh != NULL && theApp.vobLightSelected == _this && theApp.options.GetIntVal("showLightRadiusVob"))
		{
			if (theApp.vobLightSelected->lightData.range != 0.0f)
			{
				zREAL scaler = theApp.vobLightSelected->lightData.range / theApp.s_pLightSphereMesh->GetBBox3D().GetSphere3D().radius;
				theApp.s_pLightSphereMesh->Scale(scaler, scaler, scaler);
			}

			zCCamera::activeCam->SetTransform(zCAM_TRAFO_WORLD, _this->trafoObjToWorld);
			zTCam_ClipType	meshClip = zCCamera::activeCam->BBox3DInFrustum(theApp.s_pLightSphereMesh->GetBBox3D(), renderContext.clipFlags);
			renderContext.distVobToCam = zCCamera::activeCam->camMatrix.GetTranslation().LengthApprox();
			renderContext.hintLightingFullbright = TRUE;

			if ((meshClip != zCAM_CLIP_TRIV_OUT) && (renderContext.distVobToCam<5000.0F))		// 50m
			{
				//zTRnd_AlphaBlendSource oldBlendSrc = zrenderer->GetAlphaBlendSource();
				//zTRnd_AlphaBlendFunc   oldBlendFunc= zrenderer->GetAlphaBlendFunc  ();
				zTRnd_PolyDrawMode	   oldDrawMode = zrenderer->GetPolyDrawMode();
				//zREAL				   oldBlendFac = zrenderer->GetAlphaBlendFactor();

				zrenderer->SetPolyDrawMode(zRND_DRAW_WIRE);
				//zrenderer->SetAlphaBlendSource(zRND_ALPHA_SOURCE_CONSTANT);
				//zrenderer->SetAlphaBlendFunc  (zRND_ALPHA_FUNC_BLEND	 );
				//zrenderer->SetAlphaBlendFactor(0.5F						 );

				if (theApp.vobLightSelected->lightData.range != 0.0f)
					theApp.s_pLightSphereMesh->Render(renderContext);

				//zrenderer->SetAlphaBlendSource(oldBlendSrc);
				//zrenderer->SetAlphaBlendFunc  (oldBlendFunc);
				//zrenderer->SetAlphaBlendFactor(oldBlendFac);
				zrenderer->SetPolyDrawMode(oldDrawMode);

			};
		}

		return pzCVob_Render(_this, renderContext);
	}

	//0x0052D0A0 private: void __thiscall zCBspTree::RenderVobList(void)
	void  __fastcall zCBspTree_RenderVobList(zCBspTree* _this, void*);
	CInvoke <void(__thiscall *) (zCBspTree* _this)> pzCBspTree_RenderVobList(0x0052D0A0, zCBspTree_RenderVobList, IVK_AUTO);
	void  __fastcall zCBspTree_RenderVobList(zCBspTree* _this, void*)
	{

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

	// This hook prevents the game from modifying the window style on lower resolutions.
	// Without this, the main window in C# will have additional border that will prevent
	// from interacting with C# UI while using mouse.
	// NOTE! this fix works only during game startup, changing the game resolution
	// while Spacer.NET is running might still not work properly.
	int vidSetMode_Union(VIDMODE*);
	HOOK ivk_vidSetMode AS(&vidSetMode, &vidSetMode_Union);
	int vidSetMode_Union(VIDMODE*)
	{
		ivk_vidSetMode.Detach();
		return false;
	}

}