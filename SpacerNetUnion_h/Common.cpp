	// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	// ESC key crashfix
	HOOK Ivk_CGameManager_HandleEvent AS(&CGameManager::HandleEvent, &CGameManager::HandleEvent_Hook);
	int CGameManager::HandleEvent_Hook(int key)
	{
		static auto keySender = (sendKey)GetProcAddress(theApp.module, "AcceptKey");

		keySender(key);

		if (key == KEY_ESCAPE)
			return FALSE;

		return THISCALL(Ivk_CGameManager_HandleEvent)(key);
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


	HOOK Invk_zCParticleFX   AS(&zCParticleFX::zCParticleFX, &zCParticleFX::zCParticleFX_Hook);
	zCParticleFX* zCParticleFX::zCParticleFX_Hook()
	{
		THISCALL(Invk_zCParticleFX)();
		this->dontKillPFXWhenDone = TRUE;
		return this;
	}


	HOOK Invk_zCVobArchive   AS(&zCVob::Archive, &zCVob::Archive_Hook);
	void zCVob::Archive_Hook(zCArchiver& arc)
	{
#if ENGINE > Engine_G1
		if (zDYNAMIC_CAST<zCParticleFX>(this->GetVisual()))
			zDYNAMIC_CAST<zCParticleFX>(this->GetVisual())->m_bVisualNeverDies = TRUE;
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

	//убивает определенные триггеры, которые запускаютс€ автоматом, иначе в игре они будут не в нужном состо€нии
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

	//0x006C2250 public: virtual void __thiscall oCGame::OpenSavescreen(bool)
	HOOK ivk_oCGame_OpenSavescreen AS(&oCGame::OpenSavescreen, &oCGame::OpenSavescreen_Hook);
	//void OpenSavescreen_Hook(bool);
	void oCGame::OpenSavescreen_Hook(bool a)
	{
		//THISCALL(ivk_oCGame_OpenSavescreen)(a);
	}

	//”бивает внутриигровое меню
	HOOK ivk_CGameManager_InitScreen_Menu AS(&CGameManager::InitScreen_Menu, &CGameManager::InitScreen_Menu_Hook);
	//void InitScreen_Menu_Hook();
	void CGameManager::InitScreen_Menu_Hook()
	{
		//THISCALL(ivk_CGameManager_InitScreen_Menu)();
	}




	// ’ук на камеру, чтобы она летала по кадрам в инструменте работы с камерой
	//0x004BFBC0 public: virtual void __thiscall zCCSCamera::Unarchive(class zCArchiver &)
	HOOK ivk_zCCSCamera_Unarchive AS(&zCCSCamera::Unarchive, &zCCSCamera::Unarchive_Hook);
	//void Unarchive_Hook(zCArchiver &);
	void zCCSCamera::Unarchive_Hook(zCArchiver & arc)
	{
		THISCALL(ivk_zCCSCamera_Unarchive)(arc);
		if (!arc.InProperties())
		{
			this->SetSleeping(FALSE);
		}
	}

	// ”бивает запуск контроллера PFX в спейсере
	//0x006148B0 public: virtual void __thiscall zCPFXControler::PostLoad(void)
	HOOK ivk_zCPFXControler_PostLoad AS(&zCPFXControler::PostLoad, &zCPFXControler::PostLoad_Hook);
	//void PostLoad_Hook();
	void zCPFXControler::PostLoad_Hook()
	{
		//THISCALL(ivk_zCPFXControler_PostLoad)();
	}

	//0x00614960 public: virtual void __thiscall zCPFXControler::OnUntrigger(class zCVob *,class zCVob *)
	HOOK ivk_zCPFXControler_OnUntrigger AS(&zCPFXControler::OnUntrigger, &zCPFXControler::OnUntrigger_Hook);
	//void OnUntrigger_Hook(zCVob *vob1, zCVob *vob2);
	void zCPFXControler::OnUntrigger_Hook(zCVob *vob1, zCVob *vob2)
	{
		zCParticleFX *pfx = dynamic_cast<zCParticleFX*>(GetVisual());
		if (!pfx) return;

		pfx->StopEmitterOutput();
		// если убрать визуал, то PPX-дочерние не будут показывать (динамеческие, которые добавл€ютс€ при активации PFX)
		//_this->SetVisual(0);
	}

	//¬ключает добавление зон в мир
	//0x0061FA40 public: __thiscall zCWorld::zCWorld(void)
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



	// ‘икс звука после сохранени€ звукового воба
	//0x0063E3D0 protected: virtual void __thiscall zCVobSound::Archive(class zCArchiver &)
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


	// ‘икс звука после загрузки звукового воба
	//0x0063E540 protected: virtual void __thiscall zCVobSound::Unarchive(class zCArchiver &)
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

	// 0x00643F20 private: int __thiscall zCRnd_D3D::XD3D_InitPerDX(long, int, int, int, int)
	HOOK ivk_zCRnd_D3D_XD3D_InitPerDX AS(&zCRnd_D3D::XD3D_InitPerDX, &zCRnd_D3D::XD3D_InitPerDX_Hook);
	//int XD3D_InitPerDX_Hook(long,int,int,int,int);
	int zCRnd_D3D::XD3D_InitPerDX_Hook(long flags, int x, int y, int bpp, int id) {
		int result = THISCALL(ivk_zCRnd_D3D_XD3D_InitPerDX)(0, x, y, bpp, id);
		return result;
	}

	HOOK ivk_AddVob AS(&zCBspTree::AddVob, &zCBspTree::AddVob_Hook);
	void zCBspTree::AddVob_Hook(zCVob* vob) {
		if (vob)
		{
			if (theApp.treeIsReady && !theApp.nextInsertBlocked)
			{
				zCVobLight* vobLight = dynamic_cast<zCVobLight*>(vob);

				//oCVisualFX* pVisualVob = dynamic_cast<oCVisualFX*>(vob);

				bool flag = (vobLight) && (vob->GetParent() == 0) || (vob->GetVobName() == "Vob_PFX_Editor");

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

	/*
	//0x00624970 protected: virtual void __thiscall zCWorld::VobRemovedFromWorld(class zCVob *)
	HOOK Ivk_VobRemovedFromWorld AS(&zCWorld::VobRemovedFromWorld, &zCWorld::VobRemovedFromWorld_Hook);

	void zCWorld::VobRemovedFromWorld_Hook(zCVob* pVob)
	{
		THISCALL(Ivk_VobRemovedFromWorld)(pVob);
	}
	*/
	
}