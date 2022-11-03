
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {


	void MatManager::Init()
	{
		matSelectedInTree = NULL;
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
		//cmd << "RenderSelection " << pList.GetNumInList() << endl;

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

		//DoWork_TestRender();

		if (theApp.GetSelectedVob())
		{
			if (theApp.pickedVob) theApp.pickedVob->SetDrawBBox3D(FALSE);
			if (theApp.pickedWaypoint2nd) theApp.pickedWaypoint2nd->SetDrawBBox3D(FALSE);

			theApp.SetSelectedVob(NULL);
			theApp.pickedWaypoint2nd = NULL;
		}

		//ExtractTexture(matSelectedInTree);
		RenderSelection();
	}


	void MatManager::OnSelectInMatFilter(zCMaterial* mat)
	{
		mf.OnSelectInMatFilter(mat);
	}
	void MatManager::OnSelectMaterial(zCMaterial* mat)
	{
		if (mat)
		{
			//cmd << "OnSelectMaterial" << endl;
			static auto onSelect = (onSelectVob)GetProcAddress(theApp.module, "OnSelectMaterial");
			onSelect((int)mat);

			OnSelectInMatFilter(mat);
			ExtractTexture(mat);
		}
	}

	void MatManager::ExtractTexture(zCMaterial* mat)
	{
		if (!mat) return;

		if (auto pTex = mat->texture)
		{
			
			mf.Render(mat);

		}
		else
		{
			zCOLOR col = mat->color;

			Stack_PushInt(col.r);
			Stack_PushInt(col.g);
			Stack_PushInt(col.b);
			theApp.exports.MatFilter_SetTextureColor();

			//cmd << col.GetDescriptionRGB() << endl;

		}

	}
	void MatManager::SelectMaterial(zCMaterial* mat)
	{
		matSelectedInTree = mat;

		zCArchiver* arch = zarcFactory->CreateArchiverWrite(zARC_MODE_ASCII_PROPS, FALSE, 0);

		arch->SetStringEOL(zSTRING("\n"));
		arch->WriteObject(matSelectedInTree);
		zSTRING arcString;
		arch->GetBufferString(arcString);

		theApp.SetProperties(arcString, "zCMaterial");
		arch->Close();
		zRELEASE(arch);
		//cmd << "SelectMaterial" << endl;

		ExtractTexture(mat);
		//OnSelectMaterial(mat);
	}

	void MatManager::CreateNewSelection(zCPolygon* xPoly)
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


			matSelectedInTree = newSelection->GetMaterial();
			OnSelectMaterial(newSelection->GetMaterial());

			//cmd << "CreateNewSelection: End " << Z(int) xPoly << endl;
		}

	}

	zBOOL __fastcall zCWorld::PickScene_Material(zCCamera& cam, int xscr, int yscr, zREAL rayLength)
	{
		// Sucht den naechstliegendsten Hit.
		const zVALUE RAY_DIST = 600000.0F;
		// alle Vobs in der räumlichen Nähe durchlaufen.
		// - check, ob vob-center im Frustum ?
		// - check, ob vob-bbox/sphere im Frustum ?
		// - check, ob ray die bbox/sphere schneidet ?
		// - check, ob ray exakt ein vob-Poly trifft ?

		cam.Activate();									// FIXME: checken, welche 'activate' Aktionen wirklich noetig sind !!

		zPOINT3 ray00, ray, p;
		// create ray00, ray by backprojection
		// ray00, ray sind im world(obj)Space
		// 'ray00	= cam.camMatrixInv * zPOINT3(0,0,0);'  =
		cam.camMatrixInv.GetTranslation(ray00);
		p.n[VZ] = RAY_DIST;
		cam.BackProject(xscr, yscr, p);				// p im camSpace
		p = cam.camMatrixInv * p;					// p im world(obj)Space  
		ray = p - ray00;

		// wuenscht der Benutzer eine bestimmte Ray-Laenge?
		if (rayLength>0)
		{
			ray.Normalize();
			ray *= rayLength;
		};

		traceRayReport.Clear();
		traceRayIgnoreVobFlag = TRUE;
		int	traceFlags = zTRACERAY_STAT_POLY |
			zTRACERAY_POLY_TEST_WATER | zTRACERAY_VOB_IGNORE;


		const zBOOL hit = TraceRayNearestHit(ray00, ray, (zCVob*)0, traceFlags);
		traceRayIgnoreVobFlag = FALSE;

		if (hit)
			if (traceRayReport.foundPoly)
				if (!traceRayReport.foundVob)
				{
					// foundVertex, only static geom
					// nearest vertex finden
					zCPolygon	*poly = traceRayReport.foundPoly;
					zREAL		best = zREAL_MAX;
					for (int i = 0; i<poly->polyNumVert; i++)
					{
						zCVertex	*vert = poly->GetVertexPtr(i);
						zREAL		dist = (vert->position - traceRayReport.foundIntersection).Length_Sqr();
						if (dist<best) {
							best = dist;
							traceRayReport.foundVertex = vert;
						};
					};
				};

		return hit;
	};

	void MatManager::OnPick(float ax, float ay)
	{
		auto world = ogame->GetWorld();

		if (!world) return;

		//print.PrintRed(zSTRING(ax, 20) + ";" + zSTRING(ay, 20));

		/*
		float ax = screen->anx(pickTryEntry.ax);
		float ay = screen->any(pickTryEntry.ay);
		
		*/

		world->PickScene_Material(*ogame->GetCamera(), ax, ay, 600000);

		auto xPoly = world->traceRayReport.foundPoly;
		auto xVert = world->traceRayReport.foundVertex;
		
		auto selection = GetCurrentSelection();

		zCPolygon* currentPoly = NULL;

		//print.PrintRed(Z(int)xPoly);
		//print.PrintRed(Z(int) world->traceRayReport.foundVob);

		if (selection)
		{

			//cmd << "Selection exist!" << endl;
			currentPoly = selection->GetPolygon();
		}


		CleanSelection();


		if (xPoly && (xPoly != currentPoly || currentPoly == NULL))
		{
			//cmd << "CreateNewSelection!" << endl;
			CreateNewSelection(xPoly);
		}

		//cmd << "OnPick End!" << endl;

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

		matSelectedInTree = NULL;
		

		auto AddProps = (callVoidFunc)GetProcAddress(theApp.module, "AddProps");

		Stack_PushString("");
		Stack_PushString("");

		AddProps();

	}

}