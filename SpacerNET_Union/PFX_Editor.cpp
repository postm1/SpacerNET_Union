// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	typedef zCParticleEmitter* (*fptr)(class zSTRING const&);


#if ENGINE == Engine_G1
	zCArraySort<zCParticleEmitter*>& s_emitterPresetList = *(zCArraySort<zCParticleEmitter*>*)0x00873FA0;
	oCParticleControl*& pfxcGlobal = *(oCParticleControl**)0x008DA6C0;

	zCParser*& visualParser = *(zCParser**)0x00869E6C;
	zCParser*& s_pfxParser = *(zCParser**)0x00874380;
	fptr SearchParticleEmitter = (fptr)0x0058DEC0;
#else
	zCArraySort<zCParticleEmitter*>& s_emitterPresetList = *(zCArraySort<zCParticleEmitter*>*)0x008D8E0C;
	oCParticleControl*& pfxcGlobal = *(oCParticleControl**)0x00AB088C;


	zCParser*& visualParser = *(zCParser**)0x008CE6EC;
	zCParser*& s_pfxParser = *(zCParser**)0x008D9234;
	fptr SearchParticleEmitter = (fptr)0x005ADDE0;
#endif




	zCParticleFX* m_pPfx = NULL;
	int instanceFieldSize = 0;
	zCVob* pfxEditorVob = NULL;
	zCWorld* m_pWorld;
	CString currentPFXName;
	int cooldDownShowTimer = 0;
	const int PFX_EDITOR_CD_SHOW_TIME_MS = 30;

	//show current pfx again
	void Reload_PFX()
	{

		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();

			m_pPfx->SetAndStartEmitter(SearchParticleEmitter(currentPFXName.Upper()), FALSE);
			m_pPfx->dontKillPFXWhenDone = true;
		}
	}

	void Clear_PFXEditor()
	{
		pfxEditorVob = NULL;
		m_pPfx = NULL;
		m_pWorld = NULL;
	}

	void PlaceNearCamera_PFXEditor()
	{
		if (m_pPfx && pfxEditorVob)
		{
			zVEC3 pos = ogame->GetCamera()->connectedVob->GetAtVectorWorld() * 350 + ogame->GetCamera()->connectedVob->GetPositionWorld();
			pfxEditorVob->SetPositionWorld(pos + zVEC3(0, 100, 0));
			m_pPfx->dontKillPFXWhenDone = TRUE;
		}
	}

	void PfxEditorStopEffect()
	{
		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();
			m_pPfx->dontKillPFXWhenDone = true;
			zRELEASE(m_pPfx);
		}

		if (pfxEditorVob)
		{
			pfxEditorVob->RemoveVobFromWorld();
			zRELEASE(pfxEditorVob);
		}

		Clear_PFXEditor();
	}

	void PfxEditorApplyOnMesh()
	{
		return;

#if ENGINE > Engine_G1

		if (m_pPfx && pfxEditorVob)
		{
			if (auto pVob = theApp.GetSelectedVob()) 
			{
				if (auto pVisual = pVob->GetVisual())
				{
					zCParticleEmitter* emitter = m_pPfx->emitter;


					print.PrintRed("Visual#1");

					zCMesh* orgMesh = zDYNAMIC_CAST<zCMesh>(pVisual);
					zCProgMeshProto* orgProgMesh = zDYNAMIC_CAST<zCProgMeshProto>(pVisual);
					zCModel* orgModel = zDYNAMIC_CAST<zCModel>(pVisual);
					zCMorphMesh* orgMorph = zDYNAMIC_CAST<zCMorphMesh>(pVisual);

					if (orgMesh)
					{
						static_cast<zCParticleFX*>(m_pPfx)->emitter->shpMesh = orgMesh;
						orgMesh->AddRef();
					}
					else if (orgProgMesh)
					{
						static_cast<zCParticleFX*>(m_pPfx)->emitter->shpProgMesh = orgProgMesh;
						orgProgMesh->AddRef();
					}
					else if (orgModel)
					{
						static_cast<zCParticleFX*>(m_pPfx)->emitter->shpModel = orgModel;
						orgModel->AddRef();
					}
					else if (orgMorph)
					{
						static_cast<zCParticleFX*>(m_pPfx)->emitter->shpProgMesh = orgMorph->morphMesh;
						if (orgMorph->morphMesh) orgMorph->morphMesh->AddRef();
					}
				}
				
			}
			
		}
#endif
	}

	void CreateNewPFX()
	{
		m_pWorld = ogame->GetGameWorld();

		pfxEditorVob = new zCVob();

		pfxEditorVob->SetVobName("Vob_PFX_EditorPanel");

		pfxEditorVob->SetCollDet(FALSE);
		pfxEditorVob->dontWriteIntoArchive = TRUE;
		pfxEditorVob->ignoredByTraceRay = TRUE;

		m_pWorld->AddVob(pfxEditorVob);

		m_pPfx = new zCParticleFX();
		m_pPfx->dontKillPFXWhenDone = TRUE;
		pfxEditorVob->SetVisual(m_pPfx);

		PlaceNearCamera_PFXEditor();

	}

	

	void Loop_PFXEditor()
	{
		bool repeat = theApp.options.GetIntVal("pfxRepeatAutoplay");


		if (m_pPfx)
		{

			//fixes the code in Invk_zCVobArchive. WTF?! 
			if (theApp.GetSelectedVob() == pfxEditorVob)
			{
#if ENGINE > Engine_G1
				m_pPfx->m_bVisualNeverDies = FALSE;
#endif
			}
			if (m_pPfx->CalcIsDead())
			{
				if (repeat)
				{
					Reload_PFX();
				}
			}
		}

	}

	void SpacerApp::GetAllPfx()
	{
		int size = s_emitterPresetList.GetNumInList();

		auto addPFX = (callVoidFunc)GetProcAddress(this->module, "AddPfxInstancemName");



		for (int i = 0; i < size; i++)
		{
			Stack_PushString(s_emitterPresetList.GetSafe(i)->particleFXName);
			addPFX();
		}
	}

	void SpacerApp::UpdatePFXField()
	{
		CString name = currentPFXName;
		CString pfxFieldName = Stack_PeekString();
		pfxFieldName.Upper();


		if (!m_pPfx || !m_pPfx->emitter)
		{
			return;
		}

		zCPar_Symbol* ps = s_pfxParser->GetSymbol(s_pfxParser->GetIndex(name));

		if (!ps)
		{
			Message::Box("No PFX found with the name: " + name);
			return;
		}

		int index = s_pfxParser->GetIndex(name);

		int baseClassIndex = s_pfxParser->GetBaseClass(index);
		int indClass = s_pfxParser->GetIndex(name);

		zCPar_Symbol* base = s_pfxParser->GetSymbol(baseClassIndex);
		zCPar_Symbol* pfx = s_pfxParser->GetSymbol(indClass);

		void* addr = m_pPfx->emitter;
		int type = 0;

		if (!addr)
		{
			printWin("PFX instance pointer is NULL");
			return;
		}
		bool found = false;



		//cmd << "FieldName: " << pfxFieldName << endl;

		pfxFieldName = "C_PARTICLEFX." + pfxFieldName;

		instanceFieldSize = base->ele;
		//Message::Box("FieldsSize: " + ToStr instanceFieldSize);

		//MessageBox(0, zSTRING(base->m_sName) + zSTRING(base->f.tNumber), 0, 0);
		for (int i = 0; i < instanceFieldSize; i++)
		{
			// take the following base->f.tNumber characters, they are the instance fields
			zCPar_Symbol* param = s_pfxParser->GetSymbol(baseClassIndex + i + 1);

			if (!param)
			{
				continue;
			}

			zSTRING sName = param->name.Upper();

			//cmd << "sName: " << sName << endl;

			int pos = sName.Search(".", 1);
			//RX_FIX
			//if (pos > 0)
				//sName = param->name.Substr(pos + 1, 255);

			type = param->type;

			if (CString(sName) == pfxFieldName)
			{
				BYTE* finalAddr = (BYTE*)addr + param->GetOffset();

				if (type == zPAR_TYPE_FLOAT)
				{
					float val = Stack_PeekFloat();
					*((float*)finalAddr) = val;
					cmd << "UpdateFieldFloat: " << sName << " " << type << " " << zSTRING(val, 20) << " Addr: " << (int)finalAddr << endl;
				}
				else if (type == zPAR_TYPE_INT)
				{
					int val = Stack_PeekInt();
					*((int*)finalAddr) = val;
					cmd << "UpdateFieldInbt: " << sName << " " << type << " " << zSTRING(val) << " Addr: " << (int)finalAddr << endl;
				}
				else if (type == zPAR_TYPE_STRING)
				{
					CString val = Stack_PeekString();
					*(zSTRING*)(finalAddr) = val.ToChar();
					cmd << "UpdateFieldString: " << sName << " " << type << " " << val << " Addr: " << (int)finalAddr << endl;
				}




				if (m_pPfx && m_pPfx->emitter)
				{
					m_pPfx->emitter->UpdateInternals();
				}

				found = true;
				break;
			}
		}


		if (!found)
		{
			Message::Box("UpdateInternals fail! No field found");
		}
	}


	

	void SpacerApp::GetPFXInstanceProps(CString name)
	{
		//auto addPFXAddr = (callVoidFunc)GetProcAddress(this->module, "AddPfxAddr");

		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();
		}

		if (!m_pPfx) 
		{
			CreateNewPFX();
		}

	
		m_pPfx->SetAndStartEmitter(SearchParticleEmitter(name), FALSE);



		zCPar_Symbol* ps = s_pfxParser->GetSymbol(s_pfxParser->GetIndex(name));
		currentPFXName = name;

		if (!ps)
		{
			Message::Box("No PFX found with the name: " + name);
			return;
		}



		int index = s_pfxParser->GetIndex(name);

		int baseClassIndex = s_pfxParser->GetBaseClass(index);
		int indClass = s_pfxParser->GetIndex(name);

		zCPar_Symbol* base = s_pfxParser->GetSymbol(baseClassIndex);
		zCPar_Symbol* pfx = s_pfxParser->GetSymbol(indClass);

		void* addr = m_pPfx->emitter;
		int type = 0;

		if (!addr)
		{
			printWin("PFX instance pointer is NULL");
			return;
		}



		instanceFieldSize = base->ele;
		//Message::Box("FieldsSize: " + ToStr instanceFieldSize);

		//MessageBox(0, zSTRING(base->m_sName) + zSTRING(base->f.tNumber), 0, 0);
		for (int i = 0; i < instanceFieldSize; i++)
		{

			zCPar_Symbol* param = s_pfxParser->GetSymbol(baseClassIndex + i + 1);

			if (!param)
			{
				continue;
			}

			zSTRING sName = param->name;

			int pos = sName.Search(".", 1);
			//RX_FIX
			//if (pos > 0)
				//sName = param->name.Substr(pos + 1, 255);

			type = param->type;

			BYTE* addrVal = (BYTE*)addr + param->GetOffset();

			if (param->type == zPAR_TYPE_FLOAT)
			{
				float val = *((float*)addrVal);
				Stack_PushFloat(val);
				//cmd << sName << " " << type << " " << val << endl;
			}
			else if (param->type == zPAR_TYPE_INT)
			{
				int val = *((int*)addrVal);
				Stack_PushInt(val);

				//cmd << sName << " " << type << " " << val << endl;
			}
			else if (param->type == zPAR_TYPE_STRING)
			{
				zSTRING value = *((zSTRING*)((BYTE*)addr + param->GetOffset()));
				Stack_PushString(value);
				//cmd << sName << " " << type << " " << value << endl;
			}
			else
			{
				continue;
			}

			//Stack_PushInt((int)param->type);
			//Stack_PushString(sName);

			//prop->name = sName;
			//prop->type = type;

			//prop->addr = (BYTE*)addr + param->GetOffset();

			//Stack_PushUInt(*((BYTE*)addr + param->GetOffset()));

		}

		
		//Stack_PushInt(instanceFieldSize);
	}
}