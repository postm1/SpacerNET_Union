	// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	HOOK ivk_oCMobInter_CanInteractWith AS(&oCMobInter::CanInteractWith, &oCMobInter::CanInteractWith_Union);
	int oCMobInter::CanInteractWith_Union(oCNpc* npc) {

		auto ladder = this->CastTo<oCMobLadder>();

		if (!ladder && theApp.options.GetIntVal("bBlockPlayeUseMobInter"))
		{
			return FALSE;
		}

		return THISCALL(ivk_oCMobInter_CanInteractWith)(npc);
	}

	// ESC key crashfix
	HOOK Ivk_CGameManager_HandleEvent AS(&CGameManager::HandleEvent, &CGameManager::HandleEvent_Hook);
	int CGameManager::HandleEvent_Hook(int key)
	{
		static auto keySender = (sendKey)GetProcAddress(theApp.module, "AcceptKey");

		//cmd << key << endl;

		keySender(key);

// Mouse wheel on G1...
#if engine == ENGINE_G1
		if (key == zMOUSE_SCROLL_UP || key == zMOUSE_SCROLL_DOWN)
		{
			theApp.mouseWheelKeyCodeG1 = key;
		}

#endif

		if (key == KEY_ESCAPE)
			return FALSE;

		return FALSE;

		//return THISCALL(Ivk_CGameManager_HandleEvent)(key);

	}

	HOOK Ivk_oCGame_UpdatePlayerStatus AS(&oCGame::UpdatePlayerStatus, &oCGame::UpdatePlayerStatus_Hook);
	void oCGame::UpdatePlayerStatus_Hook()
	{
		return;
	}

	HOOK Ivk_oCGame_HandleEvent AS(&oCGame::HandleEvent, &oCGame::HandleEvent_Hook);
	int oCGame::HandleEvent_Hook(int key)
	{
		return FALSE;
	}


	//CS Camera EV_PLAY crashfix
	HOOK Ivk_CheckUnderWaterFX AS(&zCAICamera::CheckUnderWaterFX, &zCAICamera::CheckUnderWaterFX_Hook);
	int zCAICamera::CheckUnderWaterFX_Hook()
	{
		return FALSE;
	}

	// Def vobname Fix
	HOOK Ivk_zCVob_SetVobPresetName   AS(&zCVob::SetVobPresetName, &zCVob::SetVobPresetName_Hook);
	void zCVob::SetVobPresetName_Hook(class zSTRING const &presetName)
	{
		if (!vobPresetName) vobPresetName = zNEW(zSTRING(presetName));
		else (*vobPresetName) = presetName;
	}

	// Start screen fix
	HOOK Invk_InitScreen_Open   AS(&CGameManager::InitScreen_Open, &CGameManager::InitScreen_Open_Hook);
	void CGameManager::InitScreen_Open_Hook()
	{
		
	}


	// Start screen fix
	HOOK Invk_InitScreen_Close AS(&CGameManager::InitScreen_Close, &CGameManager::InitScreen_Close_Hook);
	void CGameManager::InitScreen_Close_Hook()
	{
		
	}


	


	HOOK Invk_zCVobArchive   AS(&zCVob::Archive, &zCVob::Archive_Hook);
	void zCVob::Archive_Hook(zCArchiver& arc)
	{
#if ENGINE > Engine_G1
		if (zDYNAMIC_CAST<zCParticleFX>(this->GetVisual()))
		{
			zDYNAMIC_CAST<zCParticleFX>(this->GetVisual())->m_bVisualNeverDies = TRUE;
		}
			
#endif
		THISCALL(Invk_zCVobArchive)(arc);
	}

	HOOK Invk_oCWorld_SaveWorld   AS(&oCWorld::SaveWorld, &oCWorld::SaveWorld_Hook);
	int oCWorld::SaveWorld_Hook(zSTRING const & fileName, enum zCWorld::zTWorldSaveMode saveMode, int writeBinary, int _saveLevelMesh)
	{

		if (this->GetWayNet())
		{
			if (this->GetWayNet()) this->GetWayNet()->UpdateVobDependencies();
			this->GetWayNet()->ClearVobDependencies();
		}

		int result = THISCALL(Invk_oCWorld_SaveWorld)(fileName, saveMode, writeBinary, _saveLevelMesh);


		if (this->GetWayNet()) this->GetWayNet()->CreateVobDependencies(this);

		return result;
	}


	HOOK zCWayNet_Unarchive   AS(&zCWayNet::Unarchive, &zCWayNet::Unarchive_Hook);
	void zCWayNet::Unarchive_Hook(zCArchiver& arc)
	{
		THISCALL(zCWayNet_Unarchive)(arc);
		this->CreateVobDependencies(world);
	}

	//kills certain triggers that are launch automatically, otherwise they will not be in the desired state in the game
	HOOK Invk_zCTriggerWorldStart_PostLoad AS(&zCTriggerWorldStart::PostLoad, &zCTriggerWorldStart::PostLoad_Hook);
	void zCTriggerWorldStart::PostLoad_Hook()
	{

	}

	HOOK ivk_oCGame_CloseSavescreen AS(&oCGame::CloseSavescreen, &oCGame::CloseSavescreen_Hook);
	//void CloseSavescreen_Hook();
	void oCGame::CloseSavescreen_Hook()
	{
		//THISCALL(ivk_oCGame_CloseSavescreen)();
	}

	HOOK ivk_oCGame_OpenSavescreen AS(&oCGame::OpenSavescreen, &oCGame::OpenSavescreen_Hook);
	//void OpenSavescreen_Hook(bool);
	void oCGame::OpenSavescreen_Hook(bool a)
	{
		//THISCALL(ivk_oCGame_OpenSavescreen)(a);
	}

	//Kills the in-game menu
	HOOK ivk_CGameManager_InitScreen_Menu AS(&CGameManager::InitScreen_Menu, &CGameManager::InitScreen_Menu_Hook);
	//void InitScreen_Menu_Hook();
	void CGameManager::InitScreen_Menu_Hook()
	{
		//THISCALL(ivk_CGameManager_InitScreen_Menu)();
	}




	// Hook the camera so that it flies through the frames in the camera tool
	HOOK ivk_zCCSCamera_Unarchive AS(&zCCSCamera::Unarchive, &zCCSCamera::Unarchive_Hook);
	void Unarchive_Hook(zCArchiver &);
	void zCCSCamera::Unarchive_Hook(zCArchiver & arc)
	{
		THISCALL(ivk_zCCSCamera_Unarchive)(arc);
		if (!arc.InProperties())
		{
			this->SetSleeping(FALSE);
		}
	}


	HOOK Invk_zCParticleFX   AS(&zCParticleFX::zCParticleFX, &zCParticleFX::zCParticleFX_Hook);
	zCParticleFX* zCParticleFX::zCParticleFX_Hook()
	{
		THISCALL(Invk_zCParticleFX)();
		this->dontKillPFXWhenDone = TRUE;
		return this;
	}

	// Убивает запуск контроллера PFX в спейсере
	HOOK ivk_zCPFXControler_PostLoad AS(&zCPFXControler::PostLoad, &zCPFXControler::PostLoad_Hook);
	void zCPFXControler::PostLoad_Hook()
	{
		//THISCALL(ivk_zCPFXControler_PostLoad)();
	}

	HOOK ivk_zCPFXControler_OnTrigger AS(&zCPFXControler::OnTrigger, &zCPFXControler::OnTrigger_Hook);
	void zCPFXControler::OnTrigger_Hook(zCVob *otherVob, zCVob *vobInstigator)
	{
		zCParticleFX *pfx = GetPFX();

		if (pfx)
		{
			pfx->dontKillPFXWhenDone = TRUE;
			pfx->RestoreEmitterOutput();
			pfx->CreateParticles();
		};
	}

	HOOK ivk_zCPFXControler_OnUntrigger AS(&zCPFXControler::OnUntrigger, &zCPFXControler::OnUntrigger_Hook);
	void zCPFXControler::OnUntrigger_Hook(zCVob *vob1, zCVob *vob2)
	{

		//print.PrintRed(Z killVobWhenDone);

		zCParticleFX *pfx = dynamic_cast<zCParticleFX*>(GetVisual());
		if (!pfx) return;

		pfx->StopEmitterOutput();

		SetVisual(0);
	}

	//Enables adding zones to the world
	HOOK ivk_zCWorld_zCWorld AS(&zCWorld::zCWorld, &zCWorld::zCWorld_Hook);
	//zCWorld* zCWorld_Hook();
	zCWorld* zCWorld::zCWorld_Hook() {
		THISCALL(ivk_zCWorld_zCWorld)();
		addZonesToWorld = TRUE;
#if ENGINE > Engine_G1
		SetWaveAnisEnabled(zoptions->ReadBool("SPACER", "zSpacerWaterAniEnabled", FALSE));
#endif
		return this;
	}



	// Fix sound after saving sound vob
	HOOK ivk_zCVobSound_Archive AS(&zCVobSound::Archive, &zCVobSound::Archive_Hook);
	//void Archive_Hook(zCArchiver &);
	void zCVobSound::Archive_Hook(zCArchiver & arc) {
		THISCALL(ivk_zCVobSound_Archive)(arc);
		if (arc.InProperties())
		{
			this->StopSound();
			this->StartSound(TRUE);
		};
	}


	// Fix sound after loading sound vob
	HOOK ivk_zCVobSound_Unarchive AS(&zCVobSound::Unarchive, &zCVobSound::Unarchive_Hook);
	//void Unarchive_Hook(zCArchiver &);
	void zCVobSound::Unarchive_Hook(zCArchiver & arc) {
		THISCALL(ivk_zCVobSound_Unarchive)(arc);
		if (arc.InProperties())
		{
			this->StopSound();
			this->StartSound(TRUE);
		};
	}


	HOOK ivk_zCRnd_D3D_XD3D_InitPerDX AS(&zCRnd_D3D::XD3D_InitPerDX, &zCRnd_D3D::XD3D_InitPerDX_Hook);
	int zCRnd_D3D::XD3D_InitPerDX_Hook(long flags, int x, int y, int bpp, int id) {
		int result = THISCALL(ivk_zCRnd_D3D_XD3D_InitPerDX)(0, x, y, bpp, id);
		return result;
	}
	

	HOOK ivk_AddVob AS(&zCBspTree::AddVob, &zCBspTree::AddVob_Hook);
	void zCBspTree::AddVob_Hook(zCVob* vob) {
		if (vob)
		{

			if (theApp.g_bIsPlayingGame)
			{
				vob->dontWriteIntoArchive = true;
				THISCALL(ivk_AddVob)(vob);
				return;
			}

			if (theApp.treeIsReady && !theApp.nextInsertBlocked)
			{
				//cmd << "AddVob_Hook: " << (int)vob << endl;

				zCVobLight* vobLight = dynamic_cast<zCVobLight*>(vob);

				//oCVisualFX* pVisualVob = dynamic_cast<oCVisualFX*>(vob);

				bool flag = (vobLight) && (vob->GetParent() == 0) || (vob->GetVobName() == "Vob_PFX_Editor") || (vob->GetVobName() == "Vob_PFX_EditorPanel");


				// don't add PFX child vobs into spacer list, don't save them
				if (auto pVisual = vob->GetVisual())
				{
					if (auto pPFX = pVisual->CastTo<zCParticleFX>())
					{
						if (vob->GetParentVob() == pfxManager.testVob || vob->GetParentVob() == pfxEditorVob)
						{
							//print.PrintRed("PFX child is not added");
							flag = true;
							vob->dontWriteIntoArchive = true;
						}
					}
				}

				if (!flag)
				{
					//OutFile("zCBspTree::AddVob: (" + AHEX32((uint)vob) + ") Parent: " + AHEX32(vob->GetParent()) + " NodesCount: " + A(ogame->GetWorld()->globalVobTree.CountNodes() - 1), false);
					
					if (theApp.selectNextVobForce)
					{
						theApp.selectNextVob = true;
						theApp.selectNextVobForce = false;
					}

					theApp.OnCreateVob(vob, theApp.selectNextVob);
					theApp.selectNextVob = false;
					
				}

			}
		}

		THISCALL(ivk_AddVob)(vob);
	}
	
	


	


	HOOK ivk_oCNpc_DoTakeVob AS(&oCNpc::DoTakeVob, &oCNpc::DoTakeVob_Union);
	int oCNpc::DoTakeVob_Union(zCVob* vob) {
		
		return FALSE;
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


	HOOK Invk_zCVobUnarchive AS(&zCVob::Unarchive, &zCVob::Unarchive_Union);
	void zCVob::Unarchive_Union(zCArchiver& arc)
	{
		this->zCObject::Unarchive(arc);

		this->ResetOnTimer();

		{
			int	pack = 0;
			arc.ReadInt("pack", pack);
			if (pack == 0)			this->UnarchiveVerbose(arc);
			else					this->UnarchivePacked(arc, pack);
		};

		// SaveGame
		if (arc.InSaveGame())
		{
			this->SetSleepingMode(zTVobSleepingMode(arc.ReadByte("sleepMode")));

			zREAL nextOnTimerRel = this->nextOnTimer;
			arc.ReadFloat("nextOnTimer", nextOnTimerRel);
			this->nextOnTimer = (nextOnTimerRel >= zREAL_MAX) ? nextOnTimerRel : (ztimer->totalTimeFloat + nextOnTimerRel);
			if (this->GetPhysicsEnabled()) {
				this->GetRigidBody()->Unarchive(arc);
			};
		};

		if (!arc.InProperties())
		{
			this->RepairIllegalFloats(this->trafoObjToWorld);
			if (!this->trafoObjToWorld.IsUpper3x3Orthonormal())
				this->trafoObjToWorld.MakeOrthonormal();
			this->CorrectTrafo();
		};

		//
#if ENGINE == Engine_G1
		this->UpdateVisualDependencies();
#else
		this->UpdateVisualDependencies(TRUE);
		const zREAL VOB_IGNORE_MIN_EXTEND = 10;
		zREAL minExtend = this->GetBBox3DWorld().GetMinExtent();
		if (minExtend > VOB_IGNORE_MIN_EXTEND) this->m_zBias = 0;


		RecalcWPBBox(this);
#endif
	}



	HOOK ivk_zCVobSound_ProcessZoneList AS(&zCVobSound::ProcessZoneList, &zCVobSound::ProcessZoneList_Union);
	void zCVobSound::ProcessZoneList_Union(zCArraySort<zCZone*> const& zoneList, zCArraySort<zCZone*> const& zoneDeactivateList, zCWorld* homeWorld)
	{
		for (int i = 0; i < zoneList.GetNum(); i++)
		{
			zCVobSound* vobSound = ((zCVobSound*)zoneList[i]);

			if (vobSound)
			{
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
			}

		};

		for (int j = 0; j < zoneDeactivateList.GetNum(); j++) {
			zCVobSound* vobSound = (zCVobSound*)zoneDeactivateList[j];

			if (vobSound)
			{
				vobSound->StopSound();
			}
		};
	}


	HOOK Ivk_zCVobLight_LoadLightPresets AS(&zCVobLight::LoadLightPresets, &zCVobLight::LoadLightPresets_Hook);
	void zCVobLight::LoadLightPresets_Hook()
	{
		Ivk_zCVobLight_LoadLightPresets.Detach();

		zCVobLight::LoadLightPresets();

		for (int i = 0; i < zCVobLight::lightPresetList.GetNumInList(); ++i)
		{
			Stack_PushString(zCVobLight::lightPresetList[i]->presetName);
			GetProcAddress(theApp.module, "AddLightPresetToList")();
		}
	}


	



	HOOK Invk_SortPolysByList   AS(&zCMesh::SortPolysByList, &zCMesh::SortPolysByList_Hook);
	void zCMesh::SortPolysByList_Hook(zCPolygon** list, int listLength)
	{
		// no sorting polys for saving ZEN, much less time
		if (!theApp.useSortPolys)
		{
			cmd << "Saving no sort polys" << endl;
			return;
		}
		else
		{
			THISCALL(Invk_SortPolysByList)(list, listLength);
		}
	}

	HOOK ivk_zERROR_Report AS(&zERROR::Report, &zERROR::Report_Union);
	int zERROR::Report_Union(zERROR_TYPE type, int id, zSTRING const& str_text, signed char levelPrio, unsigned int flag, int line, char* file, char* function) {


		if (theApp.zSpyActive && theApp.spacerWasInit)
		{
			static auto pointer = (callVoidFunc)GetProcAddress(theApp.module, "InfoWin_AddTextZSPY");


			Stack_PushString(str_text + "\n");

			if (type == zERR_TYPE_OK)
			{
				Stack_PushString("#000000");
			}
			else if (type == zERR_TYPE_INFO)
			{
				Stack_PushString("#000000");
			}
			else if (type == zERR_TYPE_WARN)
			{
				Stack_PushString("#009600");
			}
			else if (type == zERR_TYPE_FAULT)
			{
				Stack_PushString("#960000");
			}
			else if (type == zERR_TYPE_FATAL)
			{
				Stack_PushString("#C80000");
			}


			pointer();
		}

		return THISCALL(ivk_zERROR_Report)(type, id, str_text, levelPrio, flag, line, file, function);
	}

	// globalWorldLoadType
	int globalWorldLoadType = zCWorld::zWLD_LOAD_EDITOR_COMPILED;

	auto Patch_oCGame_LoadGame = InvokeAuto_BySignature("&oCGame::LoadGame", &oCGame::LoadGame_Patch, IVK_REDEFINE);
	void oCGame::LoadGame_Patch(int slotID, const struct zSTRING& wldName)
	{

		switch (globalWorldLoadType)
		{
		case 1: GetGameWorld()->LoadWorld(wldName, zCWorld::zWLD_LOAD_EDITOR_COMPILED); break;
		case 2: GetGameWorld()->LoadWorld(wldName, zCWorld::zWLD_LOAD_EDITOR_UNCOMPILED); break;
		}
	}


	

#if ENGINE == Engine_G1


	zCVob* __cdecl zCCSCamera_GetPlayerVob(zCCSCamera*);
	CInvoke <zCVob* (__cdecl*) (zCCSCamera*)> pzCCSCamera_GetPlayerVob(0x004B50C0, zCCSCamera_GetPlayerVob, IVK_AUTO);
	zCVob* __cdecl zCCSCamera_GetPlayerVob(zCCSCamera* _this)
	{
		return NULL;
	}

	int  __fastcall zCVob_Render(zCVob*, struct zTRenderContext&);
	CInvoke <int(__fastcall*) (zCVob*, struct zTRenderContext&)> pzCVob_Render(0x005D6090, zCVob_Render, IVK_AUTO);
	int  __fastcall zCVob_Render(zCVob* _this, struct zTRenderContext& renderContext)
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

			if ((meshClip != zCAM_CLIP_TRIV_OUT) && (renderContext.distVobToCam < 5000.0F))		// 50m
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



		auto result = pzCVob_Render(_this, renderContext);


		return result;
	}

	void  __fastcall zCBspTree_RenderVobList(zCBspTree* _this, void*);
	CInvoke <void(__thiscall*) (zCBspTree* _this)> pzCBspTree_RenderVobList(0x0051A7E0, zCBspTree_RenderVobList, IVK_AUTO);
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

			}
		}

		pzCBspTree_RenderVobList(_this);

		
	}

	


