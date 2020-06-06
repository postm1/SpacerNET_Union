// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct PFXManager
	{
		// рисуемый эффект
		zCParticleFX* m_pPfx;

		// воб-родитель на котором рисуется эффект
		zCVob* testVob;

		zCWorld* m_pWorld;


		void Loop();
		void RemovePFX();
		void RenderPFX(zSTRING);

		PFXManager::PFXManager()
		{
			m_pPfx = NULL;
			testVob = NULL;
			m_pWorld = NULL;
		}
	};
}