
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {


	void MatManager::Init()
	{
		selPolyList = NULL;
		pickedPoly = NULL;
		SelectMaterial = NULL;
		selectedVertext = NULL;


		zSTRING filename("select.tga");
		SelectMaterial = new zCMaterial("<Select>");
		SelectMaterial->SetTexture(filename);
		SelectMaterial->color.r = 225;
		SelectMaterial->color.g = 49;
		SelectMaterial->color.b = 101;
		SelectMaterial->AddRef();
		SelectMaterial->SetUsage(zCMaterial::zMAT_USAGE_OTHER);

		
		selPolyList = new zCList<zCSelPoly>;
		selPolyList->DeleteList();

	}

	bool MatManager::PolySelect(bool select, zCSelPoly* selPoly)
	{


		if (!selPolyList) return FALSE;
		if (!selPoly || !selPoly->GetPolygon()) return FALSE;

		if (select)
		{
			if (!selPoly->GetMaterial()) return TRUE;
			//print.PrintRed(info);

			selPoly->GetPolygon()->SetMaterial(SelectMaterial);
			selPolyList->Insert(selPoly);
			SelectMaterial->libFlag = MATLIB_INVISIBLE;
			SelectMaterial->SetUsage(zCMaterial::zMAT_USAGE_OTHER);
			return TRUE;
		}
		else
		{
			if (!selPoly->GetMaterial()) return TRUE;
			selPoly->GetPolygon()->SetMaterial(selPoly->GetMaterial());
			selPolyList->Remove(selPoly);
			SelectMaterial->libFlag = MATLIB_INVISIBLE;
			SelectMaterial->SetUsage(zCMaterial::zMAT_USAGE_OTHER);
			return TRUE;
		}
	}


	void MatManager::CopyTextureName()
	{
		if (selPolyList && selPolyList->GetNum() > 0)
		{

			zCMaterial* mat = selPolyList->Get(0)->GetMaterial();
			if (mat)
			{
				//PrintDebug("Material: " + mat->GetName());

				if (mat->texture)
				{
					CopyClipBoard(mat->texture->GetObjectName());
					print.PrintRed("Имя текстуры скопировано");
				}

			}

		}
	}

	void MatManager::DrawLine(zVEC3 wsPoint1, zVEC3 wsPoint2, zCOLOR col)
	{

		auto cam = ogame->GetCamera();

		zPOINT3 csPoint1, csPoint2;	// world/camera-space
		zPOINT2 ssPoint1, ssPoint2;						// screen-space X,Y


		csPoint1 = cam->Transform(wsPoint1);
		csPoint2 = cam->Transform(wsPoint2);
		Alg_ClipAtZ0(csPoint1, csPoint2);

		if (csPoint1[VZ] >= 0)
		{
			cam->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
		}
		else return;

		if (csPoint2[VZ] >= 0)
		{
			cam->Project(&csPoint2, ssPoint2[VX], ssPoint2[VY]);
		}
		else return;

		float camSpaceInf1 = (csPoint1[VZ] != 0 ? 1 / csPoint1[VZ] : 0);
		float camSpaceInf2 = (csPoint1[VZ] != 0 ? 1 / csPoint2[VZ] : 0);

		int x1 = (int)ssPoint1[VX];
		int y1 = (int)ssPoint1[VY];
		int x2 = (int)ssPoint2[VX];
		int y2 = (int)ssPoint2[VY];
		if (!screen->ClipLine(x1, y1, x2, y2)) return;

		if (zrenderer) zrenderer->DrawLineZ((float)x1, (float)y1, camSpaceInf1, (float)x2, (float)y2, camSpaceInf2, col);
	}

	void MatManager::DrawAxes()
	{
		if (selPolyList && selPolyList->GetNum() > 0)
		{
			auto cam = ogame->GetCamera();
			auto poly = selPolyList->Get(0)->GetPolygon();
			poly->CalcNormal();

			zVEC3 norm = poly->polyPlane.normal;
			zVEC3 center = poly->GetCenter() + norm * 35;

			zVEC3 pointCenter = center;
			zVEC3 pointY = center + norm * 150;
			zVEC3 pointX = center + norm * zVEC3(0, 0, 1) * 150;
			zVEC3 pointZ = center + norm * zVEC3(-1, 0, 0) * 150;

			
			DrawLine(pointCenter, pointX, zCOLOR(0, 255, 0));
			DrawLine(pointCenter, pointY, zCOLOR(255, 255, 255));
			DrawLine(pointCenter, pointZ, zCOLOR(0, 0, 255));
		}

	}
	void MatManager::ShowInfo()
	{
		if (selPolyList && selPolyList->GetNum() > 0)
		{

			zCMaterial* mat = selPolyList->Get(0)->GetMaterial();
			if (mat)
			{
				PrintDebug("Material: " + mat->GetName());

				if (mat->texture)
				{
					PrintDebug("Texture: " + mat->texture->GetObjectName());
				}

			}

		}
	}


	void MatManager::OnPick(float ax, float ay)
	{

		//print.PrintRed("MAtPick");
		auto world = ogame->GetWorld();

		ogame->GetWorld()->PickScene(*ogame->GetCamera(), ax, ay, -1);

		auto xPoly = world->traceRayReport.foundPoly;
		auto xVert = world->traceRayReport.foundVertex;

		if (xVert)
		{
			selectedVertext = xVert;
		}
		//else 
		if (xPoly)
		{
			zCSelPoly* selPoly = PolyIsSelected(xPoly);

			if (selPoly)				// polygon is selected ---> deselect
			{

				if (PolySelect(FALSE, selPoly))
				{
					delete selPoly;
					selPoly = NULL;
				}

			}
			else					// polygon is not selected ---> delect
			{
				if (selPolyList->GetNumInList() > 0)
				{
					PolySelect(FALSE, selPolyList->Get(0));
				}

				selPoly = new zCSelPoly(xPoly);
				PolySelect(TRUE, selPoly);
			}

		}
	}

	void MatManager::CleanSelectMaterial()
	{
		if (!selPolyList)
		{
			return;
		}

		for (int i = 0; i < selPolyList->GetNumInList(); i++)
		{
			auto selPoly = selPolyList->Get(i);

			if (selPoly)
			{
				if (!selPoly->GetMaterial()) continue;

				selPoly->GetPolygon()->SetMaterial(selPoly->GetMaterial());
				//selPolyList->Remove(selPoly);
				SelectMaterial->libFlag = MATLIB_INVISIBLE;
				SelectMaterial->SetUsage(zCMaterial::zMAT_USAGE_OTHER);
				delete selPoly;
			}

		}

		selPolyList->DeleteList();
	}


	zCSelPoly* MatManager::PolyIsSelected(zCPolygon * xPoly)
	{
		/*
		int count = selPolyList->GetNumInList();
		zCSelPoly* selPoly = NULL;
		for (int i=0;i<count;i++)
		{
		selPoly = selPolyList->Get(i);
		if (!selPoly || (selPoly->GetPolygon() == xPoly)) break;
		}
		if (selPoly && (selPoly->GetPolygon()!=xPoly)) selPoly = NULL;
		return selPoly;
		*/
		if (!selPolyList) return 0;

		if (selPolyList->GetNumInList() == 0) return 0;

		zCList<zCSelPoly>* lList = selPolyList->GetNextInList();
		while (lList && lList->GetData()->GetPolygon() != xPoly)
			lList = lList->GetNextInList();
		if (lList) return lList->GetData();
		else return 0;

	}
	// Add your code here . . .
}