#endif


	

#if ENGINE == Engine_G2A

	zCVob * __cdecl zCCSCamera_GetPlayerVob(zCCSCamera*);
	CInvoke <zCVob *(__cdecl *) (zCCSCamera*)> pzCCSCamera_GetPlayerVob(0x004BE400, zCCSCamera_GetPlayerVob, IVK_AUTO);
	zCVob * __cdecl zCCSCamera_GetPlayerVob(zCCSCamera* _this)
	{
		return NULL;
	}


	int  __fastcall zCVob_Render(zCVob*, struct zTRenderContext &);
	CInvoke <int(__fastcall *) (zCVob*, struct zTRenderContext &)> pzCVob_Render(0x006015D0, zCVob_Render, IVK_AUTO);
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

		auto saveOption = zCVob::GetAnimationsEnabled();

		//cmd << saveOption << endl;

		if (_this)
		{
			if (!_this->visual || _this->CastTo<zCVobSpot>() || _this->CastTo<zCVobStartpoint>() || _this->CastTo<zCVobWaypoint>())
			{
				//if (!_this->visual)
				{
					zCVob::SetAnimationsEnabled(FALSE);

					renderContext.m_AniMode = zVISUAL_ANIMODE_NONE;

					/*cmd << _this->GetObjectName() 
						<< " " << zCVob::GetAnimationsEnabled() 
						<< " " << (int)_this->visual
						<< endl;*/
				
				}
			}
			
		}

		
		auto result =  pzCVob_Render(_this, renderContext);


		zCVob::SetAnimationsEnabled(saveOption);
		return result;
	}

	
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
			}
		}

		pzCBspTree_RenderVobList(_this);
	}

	// FIXME_G1 ???!
	
	HOOK Ivk_zCMesh_Render AS(&zCMesh::Render, &zCMesh::Render_Patch);
	zBOOL zCMesh::Render_Patch(zTRenderContext& renderContext, zCOLOR* vertexColor)
	{
		const zPOINT3& camPos = zCCamera::activeCamPos;
		zREAL texCacheInPrio = (renderContext.distVobToCam / renderContext.cam->farClipZ) * 0.5F + 0.25F;

		if (renderContext.clipFlags == -1)
		{
			renderContext.clipFlags = zCCamera::CLIP_FLAGS_FULL;
			zTCam_ClipType meshClip = zCCamera::activeCam->BBox3DInFrustum(bbox3D, renderContext.clipFlags);

			if (meshClip == zCAM_CLIP_TRIV_OUT)
				return FALSE;
		};

		size_t markedPos = zCVertexTransform::s_MemMan.Mark();

		for (int polyCtr = 0; polyCtr < numPoly; ++polyCtr)
		{
			zCPolygon* actPoly = Poly(polyCtr);

			// backface culling
			if (zrenderer->GetPolyDrawMode() != zRND_DRAW_WIRE
				&& zrenderer->GetPolyDrawMode() != zRND_DRAW_FLAT
				&& camPos.Dot(actPoly->polyPlane.normal) <= actPoly->polyPlane.distance)
				continue;


			// 3D-Clipping im Frustum, in World Coordinates
			if (renderContext.clipFlags > 0)
			{
				if (!actPoly->ClipToFrustum(renderContext.clipFlags))
					continue;
			}
			else
				actPoly->Unclip();

			if (actPoly->GetMaterial())
			{
				actPoly->GetMaterial()->ApplyTexAniMapping(actPoly);
				if (actPoly->GetMaterial()->texture)
					actPoly->GetMaterial()->texture->CacheIn(texCacheInPrio);
			}


			int	vertCtr = actPoly->numClipVert - 1;
			do
			{
				zCVertex* actVert = (actPoly->clipVert[vertCtr]);
				if (!actVert->transformedIndex)
				{
					zCVertexTransform* trans = actVert->CreateVertexTransform();

					trans->vertCamSpace = (zCCamera::activeCam->camMatrix) * (actVert->position);
					trans->vertCamSpaceZInv = (zVALUE(1)) / trans->vertCamSpace.n[VZ];

					zCCamera::activeCam->ProjectClamp(trans, trans->vertCamSpaceZInv);
				}

				if (vertexColor)
					actPoly->clipFeat[vertCtr]->lightDyn = *vertexColor;
			} while (vertCtr--);

			actPoly->LightDynCamSpace(camPos, playerLightInt);
			zrenderer->DrawPoly(actPoly);
		}

		zCVertexTransform::s_MemMan.Restore(markedPos);
		return TRUE;
	}
	

