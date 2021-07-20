
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {


	void MatManager::Init()
	{
		selPolyList = NULL;
		pickedPoly = NULL;
		SelectMaterial = NULL;
		selectedVertext = NULL;
		copyMat = NULL;


		zSTRING filename("select.tga");
		SelectMaterial = new zCMaterial("<Select>");
		SelectMaterial->SetTexture(filename);
		SelectMaterial->color.r = 255;
		SelectMaterial->color.g = 0;
		SelectMaterial->color.b = 0;
		SelectMaterial->color.alpha = 255;
		SelectMaterial->AddRef();

		SelectMaterial->SetUsage(zCMaterial::zMAT_USAGE_OTHER);
		
		SelectMaterial->rndAlphaBlendFunc = zRND_ALPHA_FUNC_MUL;

		
		selPolyList = new zCList<zCSelPoly>;
		selPolyList->DeleteList();

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

		if (zrenderer) zrenderer->DrawLine((float)x1, (float)y1, (float)x2, (float)y2, col);
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

			for (int i = 0; i < selPolyList->GetNumInList(); i++)
			{
				zCMaterial* mat = selPolyList->Get(i)->GetMaterial();
				if (mat)
				{
					PrintDebug("Material: " + mat->GetName());

					if (mat->texture)
					{
						PrintDebug("Texture: " + mat->texture->GetObjectName());
					}


					PrintDebug("MatGroup: " + mat->GetMatGroupString());

				}
			}

			

		}
	}

	void MatManager::PolyApplyMapping()
	{
		zCSelPoly* selPoly = NULL;
		int count = selPolyList->GetNumInList();
		if (count == 0) return;

		zCList<zCPolygon> polylist;

		for (int i = 0; i<count; i++)
		{
			selPoly = selPolyList->Get(i);
			if (selPoly) polylist.Insert(selPoly->GetPolygon());
		}
		// now apply planar mapping to all polygons in generated list
		float x, y;
		x = 1;
		y = 1;

		zCPolygon::TexApplyPlanarMapping(polylist, 1, 1, 1);

	}

	void MatManager::OnPolyApplyTexture()
	{
		zCMaterial* mat = this->copyMat;
		if (!mat) return;

		// iterate all selected polygons and apply
		zCSelPoly* selPoly = NULL;
		int count = selPolyList->GetNumInList();
		for (int i = 0; i<count; i++)
		{
			selPoly = selPolyList->Get(i);
			if (selPoly) selPoly->SetMaterial(mat);
		}
	}

	void MatManager::CalcPolyCenter(zPOINT2& center)
	{
		if (!selPolyList) return;

		zVALUE uSum = 0;
		zVALUE vSum = 0;
		int count = selPolyList->GetNumInList();
		zCSelPoly* selPoly = NULL;
		int num = 0;
		for (int i = 0; i<count; i++)
		{
			selPoly = selPolyList->Get(i);
			if (selPoly == NULL) continue;
			if (selPoly->GetPolygon() == NULL) continue;
			for (int j = 0; j<selPoly->GetPolygon()->polyNumVert; j++)
			{
				uSum += selPoly->GetPolygon()->feature[j]->texu;
				vSum += selPoly->GetPolygon()->feature[j]->texv;
				num++;
			}
		}
		center.n[VX] = uSum / num;
		center.n[VY] = vSum / num;
	}


	void MatManager::ResetUV()
	{
		int count = selPolyList->GetNumInList();

		if (count == 0) return;

		zVEC2 center;
		CalcPolyCenter(center);
		zCSelPoly* selPoly = NULL;


		zCList<zCPolygon> polyList;

		for (int i = 0; i<count; i++)
		{
			selPoly = selPolyList->Get(i);
			if (selPoly && selPoly->GetPolygon())
			{
				polyList.Insert(selPoly->GetPolygon());
				
			}

		}

		zCPolygon::TexApplyPlanarMapping(polyList, 1, 1, 1);

		//selPoly->GetPolygon()->TexApplyPlanarMapping(1, 1, 1);
		//selPoly->GetPolygon()->material->ApplyTexAniMapping(selPoly->GetPolygon());
	}

	void MatManager::TextureScale(zVALUE x, zVALUE y)
	{
		int grid = 1;

		int count = selPolyList->GetNumInList();
		if (count == 0) return;
		x = zVALUE(-int(x) * grid);
		y = zVALUE(-int(y) * grid);
		x /= 800; y /= 800;

		x += 1; y += 1;

		zVEC2 center;
		CalcPolyCenter(center);
		zCSelPoly* selPoly = NULL;
		for (int i = 0; i<count; i++)
		{
			selPoly = selPolyList->Get(i);
			if (selPoly && selPoly->GetPolygon())
			{
				selPoly->GetPolygon()->TexScale(center, x, y);
				
			}

		}
	}

	void MatManager::Loop() 
	{
		if (selPolyList && selPolyList->GetNum() > 0)
		{
			for (int i = 0; i < selPolyList->GetNumInList(); i++) {

				auto poly = selPolyList->Get(i)->GetPolygon();

				zVEC3 pos0 = poly->vertex[0]->position;
				zVEC3 pos1 = poly->vertex[1]->position;
				zVEC3 pos2 = poly->vertex[2]->position;

				this->DrawLine(pos0, pos1, GFX_RED);
				this->DrawLine(pos1, pos2, GFX_RED);
				this->DrawLine(pos2, pos0, GFX_RED);

			}


			if (zinput->GetMouseButtonPressedRight())
			{

				return;
			}
			




			zVEC2 center;

			CalcPolyCenter(center);

			for (int i = 0; i < selPolyList->GetNumInList(); i++) {

				auto poly = selPolyList->Get(i)->GetPolygon();

				float spd = 0.0001;
				float angle = 1.0f / 60.0f;

				if (keys.KeyPressed("VOB_SPEED_X10", false, true))
				{
					spd *= 10;
					angle *= 10;
				}


				if (keys.KeyPressed("VOB_SPEED_MINUS_10", false, true))
				{
					spd /= 10;
					angle /= 10;
				}
				



				if (keys.KeyPressed("VOB_TRANS_RIGHT", false, true))
				{
					poly->TexTranslate(zVEC2(-spd, 0));
				}

				if (keys.KeyPressed("VOB_TRANS_LEFT", false, true))
				{
					poly->TexTranslate(zVEC2(spd, 0));
				}


				if (keys.KeyPressed("VOB_TRANS_FORWARD", false, true))
				{
					poly->TexTranslate(zVEC2(0, -spd));
				}

				if (keys.KeyPressed("VOB_TRANS_BACKWARD", false, true))
				{
					poly->TexTranslate(zVEC2(0, spd));
				}



				if (keys.KeyPressed("VOB_ROT_VERT_RIGHT", false, true))
				{
					poly->TexRotate(center, angle);
				}

				if (keys.KeyPressed("VOB_ROT_VERT_LEFT", false, true))
				{
					poly->TexRotate(center, -angle);
				}
			}

			
			

			


			
		}
	}

	bool MatManager::PolySelect(bool select, zCSelPoly* selPoly)
	{
		if (!selPolyList) return FALSE;
		if (!selPoly || !selPoly->GetPolygon()) return FALSE;

		if (select)
		{
			if (!selPoly->GetMaterial()) return TRUE;
			//print.PrintRed(info);

			//selPoly->GetPolygon()->SetMaterial(SelectMaterial);
			selPolyList->Insert(selPoly);
			//SelectMaterial->libFlag = MATLIB_INVISIBLE;
			//SelectMaterial->SetUsage(zCMaterial::zMAT_USAGE_OTHER);
			return TRUE;
		}
		else
		{
			if (!selPoly->GetMaterial()) return TRUE;
			selPolyList->Remove(selPoly);
			delete selPoly;
			return TRUE;
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


		if (xPoly)
		{
			theApp.SetSelectedVob(NULL, "");

			zCSelPoly* selPoly = PolyIsSelected(xPoly);

			if (selPoly) // полигон уже выбран
			{
				selPoly->poly->material = selPoly->mat;

				if (PolySelect(FALSE, selPoly))
				{
					
				}
			}
			else					
			{
				selPoly = new zCSelPoly(xPoly);

				selPoly->mat = xPoly->material;


				PolySelect(TRUE, selPoly);
				

			}

		}
	}

	void MatManager::RestoreMat()
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