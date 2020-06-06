// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void PFXManager::Loop()
	{
		if (testVob && m_pPfx && m_pWorld)
		{
			zCVob* camVob = ogame->GetCamera()->GetVob();

			zVEC3 pos = (camVob->GetPositionWorld()
				- camVob->GetAtVectorWorld().Cross(zVEC3(0, -1, 0) * 150)
				+ camVob->GetAtVectorWorld() * 200
				+ zVEC3(0, 25, 0)
				);


			testVob->SetPositionWorld(pos);
		}
	}

	void PFXManager::RemovePFX()
	{
		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();
			zRELEASE(m_pPfx);
		}

		if (testVob)
		{
			testVob->RemoveVobFromWorld();
			zRELEASE(testVob);
		}

		m_pWorld = NULL;

	}

	void PFXManager::RenderPFX(zSTRING visual)
	{

		if (!m_pWorld)
		{
			m_pWorld = ogame->GetGameWorld();
		}


		if (m_pPfx)
		{
			m_pPfx->StopEmitterOutput();
			m_pPfx->emitter->ppsIsLooping = FALSE;
			m_pPfx->dontKillPFXWhenDone = false;
			zRELEASE(m_pPfx);
			testVob->SetVisual(0);
		}

		if (!testVob)
		{
			testVob = new zCVob();
			testVob->SetVobName("Vob_PFX_Editor");
			m_pWorld->AddVob(testVob);
			testVob->SetCollDet(FALSE);
			testVob->dontWriteIntoArchive = TRUE;
			testVob->ignoredByTraceRay = TRUE;
		}


		zVEC3 pos = ogame->GetCamera()->GetVob()->GetAtVectorWorld() * 200 + ogame->GetCamera()->GetVob()->GetPositionWorld();
		testVob->Move(pos[0], pos[1], pos[2]);


		m_pPfx = new zCParticleFX();
		m_pPfx->SetEmitter(zCParticleFX::SearchParticleEmitter(visual), FALSE);
		testVob->SetVisual(m_pPfx);



		if (testVob && m_pPfx && m_pPfx->emitter)
		{
			m_pPfx->dontKillPFXWhenDone = true;
			m_pPfx->SetAndStartEmitter(zCParticleFX::SearchParticleEmitter(visual.Upper()), FALSE);

		}
	}
}