#endif


#if ENGINE > Engine_G1
	void __cdecl insertionsort_Hook(void*, unsigned int, unsigned int, int(__cdecl*)(void const*, void const*), bool);
	CInvoke <void(__cdecl*) (void*, unsigned int, unsigned int, int(__cdecl*)(void const*, void const*), bool)> insertionsort_Hooked(0x00553CC0, insertionsort_Hook, IVK_AUTO);
	void __cdecl insertionsort_Hook(void* data, size_t num, size_t size, int(__cdecl* compare)(const void*, const void*), bool falltoqs)
	{
		if (num >= 200000 && theApp.options.GetVal("bSortMerge"))
		{
			cmd << "Sort merging... Elements: " << num << endl;

			sortData(data, num, size, compare);
		}
		else
		{
			const int MAXSIZE = 24;

			char swapplace[MAXSIZE];

			if (size > MAXSIZE) {
				qsort(data, num, size, compare);
				return;
			}

			int swaps = 0;
			for (int i = 1; i < (int)num; i++) {
				void* lower = d(i);
				for (int j = i - 1; j >= 0; j--) {
					void* upper = lower;
					lower = d(j);
					if ((*compare)(upper, lower) < 0) { 
						swaps++;
						memcpy(&swapplace, upper, size);
						memcpy(upper, lower, size);
						memcpy(lower, &swapplace, size);
					}
					else
						j = 0;
				}
				if (falltoqs && swaps > 5 * i + 5) {
					qsort(data, num, size, compare);
					return;
				}
			}
		}
	}
