// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	zCParticleFX* m_pPfx = NULL;
	int instanceFieldSize = 0;
	zCVob* pfxEditorVob = NULL;
	zCWorld* m_pWorld;
	CString currentPFXName;
	int cooldDownShowTimer = 0;
	const int PFX_EDITOR_CD_SHOW_TIME_MS = 30;
	SpacerPfxMotion pfxMotionType = SpacerPfxMotion::SPACER_PFX_MOTION_TYPE_STATIC;

	struct
	{
		zVEC3 finalForwPos;
		zVEC3 forwVel;
		zVEC3 oldCamPos;

		int forwDistMax = 800;
		bool forward = true;

		float radius = 300;
		float w = 0;
		zVEC3 center;
	} moveStruct;

	void PlaceNearCamera_PFXEditor();

	//show current pfx again
	void Reload_PFX()
	{

		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();

			m_pPfx->SetAndStartEmitter(zCParticleFX::SearchParticleEmitter(currentPFXName.Upper()), FALSE);
			m_pPfx->dontKillPFXWhenDone = true;
		}
	}

	

	void Clear_PFXEditor()
	{
		pfxEditorVob = NULL;
		m_pPfx = NULL;
		m_pWorld = NULL;
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

#if ENGINE >= Engine_G2A

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

	
	void SetMotionTypePFX(int type)
	{
		pfxMotionType = (SpacerPfxMotion)type;

		if (!pfxEditorVob || !ogame || !ogame->GetWorld() || !ogame->GetCameraVob() || !m_pPfx || !m_pPfx->emitter)
		{
			return;
		}

		auto pos = ogame->GetCameraVob()->GetPositionWorld();
		auto dir = ogame->GetCameraVob()->GetAtVectorWorld();

		if (pfxMotionType == SPACER_PFX_MOTION_TYPE_FORW)
		{
			
			moveStruct.finalForwPos = pos + dir * moveStruct.forwDistMax + zVEC3(0, 150, 0);
			moveStruct.forwVel = dir;
			moveStruct.oldCamPos = pos + dir * 350;
			moveStruct.forward = true;

			//theApp.debug.AddLine(moveStruct.oldCamPos, moveStruct.finalForwPos, GFX_RED, 15000);
		}
		else if (pfxMotionType == SPACER_PFX_MOTION_TYPE_CIRCLE)
		{
			moveStruct.center = pos + dir * 500;
			moveStruct.radius = m_pPfx->emitter->shpCircleSphereRadius > 1 ? m_pPfx->emitter->shpCircleSphereRadius * 10 : 150;
		}
		else if (pfxMotionType == SPACER_PFX_MOTION_TYPE_STATIC)
		{
			pfxEditorVob->SetPositionWorld(pos + dir * 350);
		}
	}

	void PlaceNearCamera_PFXEditor()
	{
		if (m_pPfx && pfxEditorVob)
		{
			zVEC3 pos = ogame->GetCamera()->connectedVob->GetAtVectorWorld() * 350 + ogame->GetCamera()->connectedVob->GetPositionWorld();
			
			m_pPfx->dontKillPFXWhenDone = TRUE;

			SetMotionTypePFX(pfxMotionType);

			if (pfxMotionType == SPACER_PFX_MOTION_TYPE_STATIC)
			{
				pfxEditorVob->SetPositionWorld(pos + zVEC3(0, 100, 0));
			}
			else
			{
				pfxEditorVob->SetPositionWorld(pos);
			}
			
		}
	}

	void Loop_PFXEditor()
	{
		bool repeat = theApp.options.GetIntVal("pfxRepeatAutoplay");


		if (m_pPfx)
		{
			// moving PFX vob
			if (pfxEditorVob)
			{
				auto dt = ztimer->frameTimeFloat / 1000.0f;
				auto speed = theApp.options.GetIntVal("intPfxEditorSpeedMotion");

				if (pfxMotionType == SPACER_PFX_MOTION_TYPE_ROTATE_LOCAL_Y)
				{
					pfxEditorVob->RotateLocalY(150 * (speed / 100.0f) * dt);
				}
				else if (pfxMotionType == SPACER_PFX_MOTION_TYPE_FORW)
				{
					auto speedTrans = moveStruct.forwVel * (300.0f * speed / 100.0f);

					auto newPos = pfxEditorVob->GetPositionWorld() + speedTrans * dt;

					if (moveStruct.forward)
					{
						if (newPos.Distance(moveStruct.oldCamPos) >= moveStruct.forwDistMax)
						{
							moveStruct.forward = false;
							moveStruct.forwVel *= -1;
						}
					}
					else
					{
						if (newPos.Distance(moveStruct.oldCamPos) <= 50)
						{
							moveStruct.forward = true;
							moveStruct.forwVel *= -1;
						}
					}

					pfxEditorVob->SetPositionWorld(newPos);
					//theApp.debug.AddLine(newPos, newPos + zVEC3(0, 100, 0), GFX_BLUE, 2000);
				}
				else if (pfxMotionType == SPACER_PFX_MOTION_TYPE_CIRCLE)
				{
					auto w = PI / 2 * (speed / 100.0f);
					moveStruct.w += w * dt;

					if (moveStruct.w > PI2)
					{
						moveStruct.w -= PI2;
					}

					float x, z;

					x = moveStruct.radius * cos(moveStruct.w);
					z = moveStruct.radius * sin(moveStruct.w);

					//cmd << x << " " << z << endl;

					auto newPos = moveStruct.center + zVEC3(x, 0, z);

					pfxEditorVob->SetPositionWorld(newPos);
				}
			}
			//fixes the code in Invk_zCVobArchive. WTF?! 
			if (theApp.GetSelectedVob() == pfxEditorVob)
			{
#if ENGINE >= Engine_G2
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
		int size = zCParticleFX::s_emitterPresetList.GetNumInList();

		auto addPFX = (callVoidFunc)GetProcAddress(this->module, "AddPfxInstancemName");



		for (int i = 0; i < size; i++)
		{
			Stack_PushString(zCParticleFX::s_emitterPresetList.GetSafe(i)->particleFXName);
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

		zCPar_Symbol* ps = zCParticleFX::s_pfxParser->GetSymbol(zCParticleFX::s_pfxParser->GetIndex(name));

		if (!ps)
		{
			Message::Box("No PFX found with the name: " + name);
			return;
		}

		int index = zCParticleFX::s_pfxParser->GetIndex(name);

		int baseClassIndex = zCParticleFX::s_pfxParser->GetBaseClass(index);
		int indClass = zCParticleFX::s_pfxParser->GetIndex(name);

		zCPar_Symbol* base = zCParticleFX::s_pfxParser->GetSymbol(baseClassIndex);
		zCPar_Symbol* pfx = zCParticleFX::s_pfxParser->GetSymbol(indClass);

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
			zCPar_Symbol* param = zCParticleFX::s_pfxParser->GetSymbol(baseClassIndex + i + 1);

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

	
		m_pPfx->SetAndStartEmitter(zCParticleFX::SearchParticleEmitter(name), FALSE);



		zCPar_Symbol* ps = zCParticleFX::s_pfxParser->GetSymbol(zCParticleFX::s_pfxParser->GetIndex(name));
		currentPFXName = name;

		if (!ps)
		{
			Message::Box("No PFX found with the name: " + name);
			return;
		}



		int index = zCParticleFX::s_pfxParser->GetIndex(name);

		int baseClassIndex = zCParticleFX::s_pfxParser->GetBaseClass(index);
		int indClass = zCParticleFX::s_pfxParser->GetIndex(name);

		zCPar_Symbol* base = zCParticleFX::s_pfxParser->GetSymbol(baseClassIndex);
		zCPar_Symbol* pfx = zCParticleFX::s_pfxParser->GetSymbol(indClass);

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

			zCPar_Symbol* param = zCParticleFX::s_pfxParser->GetSymbol(baseClassIndex + i + 1);

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