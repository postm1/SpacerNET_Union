// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void SpacerApp::CleanTestLoop()
	{
		
	}

	zVEC3 GetFocus(zCVob* vob)
	{
		return vob->bbox3D.GetCenter() + (vob->bbox3D.GetCenter() - vob->bbox3D.GetCenterFloor()) * 0.5f;
	}


	bool IsBadVob(zCVob* vob)
	{
		const float diffMargin = 30;
		const int offsetPoint0 = 100;


		bool vobCheck = vob && vob == theApp.GetSelectedVob();
		auto wld = ogame->GetWorld();

		zVEC3 point0 = vob->GetPositionWorld() + zVEC3(0, offsetPoint0, 0);
		zVEC3 inter0 = point0;
		float heightDiff0 = 0.0f;
		bool foundPoint0 = false;

		wld->TraceRayNearestHit(point0, zVEC3(0, -500, 0), vob,
			zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER | zTRACERAY_VOB_IGNORE);

		if (wld->traceRayReport.foundHit)
		{
			inter0 = wld->traceRayReport.foundIntersection;
			heightDiff0 = point0.Distance(inter0);
			foundPoint0 = true;
		}


		zVEC3 point1 = vob->GetPositionWorld();
		zVEC3 inter1 = point1;
		float heightDiff1 = 0.0f;
		bool foundPoint1 = false;

		wld->TraceRayNearestHit(point1, zVEC3(0, -500, 0), vob,
			zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER | zTRACERAY_VOB_IGNORE);

		if (wld->traceRayReport.foundHit)
		{
			inter1 = wld->traceRayReport.foundIntersection;

			heightDiff1 = point1.Distance(inter0);
			foundPoint1 = true;

			
		}

		if (vobCheck)
		{
			PrintDebug("Diff: " + zSTRING(heightDiff1, 20));
			//PrintDebug("Height: " + zSTRING(point1.n[1], 20) + "; " + zSTRING(inter1.n[1], 20));
		}

		if (foundPoint0 && foundPoint1 && inter1.Distance(inter0) <= 0.5f)
		{
			if (heightDiff1 > diffMargin)
			{
				return true;
			}
		}

		

		


		return false;
	}

	void SpacerApp::TestLoop()
	{
		return;

		if (zKeyToggled(KEY_F1))
		{
			zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
			int numPolys = ogame->GetWorld()->bspTree.numPolys;

			zCArray<CString> texturesFound;


			print.PrintRed("Search");

		

			for (int i = 0; i < numPolys; i++)
			{
				zCPolygon* poly = trisList[i];

				//if (poly->flags.portalPoly != 0) continue;

				auto mat = poly->material;

				if (i % 1000 == 0)
				{
					//cmd << "i = " << i << endl;
				}

				if (mat)
				{
					if (mat->texture)
					{
						CString name = mat->texture->GetObjectName();

						if (/*mat->texture->HasAlpha() &&*/ !texturesFound.IsInList(name))
						{
							zCTextureConvert* texConv = zrenderer->CreateTextureConvert();

							// защита указателя
							if (!texConv)
								return;

							if (!texConv->LoadFromFileFormat(Z name))
							{
								// если не загружена, то выводим сообщение
								cmd << "Texture '" << name << "' not load!";

								// удаляем конвертёр
								delete texConv;

								// выходим
								return;
							}

							// берём информацию о текстуре, расположенной в памяти конверт-менеджера
							zCTextureInfo texInfo = texConv->GetTextureInfo();

							switch (texInfo.texFormat)
							{
								// если исходная текстура (C-TEX с альфа-каналом, формат #12)
							case zRND_TEX_FORMAT_DXT3:

								cmd << mat->GetName() << " : " << name << endl;
								break;
								// если исходная текстура (TGA альфа-каналом, 32 бит)
							case zRND_TEX_FORMAT_BGRA_8888:
							

								cmd << mat->GetName() << " : " << name << endl;
								break;
								// если исходная текстура (TGA без альфа-канала, 24 бит)
							case zRND_TEX_FORMAT_BGR_888:
								break;
								// в случае использования: C-TEX без альфа-канала(zRND_TEX_FORMAT_DXT1, формат #10) и других
							default:
								break;
							}

							texturesFound.InsertEnd(name);
							delete texConv;
						}
					}
				}

			}

			zinput->ClearKeyBuffer();
		}


		return;

		float time = ztimer->frameTimeFloat;
		


		zCWorld* wld = ogame->GetWorld();

		if (!wld) return;
		//auto pos = vob->GetPositionWorld();

		zTBBox3D bbox;
		zCArray<zCVob*> found;
		zREAL distance = 50000;

		bbox.maxs = bbox.mins = ogame->GetCamera()->GetVob()->GetPositionWorld();
		bbox.maxs[0] += distance; bbox.maxs[1] += distance; bbox.maxs[2] += distance;
		bbox.mins[0] -= distance; bbox.mins[1] -= distance; bbox.mins[2] -= distance;
		ogame->GetWorld()->CollectVobsInBBox3D(found, bbox);

		// Nach zCVobSpots durchsuchen
		for (int i = 0; i<found.GetNumInList(); i++)
		{
			auto vob = found.GetSafe(i);

			if (vob && vob->GetVisual() && vob->GetVisual()->GetVisualName() == "NW_NATURE_GRASSGROUP_01.3DS" && IsBadVob(vob))
			{
				debug.AddLine(vob->GetPositionWorld(), vob->GetPositionWorld() + zVEC3(0, 1000, 0), GFX_RED, time);
				//PrintDebug("Diff: " + zSTRING(diff, 20));
				//PrintDebug("Height: " + zSTRING(vob->GetPositionWorld().n[1], 20) + "; " + zSTRING(newpos.n[1], 20));
			}

		}
	}
	
}