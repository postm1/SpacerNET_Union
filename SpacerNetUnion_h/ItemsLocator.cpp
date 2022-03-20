// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	ItemsLocator::ItemsLocator()
	{
		show = false;
	}

	void ItemsLocator::Init()
	{
		sx = 160;
		sy = screen->any(screen->nax(sx));

		pLocatorView = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
		pLocatorView->SetPos(0, 0);
		screen->InsertItem(pLocatorView);

		showOnlyProbablyBad = true;
		offsetVertical = 200;
	}

	void ItemsLocator::Reset()
	{
		for (int i = 0; i < pListEntries.GetNumInList(); i++)
		{
			auto entry = pListEntries.GetSafe(i);

			if (entry)
			{
				if (pLocatorView) pLocatorView->RemoveItem(entry->pView);

				SAFE_DELETE(entry->pView);
				SAFE_DELETE(entry);
			}
		}

		pListEntries.DeleteList();
		pListItems.DeleteList();
	}

	void ItemsLocator::RemoveByItem(zCVob* pItem)
	{
		if (!pItem) return;

		for (int i = 0; i < pListEntries.GetNumInList(); i++)
		{
			auto entry = pListEntries.GetSafe(i);

			if (entry && entry->pVob == pItem)
			{
				pLocatorView->RemoveItem(entry->pView);
				SAFE_DELETE(entry->pView);
				SAFE_DELETE(entry);
				pListEntries.RemoveOrderIndex(i);
				pListItems.Remove(pItem->CastTo<oCItem>());
				break;
			}
		}
	}
	
	zVEC3 ItemsLocator::GetFocus(zCVob* vob)
	{
		return vob->bbox3D.GetCenter() + (vob->bbox3D.GetCenter() - vob->bbox3D.GetCenterFloor()) * 0.5f;
	}

	bool ItemsLocator::IsBadItem(zCVob * vob)
	{
		
		if (vob->groundPoly

			|| ogame->GetWorld()->TraceRayFirstHit(GetFocus(vob), zVEC3(0, -500, 0), vob, zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER)
			
			)
		{
			return false;
		}
		

		int result = ogame->GetWorld()->TraceRayNearestHit(vob->bbox3D.GetCenter(), zVEC3(0, 500, 0), vob,
			zTRACERAY_POLY_2SIDED | zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER | zTRACERAY_VOB_IGNORE);

		if (result)
		{
			if (vob->bbox3D.GetCenter().n[1] < ogame->GetWorld()->traceRayReport.foundIntersection.n[1])
			{
				return true;
			}
		}

		if (!ogame->GetWorld()->TraceRayNearestHit(vob->GetPositionWorld(), zVEC3(0, offsetVertical, 0), vob,
			zTRACERAY_POLY_2SIDED | zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER))
		{
			return false;
		}

		//zVEC3 shift = ogame->GetWorld()->traceRayReport.foundIntersection - vob->GetPositionWorld();
		//vob->Move(shift[0], shift[1], shift[2]);
		return true;
	}

	void ItemsLocator::Draw(ItemsLocatorEntry* drawEntry)
	{
		zVEC3 iconPosition = drawEntry->pVob->GetPositionWorld() + zVEC3(0.0f, 20.0f, 0.0f);
		zCCamera* cam = ogame->GetCamera();
		zVEC3 viewPos = cam->GetTransform(zTCamTrafoType::zCAM_TRAFO_VIEW) * iconPosition;
		int x, y;

		bool searchByName = theApp.options.GetIntVal("itemLocatorOnlyByName");


		cam->Project(&viewPos, x, y);

		if (viewPos[2] > cam->nearClipZ) {
			int px = pLocatorView->anx(x);
			int py = pLocatorView->any(y);

			bool isWrong = sx <= 0 || sx >= 8192 || sy <= 0 || sy >= 8192 || px <= 0 || px >= 8192 || py <= 0 || py >= 8192;
			bool isWrong2 = (px + sx <= 0) || (px + sx >= 8192) || (py + sy <= 0) || (py + sy >= 8192);
			if (!isWrong && !isWrong2) {

				
				zCWorld* wld = drawEntry->pVob->GetHomeWorld();
				

				if (searchByName)
				{
					zSTRING searchName = theApp.options.GetVal("itemLocatorNameSearch").Upper();

					if (drawEntry->pVob->GetInstanceName().Upper().Contains(searchName))
					{
						drawEntry->pView->SetPos(px, py);
						drawEntry->pView->SetSize(sx, sy);
						pLocatorView->InsertItem(drawEntry->pView);
						drawEntry->pView->Blit();
						pLocatorView->RemoveItem(drawEntry->pView);
					}
					else
					{
						drawEntry->pView->ClrPrintwin();
						drawEntry->pVob->drawBBox3D = false;
					}
				}
				else if (!showOnlyProbablyBad || IsBadItem(drawEntry->pVob))
				{
					drawEntry->pView->SetPos(px, py);
					drawEntry->pView->SetSize(sx, sy);
					pLocatorView->InsertItem(drawEntry->pView);
					drawEntry->pView->Blit();
					pLocatorView->RemoveItem(drawEntry->pView);
				}
	
				
				/*
				if (wld->TraceRayNearestHit(drawEntry->pVob->GetPositionWorld() + zVEC3(0, 200, 0), zVEC3(0, -3000, 0), drawEntry->pVob, zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE_NO_CD_DYN)) {
					// Poly gefunden
					if (wld->traceRayReport.foundPoly || wld->traceRayReport.foundVertex) {

						zVEC3 newpos = wld->traceRayReport.foundIntersection;

						if (newpos.n[1] > (drawEntry->pVob->GetPositionWorld().n[1] + offset) || !showOnlyProbablyBad)
						{
							drawEntry->pView->ClrPrintwin();
							//itemView->InsertBack(Z * allowedTextures.GetSafe(param.textureId));

							//drawEntry->pVob->drawBBox3D = true;

							drawEntry->pView->SetPos(px, py);
							drawEntry->pView->SetSize(sx, sy);
							pLocatorView->InsertItem(drawEntry->pView);
							drawEntry->pView->Blit();
							pLocatorView->RemoveItem(drawEntry->pView);
						}

					}
				}
				*/
				else
				{
					drawEntry->pView->ClrPrintwin();
					drawEntry->pVob->drawBBox3D = false;

				}

			}
		}
		else
		{
			drawEntry->pView->ClrPrintwin();
			drawEntry->pVob->drawBBox3D = false;
		}
	}

	void ItemsLocator::Loop()
	{
		if (!theApp.IsAWorldLoaded()) return;
		if (!ogame->GetCamera()) return;
		if (!ogame->GetCamera()->GetVob()) return;


		zCArray<oCItem*>finalResultList;
		static bool clearShow = false;
		auto camVob = ogame->GetCamera()->GetVob();

		bool showEnabled = theApp.options.GetIntVal("itemLocatorEnabled");
		int radius = theApp.options.GetIntVal("itemLocatorRadius");
		showOnlyProbablyBad = theApp.options.GetIntVal("itemLocatorOnlySusp");


		if (showEnabled)
		{
			
			auto pos = camVob->GetPositionWorld();

			zTBBox3D box;

			
			clearShow = false;

			box.maxs = camVob->GetPositionWorld() + zVEC3(radius, radius, radius);
			box.mins = camVob->GetPositionWorld() - zVEC3(radius, radius, radius);


			zCArray<zCVob*>resVobList;
			
			ogame->GetWorld()->CollectVobsInBBox3D(resVobList, box);

			for (int i = 0; i < resVobList.GetNumInList(); i++)
			{

				if (auto pItem = dynamic_cast<oCItem*>(resVobList.GetSafe(i)))
				{
					if (pItem == theApp.currenItemRender || pItem == theApp.currentVobRender)
					{
						continue;
					}

					
					finalResultList.Insert(pItem);


					
				}
			}
		}
		else
		{
			if (!clearShow)
			{
				clearShow = true;

				for (int i = 0; i < pListEntries.GetNumInList(); i++)
				{
					auto entry = pListEntries.GetSafe(i);

					if (entry && entry->pVob && entry->pView)
					{
						entry->pView->ClrPrintwin();
						entry->pVob->drawBBox3D = false;
					}

				}
			}

			return;
			
		}

		
		

		for (int i = 0; i < finalResultList.GetNumInList(); i++)
		{
			auto nextVob = finalResultList.GetSafe(i);

			if (nextVob)
			{
				if (!pListItems.IsInList(nextVob))
				{
					pListItems.Insert(nextVob);

					auto entry = new ItemsLocatorEntry();

					entry->pVob = nextVob;
					entry->pView = new zCView(0, 0, sx, sy);
					entry->pView->InsertBack("L_I_POTIONS.TGA");
					entry->pView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);


					pListEntries.Insert(entry);
				}
			}
		}


		screen->RemoveItem(pLocatorView);

		for (int i = 0; i < pListEntries.GetNumInList(); i++)
		{
			auto entry = pListEntries.GetSafe(i);

			if (entry && entry->pVob && entry->pView)
			{
				if (Dist(entry->pVob, camVob) <= radius)
				{
					Draw(entry);
				}
				else
				{
					entry->pView->ClrPrintwin();
					entry->pVob->drawBBox3D = false;
				}
				
			}

		}

		pLocatorView->Blit();
		//pLocatorView->BlitText();
		screen->InsertItem(pLocatorView);
		pLocatorView->Top();
	}
	

	void ItemsLocator::Toggle()
	{
		if (this->show)
		{
			this->Hide();
		}
		else
		{
			this->Show();
		}
	}

	void ItemsLocator::Show()
	{
		this->show = true;
	}

	void ItemsLocator::Hide()
	{
		this->show = false;
	}

}