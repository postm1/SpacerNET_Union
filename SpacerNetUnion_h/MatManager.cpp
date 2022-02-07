
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {


	void MatManager::Init()
	{

	}

	bool MatManager::IsMaterialSelected()
	{
		return pList.GetNum() > 0;
	}

	zCSelPoly* MatManager::GetCurrentSelection()
	{
		if (IsMaterialSelected())
		{
			return pList.Get(0);
		}

		return NULL;
	}

	void MatManager::RenderSelection()
	{
		auto selection = GetCurrentSelection();

		if (selection)
		{
			auto poly = selection->GetPolygon();

			if (poly)
			{
				zVEC3 pos0 = poly->vertex[0]->position;
				zVEC3 pos1 = poly->vertex[1]->position;
				zVEC3 pos2 = poly->vertex[2]->position;


				zlineCache->Line3D(pos0, pos1, GFX_RED, 0);
				zlineCache->Line3D(pos1, pos2, GFX_RED, 0);
				zlineCache->Line3D(pos2, pos0, GFX_RED, 0);
			}
		}
	}

	void MatManager::Loop() 
	{
		auto pickMode = theApp.GetPickMode();

		if (pickMode != SWM_MATERIALS)
		{
			return;
		}

		if (theApp.GetSelectedVob())
		{
			if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
			if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

			theApp.SetSelectedVob(NULL);
			theApp.pickedWaypoint2nd = NULL;
		}

		RenderSelection();
	}


	void MatManager::OnPick(float ax, float ay)
	{
		auto world = ogame->GetWorld();

		if (!world) return;

		world->PickScene(*ogame->GetCamera(), ax, ay, -1);

		auto xPoly = world->traceRayReport.foundPoly;
		auto xVert = world->traceRayReport.foundVertex;
		
		auto selection = GetCurrentSelection();

		zCPolygon* currentPoly = NULL;

		if (selection)
		{
			currentPoly = selection->GetPolygon();
		}


		CleanSelection();


		if (xPoly && xPoly != currentPoly)
		{
			zCSelPoly* newSelection = new zCSelPoly(xPoly);

			if (newSelection)
			{


				pList.Insert(newSelection);




				zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, FALSE, 0);

				arch->SetStringEOL(zSTRING("\n"));
				arch->WriteObject(newSelection->GetMaterial());
				zSTRING arcString;
				arch->GetBufferString(arcString);

				theApp.SetProperties(arcString, "zCMaterial");
				arch->Close();
				zRELEASE(arch);


			}
		}

		zinput->ClearKeyBuffer();
		zinput->ClearLeftMouse();

	}


	void MatManager::CleanSelection()
	{
		auto selection = GetCurrentSelection();



		if (selection)
		{
			pList.Remove(selection);
			delete selection;
		}
		

		auto AddProps = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

		Stack_PushString("");
		Stack_PushString("");

		AddProps();

	}

}