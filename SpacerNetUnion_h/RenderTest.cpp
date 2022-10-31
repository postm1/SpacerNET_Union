// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	/*
	zCPolygon* mixPoly = NULL;
	const zSTRING nameTex = "circle_01.TGA";
	zCTexture* textMixInfo = NULL;
	zCMaterial	*newMat = NULL;

	void DoWork_TestRender()
	{
		if (!mixPoly)
		{
			if (auto sel = mm.GetCurrentSelection())
			{
				if (sel->poly)
				{
					mixPoly = sel->poly;
					print.PrintRed("Poly select ok");
				}
			}
		}

		if (zinput->KeyPressed(KEY_F1))
		{
			if (!newMat)
			{
				auto oldTexture = mixPoly->GetMaterial()->texture;

				newMat = zNEW(zCMaterial);
				newMat->SetTexture(zSTRING(oldTexture->objectName));
				newMat->SetName("SPC_MATBLEND_000");

				mixPoly->SetMaterial(newMat);

				print.PrintRed("Set new tex: " + oldTexture->objectName);


				auto newTex = newMat->texture;

				newTex->GetPixelSize(xdim, ydim); // [Moos] 29.11.00 neu
				zVEC4 col = lightmap->Texture()->GetRGBAAtTexel(zREAL(xdim)*u, zREAL(ydim)*v);
			}
		}
		
	}
	*/
}