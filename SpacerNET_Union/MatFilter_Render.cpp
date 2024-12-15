// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	zCArray<MatFilterRenderEntry*> pListCache;
	// Output texture size (in pixels):
	const int OUTPUT_SIZEX = 128;
	const int OUTPUT_SIZEY = 128;

	//DWORD arr_pixels[OUTPUT_SIZEX * OUTPUT_SIZEY];




	// RGBA -> DWORD
	inline DWORD RGBA_2_DWORD(int iR, int iG, int iB, int iA)
	{
		return (((((iA << 8) + iR) << 8) + iG) << 8) + iB;
	}

	// ������� ������ ������ ����� ������� ��������
	// ��� ������� 24 ���, ��� �����-������
	void FillBlack_ArrPixels(DWORD* arr_pixels)
	{
		// ���� ������� � ���� 4-� �������� �����
		// (RGBA = 0, 0, 0, 255)
		DWORD FillColor = RGBA_2_DWORD(0, 0, 0, 255);

		// number of pixels in the array
		int numEle = sizeof(arr_pixels) / 4;
		for (int i = 0; i < numEle; i++)
		{
			// and fill them with black
			memcpy(&arr_pixels[i], &FillColor, 4);
		}
	}

	// ������� ���������� ������ ����� ������� ��������
	// ��� ������� 32 ���, � �����-�������
	void FillAlpha_ArrPixels(DWORD* arr_pixels)
	{
		// ��������� ������ ����������� ���������
		memset(&arr_pixels, 0, sizeof(arr_pixels));
	}


	// ������ �� ������ �� ��������
	void RenderOnScreen(int x, int y, D3DCOLOR COL)
	{
		//static IDirect3DDevice7*& dev = *(IDirect3DDevice7**)0x009FC9F4;

		D3DRECT rec = { 500 + x, 400 + y, 500 + x + 1, 400 + y + 1 };

		zCRnd_D3D::xd3d_pd3dDevice7->Clear(1, &rec, D3DCLEAR_TARGET, COL, 1.0f, 0);
	}


	// sending the texture to the interface
	void RenderTexture_Send(MatFilterRenderEntry* entry)
	{
		if (entry)
		{
			//cmd << "Send: " << entry->name << endl;

			DWORD* addr = &entry->pixels[0];
			int size = OUTPUT_SIZEX * OUTPUT_SIZEY;
			uint addrSend = (uint)addr;

			/*
			for (int x = 0; x < OUTPUT_SIZEX; x++)
			{
				for (int y = 0; y < OUTPUT_SIZEY; y++)
				{
					int pos = x * OUTPUT_SIZEX + y;

					auto COL = entry->pixels[pos];

					RenderOnScreen(x, y, COL);
				}
			}
			*/


			CString sizeStr = " " + Z entry->x + " x " + Z entry->y + " x " + Z entry->bit + " ";

			Stack_PushString(entry->name);
			Stack_PushString(sizeStr);
			theApp.exports.MatFilter_UpdateTextureSize();

			//cmd << "MatFilter_SendTexture 1" << endl;
			Stack_PushInt(entry->hasAlpha);
			Stack_PushUInt(addrSend);
			theApp.exports.MatFilter_SendTexture();
			//cmd << "MatFilter_SendTexture 2" << endl;

			//cmd << "MatFilter_UpdateTextureAlphaInfo 1" << endl;
			Stack_PushInt(entry->hasAlpha);
			theApp.exports.MatFilter_UpdateTextureAlphaInfo();

			//cmd << "MatFilter_UpdateTextureAlphaInfo 2" << endl;
			
		}
		
	}

	

	void RenderTextureByPixels3(const zSTRING& texName, zSTRING originalName)
	{
		// ���� ����� ������� ��� ���� ��������, �� ���������
		bool cleanTextureCache = mf.removeTextureCacheNext;



		if (cleanTextureCache)
		{
			//cmd << "remove cache, new one! " << texName << endl;

		}
		else
		{
			//cmd << "no remove cache! " << texName << endl;
		}

		mf.removeTextureCacheNext = false;

		//cmd << "RenderTextureByPixels3: try texture: " << texName << endl;
		// Direct3D ������ ��� �2�
		

		//cmd << "RenderTextureByPixels3: " << pListCache.GetNumInList() << endl;

		
		
		for (int i = 0; i < pListCache.GetNumInList(); i++)
		{
			if (auto entry = pListCache.GetSafe(i))
			{
				if (entry->name == texName)
				{
					if (cleanTextureCache)
					{
						delete entry;
						pListCache.RemoveOrderIndex(i);
						break;
					}
					// ���� ��� �� ����������� �������������, �� ���������, ����� ��� ��� �������� (��������� ��������� ��� ��������)
					else if (!cleanTextureCache)
					{
						// ���� ���� ���� �� �������� �� ���������, ������� ���, �� �� ��������
						if (entry->bUseAlphaChannels != mf.bUseAlphaChannels
							|| entry->bResizeSmallTextures != mf.bResizeSmallTextures
							|| entry->bUseCenterAligment != mf.bUseCenterAligment
							)
						{
							delete entry;
							pListCache.RemoveOrderIndex(i);
							break;
						}

					}
					//cmd << "Texture " << texName << " found" << endl;
					RenderTexture_Send(entry);
					//cmd << "From cache! " << texName << endl;
					return;
				}
			}
		}
		

		RX_Begin(3);
		// ������ ���������� ���������� - ���������� � ���������� (�� ���� �� ������� ��� �����!)
		// ������ ���-�� �������� ����� � ������������
		//zoptions->ChangeDir(DIR_TEX);

		int result = 0;

		zCTexConGeneric* texConv = reinterpret_cast<zCTexConGeneric*>(zrenderer->CreateTextureConvert());

		// ������ ���������
		if (!texConv)
			return;

		if (!texConv->LoadFromFileFormat(Z texName))
		{
			// ���� �� ���������, �� ������� ���������
			cmd << "Texture '" << texName << "' not load!";

			// ������� ��������
			delete texConv;

			// �������
			return;
		}
		
		// ���� ���������� � ��������, ������������� � ������ �������-���������
		zCTextureInfo texInfo = texConv->GetTextureInfo();

		// �� ��������� ������� �������� ����� BGRA ��� ����� �������
		BOOL bUseOriginalColor = FALSE;



		auto entry = new MatFilterRenderEntry();
		entry->name = originalName;
		entry->bUseCenterAligment = mf.bUseCenterAligment;
		entry->bUseAlphaChannels = mf.bUseAlphaChannels;
		entry->bResizeSmallTextures = mf.bResizeSmallTextures;

		pListCache.InsertEnd(entry);

		// ������������ �������� � ������ ������:
		// ���������� ������ �������� ��������
		switch (texInfo.texFormat)
		{
			// ���� �������� �������� (C-TEX � �����-�������, ������ #12)
		case zRND_TEX_FORMAT_DXT3:
			// ������� ��������� ������ ��������� ����������� ����������� ���������
			FillAlpha_ArrPixels(entry->pixels);
			// ������������� ����� ������ ������������ �������� ��������
			texInfo.texFormat = zRND_TEX_FORMAT_RGBA_8888;
			// ���������� ��������� �������: RGBA
			bUseOriginalColor = TRUE;
			entry->bit = 32;
			entry->hasAlpha = 1;
			break;
			// ���� �������� �������� (TGA �����-�������, 32 ���)
		case zRND_TEX_FORMAT_BGRA_8888:
			// ��������� ������ ����������� ���������
			FillAlpha_ArrPixels(entry->pixels);
			entry->bit = 32;
			entry->hasAlpha = 1;
			break;
			// ���� �������� �������� (TGA ��� �����-������, 24 ���)
		case zRND_TEX_FORMAT_BGR_888:
			// ��������� ������ ������� ���������
			FillBlack_ArrPixels(entry->pixels);
			entry->bit = 24;
			entry->hasAlpha = 0;
			break;
			// � ������ �������������: C-TEX ��� �����-������(zRND_TEX_FORMAT_DXT1, ������ #10) � ������
		default:
			// ��������� ������ ������� ���������
			FillBlack_ArrPixels(entry->pixels);
			// ������������� ����� ������ ������������ �������� ��������
			texInfo.texFormat = zRND_TEX_FORMAT_RGB_565;
			entry->bit = 24;
			entry->hasAlpha = 0;
			break;
		}


	
		entry->x = texInfo.sizeX;
		entry->y = texInfo.sizeY;

		/*
		if (entry->x > 4096 || entry->y > 4096)
		{
			entry->bIsTextureTooBig = true;

			CString sizeStr = " " + Z texInfo.sizeX + " x " + Z texInfo.sizeY + " x " + Z entry->bit + " ";

			Stack_PushString(entry->name);
			Stack_PushString(sizeStr);
			theApp.exports.MatFilter_UpdateTextureSize();


			Stack_PushString(originalName);
			theApp.exports.MatFilter_SetTextureBigDontRender();
			return;
		}
		*/


		// ������ �������������� ����� ���� �����������
		texInfo.numMipMap = 1;

		// ������������ �������� ������ ��������� � �������� ��������� ��������
		result = texConv->ConvertTextureFormat(texInfo);



		// ���� ��������� ������ ����������� �������� - �����������
		if (!result)
		{
			cmd << "Convert process #1 failed!" << endl;

			// ������� ��������
			delete texConv;

			// �������
			return;
		}

		if (((texInfo.sizeX < OUTPUT_SIZEX) && (texInfo.sizeY < OUTPUT_SIZEY) && (mf.bResizeSmallTextures == TRUE)) || ((texInfo.sizeX > OUTPUT_SIZEX) || (texInfo.sizeY > OUTPUT_SIZEY)))
		{
			// �������� ������ ��������
			float sx = (float)texInfo.sizeX;

			// �������� ������ ��������
			float sy = (float)texInfo.sizeY;

			// ����������� ������ ��������
			float AspectRatio;

			// ����������� ���������������� ������� ��������:
			// ���� ������ �������� ������, ���� ����� � ������
			if (sx >= sy)
			{
				// ����������� ����������� ������ ��������
				AspectRatio = sx / sy;

				// ������������� ������������ ������
				sx = OUTPUT_SIZEX;

				// ����������� ���������������� ������ ��������
				sy = sx / AspectRatio;
			}
			else // �����, ������ �������� ������ � ������
			{
				// ����������� ����������� ������ ��������
				AspectRatio = sy / sx;

				// ������������� ������������ ������
				sy = OUTPUT_SIZEY;

				// ����������� ���������������� ������ ��������
				sx = sy / AspectRatio;
			}


			// ���������� ����� ������ ��������
			texInfo.sizeX = (int)sx;

			// � ����� ������ ��������
			texInfo.sizeY = (int)sy;

			// ��������� ��������� ������� �� ������� ������
			zCTextureConvert::CorrectPow2(texInfo.sizeX, texInfo.sizeY);

			// ����������� ����������� ������ �������� �� 4-� ��������
			if (texInfo.sizeX < 4)
				texInfo.sizeX = 4;

			// ����������� ����������� ������ �������� �� 4-� ��������
			if (texInfo.sizeY < 4)
				texInfo.sizeY = 4;

			//---

			// ������������ �������� � � ������ ����������������� ���������
			result = texConv->ConvertTextureFormat(texInfo);

			// ���������� ����� �������� ����� ��� ��������
			bUseOriginalColor = FALSE;
		}

		// ���� ��������� ������ ����������� �������� - �����������
		if (!result)
		{
			cmd << "Convert process #2 failed!" << endl;

			// ������� ��������
			delete texConv;

			// �������
			return;
		}

		// ��������� ���� �������
		D3DCOLOR COL;

		// ��������� RGBA �������
		zVEC4 col;

		// �������� ��� ���������
		int offsetX = 0;
		int offsetY = 0;


		

		
		// ���� �������� ������������� ����� �������
		if (mf.bUseCenterAligment)
		{
			
			// ���� ����� ������������ ������ �����,
			// ����� ������ ���� �� �������� ��������
			// ������ �����. ������� ��������� �����������
			if (texInfo.sizeX < OUTPUT_SIZEX)
				offsetX = (OUTPUT_SIZEX - texInfo.sizeX) / 2;

			if (texInfo.sizeY < OUTPUT_SIZEY)
				offsetY = (OUTPUT_SIZEY - texInfo.sizeY) / 2;
		}

		
		int count = 0;

		//cmd << "Format: " << texInfo.texFormat << endl;




		// ����������� �� ������������ ������� ��������
			for (int y = 0; y < texInfo.sizeY; y++)
			{
				// ����������� �� �������������� ��������
				for (int x = 0; x < texInfo.sizeX; x++)
				{
					// �������� RGBA ���� ����� �� ����������� ��������
					zVEC4 col = texConv->GetRGBAAtTexel(x, y);

					// ���� ����� ��������� �����-������ � ��������
					if (!mf.bUseAlphaChannels)
						// ��������� ������������
						col[3] = 255;


					//***********************************
					// ������������� BGRA/RGBA -> DWORD
					//***********************************
					// ���� ����� ������������� BGRA -> DWORD
					if (bUseOriginalColor)
					{
						// ����������� ���� ������� � 4-� ������� ����� �����
						COL = RGBA_2_DWORD((int)col[2], (int)col[1], (int)col[0], (int)col[3]);
					}
					else // �����, ����� ������������ �������������� RGBA -> DWORD
					{
						// ����������� ���� ������� � 4-� ������� ����� �����
						COL = RGBA_2_DWORD((int)col[0], (int)col[1], (int)col[2], (int)col[3]);
					}

					// ���������� ���� ������� � �����. ������ ������� �������� ��������
					// (� ������ ��������� �����������)
					//arr_pixels[(y + offsetY) * OUTPUT_SIZEX + (x + offsetX)] = COL;

					entry->pixels[(y + offsetY) * OUTPUT_SIZEX + (x + offsetX)] = COL;

					count += 1;
				}
			}

		RX_End(3);

		//cmd << texName << " convert: " << RX_PerfString(3) << " originalColor: " << bUseOriginalColor << endl;

		

		RenderTexture_Send(entry);

		

		delete texConv;
	}


	static zSTRING lastName = "";

	void MatFilter::Loop()
	{
	
		if (lastName != "")
		{
			//RenderTextureByPixels3(Z lastName);
		}
	}
	void MatFilter::Render(zCMaterial* pMat)
	{
		if (!pMat || !pMat->texture)
			return;

		if (CheckDx11())
		{
			return;
		}



	





		zSTRING cTEX = "-C.TEX";
		// ���� �������� �������� ���������
		zSTRING texName = pMat->texture->GetObjectName();
		zSTRING originalNoChangeName = texName;
		//texName = "roof_batons_alpha.TGA";

		texName.Upper();

		//cmd << "Input: " << texName << endl;

		zSTRING originalName = texName;

		originalName = originalName.Cut(originalName.Length() - 4, 4);
		//cmd << "originalName: " << originalName << endl;




		// ��������� ��������� ��� ���������������� �������
		
		zSTRING innerFormatName = originalName + cTEX;

		auto result = vdf_fexists(innerFormatName.ToChar(), VDF_DEFAULT);
		bool texFound = false;

		if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
		{
			//cmd << innerFormatName << " TEX found in VDF" << endl;
			texFound = true;
		}

		if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
		{
			//cmd << innerFormatName << " TEX found in _WORK" << endl;
			texFound = true;
		}

		if (texFound)
		{
			lastName = innerFormatName;
			RenderTextureByPixels3(innerFormatName, originalNoChangeName);
		}
		else
		{
			auto result = vdf_fexists(texName.ToChar(), VDF_DEFAULT);
			bool TGAFound = false;

			if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
			{
				//cmd << texName << " TGA found in VDF" << endl;
				TGAFound = true;
			}

			if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
			{
				//cmd << texName << " TGA found in _WORK" << endl;
				TGAFound = true;
			}

			if (TGAFound)
			{
				RenderTextureByPixels3(texName, originalNoChangeName);
			}
			else
			{
				Stack_PushString(originalNoChangeName);
				theApp.exports.MatFilter_SetEmptyTexture();
			}
		}


	}
}