#endif

	// =============================================== TEST HOOKS ===============================================
	/*

	void __cdecl SaveMSH(zCFileBIN & file, zCPolygon * *polyList, int numPoly, zCMesh * mesh);
	CInvoke <void(__cdecl*) (zCFileBIN& file, zCPolygon** polyList, int numPoly, zCMesh* mesh)> pSaveMSH(0x0056D940, SaveMSH, IVK_AUTO);
	void __cdecl SaveMSH(zCFileBIN & file, zCPolygon * *polyList, int numPoly, zCMesh * mesh)
	{
		
	}
	*/

	//test hook
	int __cdecl Wld_InsertNpc_Hook();
	//CInvoke <int(__cdecl *) (void)> Wld_InsertNpc_Hooked(0x6DF1F0, Wld_InsertNpc_Hook, IVK_AUTO);
	int __cdecl Wld_InsertNpc_Hook() {
		int index;
		zSTRING posPoint;

		zCParser* p = zCParser::GetParser();
		p->GetParameter(posPoint);
		p->GetParameter(index);

		posPoint = posPoint.Upper();


		auto& foundPair = theApp.respawnShowList[posPoint];

		auto sym = parser->GetSymbol(index);

		if (!sym) { cmd << "not found: " << posPoint << endl;  return 0; }

		CString monsterName = sym->name;

		// found
		if (!foundPair.IsNull())
		{
			foundPair.GetValue()->monsters.Insert(monsterName);
		}
		else
		{
			auto entry = new RespawnEntry();
			entry->monsters.Insert(monsterName);
			theApp.respawnShowList.Insert(posPoint, entry);
		}




		return FALSE;

	}
}