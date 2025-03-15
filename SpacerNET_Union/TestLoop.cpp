// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .



	void ScaleVob(zCVob* pVob, float scaleFactor)
	{
		zMAT4& trafo = pVob->GetNewTrafoObjToWorld();


		//trafo.MakeIdentity();
		//trafo.PreScale(0.97f);
		/*
		auto rot = trafo.ExtractRotation();

		trafo.ResetRotation();
		trafo.PostScale(scaleFactor);
		trafo *= rot;
		*/


		cmd << "PRE: " << trafo.ToString().c_str() << endl;

		print.PrintGreen("SCALE");
		trafo.PostScaleNew(scaleFactor);

		pVob->SetPositionWorld(pVob->GetPositionWorld());

		cmd << "AFTER: " << trafo.ToString().c_str() << endl;
	}

	// test loop, some test code always here...
	void SpacerApp::TestLoop()
	{
		/*if (auto pVob = GetSelectedVob())
		{
			if (zinput->KeyPressed(KEY_F1))
			{
				ScaleVob(pVob, 0.95f);
			}

			if (zinput->KeyPressed(KEY_F2))
			{
				ScaleVob(pVob, 1.10f);
			}
		}*/
		return;
	}
	
}