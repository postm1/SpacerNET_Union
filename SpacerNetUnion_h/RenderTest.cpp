// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	
	zCPolygon* mixPoly = NULL;
	const zSTRING nameTex = "circle_01.TGA";
	zCTexture* textMixInfo = NULL;
	zCMaterial	*newMat = NULL;

	void DoWork_TestRender()
	{
		static zCView* pView = NULL;
		static zCCamera*& pCamera = *(zCCamera**)0x008D7F94;

		if (!pView)
		{
			pView = new zCView(0, 0, SCREEN_MAX, SCREEN_MAX);
			screen->InsertItem(pView);
			ogame->GetWorld()->bspTree.mesh->ArraysToLists();
			ogame->GetWorld()->bspTree.mesh->UnshareFeatures();
		}


		if (auto sel = mm.GetCurrentSelection())
		{
			if (sel->poly)
			{
				mixPoly = sel->poly;
				print.PrintRed("Poly select ok");

				
			}
			else
			{
				mixPoly = NULL;
			}
		}
		else
		{
			mixPoly = NULL;
		}
	
		if (pView)
		{
			pView->ClrPrintwin();
		}

		if (mixPoly)
		{
			
			int count = mixPoly->polyNumVert;

			
			if (KeyClick(KEY_D))
			{
				mixPoly->feature[0]->texu += 0.1f;
				zClamp(mixPoly->feature[0]->texu, 0.0f, 1.0f);
			}

			if (KeyClick(KEY_A))
			{
				mixPoly->feature[0]->texu -= 0.1f;
				zClamp(mixPoly->feature[0]->texu, 0.0f, 1.0f);
			}


			if (KeyClick(KEY_W))
			{
				mixPoly->feature[0]->texv += 0.1f;
				zClamp(mixPoly->feature[0]->texv, 0.0f, 1.0f);
			}

			if (KeyClick(KEY_S))
			{
				mixPoly->feature[0]->texv -= 0.1f;
				zClamp(mixPoly->feature[0]->texv, 0.0f, 1.0f);
			}

			if (KeyClick(KEY_F))
			{
				mixPoly->TexScale(2.0f, 2.0f);
			}

			if (KeyClick(KEY_G))
			{
				mixPoly->TexScale(0.5f, 0.5f);
			}

			for (int i = 0; i < count; i++)
			{

				zVEC3 vWorldPos = mixPoly->vertex[i]->position + zVEC3(0, 20, 0);
				zSTRING text = Z i  + " uv: " + zSTRING(mixPoly->feature[i]->texu, 10)
					+ " " + +zSTRING(mixPoly->feature[i]->texv, 10);


				// transform the world position to pixels on screen
				float fPixelPosX = FLT_MAX, fPixelPosY = FLT_MAX;
				if (vWorldPos[0] != FLT_MAX) {
					pCamera->Activate();
					zVEC3 vec = pCamera->camMatrix * vWorldPos;
					if (vec[2] > 0)
						pCamera->Project(&vec, fPixelPosX, fPixelPosY);
				}

				if (fPixelPosX != FLT_MAX) {
					fPixelPosX -= screen->nax(screen->FontSize(text) / 2);
					int x = screen->anx(fPixelPosX);
					int y = screen->any(fPixelPosY);

					zClamp(y, screen->FontY(), 8191 - screen->FontY());
					zClamp(x, 0, 8191 - screen->FontSize(text));


					pView->Print(x, y, text);
				}

				

			}
		}
		//print.PrintRed("S_GetDetailTexturesEnabled: " + Z zrenderMan->S_GetDetailTexturesEnabled());
		//print.PrintRed("zRND_CAP_BLENDDIFFUSEALPHA: " + Z zrenderer->HasCapability(zRND_CAP_BLENDDIFFUSEALPHA));

			
		if (zinput->KeyPressed(KEY_F1))
		{
			if (!newMat)
			{
				auto oldTexture = mixPoly->GetMaterial()->texture;

				newMat = zNEW(zCMaterial);
				newMat->SetTexture(zSTRING(oldTexture->objectName));
				newMat->SetName("SPC_MATBLEND_000");

				newMat->color = GFX_RED;

				/*
				mixPoly->SetMapping(0, zVEC2(1, 1));
				mixPoly->SetMapping(1, zVEC2(1, 1));
				mixPoly->SetMapping(2, zVEC2(1, 1));
				*/
				
				mixPoly->SetMaterial(newMat);

				print.PrintRed("Set new tex: " + oldTexture->objectName);


				auto newTex = newMat->texture;

				//newTex->GetPixelSize(xdim, ydim); // [Moos] 29.11.00 neu
				//zVEC4 col = lightmap->Texture()->GetRGBAAtTexel(zREAL(xdim)*u, zREAL(ydim)*v);
			}
		}
		
	}
	
}