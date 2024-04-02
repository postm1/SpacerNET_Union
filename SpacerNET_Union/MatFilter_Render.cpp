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

	// Заливка чёрным цветом всего массива пикселей
	// для текстур 24 бит, без альфа-канала
	void FillBlack_ArrPixels(DWORD* arr_pixels)
	{
		// цвет заливки в виде 4-х байтного числа
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

	// Заливка прозрачным цветом всего массива пикселей
	// для текстур 32 бит, с альфа-каналом
	void FillAlpha_ArrPixels(DWORD* arr_pixels)
	{
		// заполняем массив прозрачными пикселями
		memset(&arr_pixels, 0, sizeof(arr_pixels));
	}


	// рендер на экране по пикселям
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
		// если нужно скинуть кэш след текстуры, то скидываем
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
		// Direct3D девайс для Г2А
		

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
					// если кеш не скидывается принудительно, то проверяем, чтобы кэш был актуален (настройки совпадали для текстуры)
					else if (!cleanTextureCache)
					{
						// если хоть одна из настроек не совпадает, удаляем кэш, он не актуален
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
		// ставим актуальную директорию - директорию с текстурами (не знаю на сколько это важно!)
		// видимо как-то упрощает поиск в подкаталогах
		//zoptions->ChangeDir(DIR_TEX);

		int result = 0;

		zCTextureConvert* texConv = zrenderer->CreateTextureConvert();

		// защита указателя
		if (!texConv)
			return;

		if (!texConv->LoadFromFileFormat(Z texName))
		{
			// если не загружена, то выводим сообщение
			cmd << "Texture '" << texName << "' not load!";

			// удаляем конвертёр
			delete texConv;

			// выходим
			return;
		}
		
		// берём информацию о текстуре, расположенной в памяти конверт-менеджера
		zCTextureInfo texInfo = texConv->GetTextureInfo();

		// по умолчанию включён цветовой набор BGRA для цвета пикселя
		BOOL bUseOriginalColor = FALSE;



		auto entry = new MatFilterRenderEntry();
		entry->name = originalName;
		entry->bUseCenterAligment = mf.bUseCenterAligment;
		entry->bUseAlphaChannels = mf.bUseAlphaChannels;
		entry->bResizeSmallTextures = mf.bResizeSmallTextures;

		pListCache.InsertEnd(entry);

		// Конвертируем текстуру в нужный формат:
		// перебираем формат исходной текстуры
		switch (texInfo.texFormat)
		{
			// если исходная текстура (C-TEX с альфа-каналом, формат #12)
		case zRND_TEX_FORMAT_DXT3:
			// сначала заполняем массив выходного изображения прозрачными пикселями
			FillAlpha_ArrPixels(entry->pixels);
			// устанавливаем новый формат расположения пикселей текстуры
			texInfo.texFormat = zRND_TEX_FORMAT_RGBA_8888;
			// используем раскладку цвевтов: RGBA
			bUseOriginalColor = TRUE;
			entry->bit = 32;
			entry->hasAlpha = 1;
			break;
			// если исходная текстура (TGA альфа-каналом, 32 бит)
		case zRND_TEX_FORMAT_BGRA_8888:
			// заполняем массив прозрачными пикселями
			FillAlpha_ArrPixels(entry->pixels);
			entry->bit = 32;
			entry->hasAlpha = 1;
			break;
			// если исходная текстура (TGA без альфа-канала, 24 бит)
		case zRND_TEX_FORMAT_BGR_888:
			// заполняем массив чёрными пикселями
			FillBlack_ArrPixels(entry->pixels);
			entry->bit = 24;
			entry->hasAlpha = 0;
			break;
			// в случае использования: C-TEX без альфа-канала(zRND_TEX_FORMAT_DXT1, формат #10) и других
		default:
			// заполняем массив чёрными пикселями
			FillBlack_ArrPixels(entry->pixels);
			// устанавливаем новый формат расположения пикселей текстуры
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


		// меняем результирующее число карт детализации
		texInfo.numMipMap = 1;

		// конвертируем текстуру внутри менеджера и получаем результат операции
		result = texConv->ConvertTextureFormat(texInfo);



		// Если результат первой конвертации текстуры - безуспешный
		if (!result)
		{
			cmd << "Convert process #1 failed!" << endl;

			// удаляем конвертёр
			delete texConv;

			// выходим
			return;
		}

		if (((texInfo.sizeX < OUTPUT_SIZEX) && (texInfo.sizeY < OUTPUT_SIZEY) && (mf.bResizeSmallTextures == TRUE)) || ((texInfo.sizeX > OUTPUT_SIZEX) || (texInfo.sizeY > OUTPUT_SIZEY)))
		{
			// исходная ширина текстуры
			float sx = (float)texInfo.sizeX;

			// исходная высота текстуры
			float sy = (float)texInfo.sizeY;

			// соотношение сторон текстуры
			float AspectRatio;

			// Расчитываем пропорциональные размеры текстуры:
			// если ширина текстуры больше, либо равна её высоте
			if (sx >= sy)
			{
				// расчитываем соотношение сторон текстуры
				AspectRatio = sx / sy;

				// устанавливаем максимальную ширину
				sx = OUTPUT_SIZEX;

				// расчитываем пропорциональную высоту текстуры
				sy = sx / AspectRatio;
			}
			else // иначе, ширина текстуры меньше её высоты
			{
				// расчитываем соотношение сторон текстуры
				AspectRatio = sy / sx;

				// устанавливаем максимальную высоту
				sy = OUTPUT_SIZEY;

				// расчитываем пропорциональную ширину текстуры
				sx = sy / AspectRatio;
			}


			// записываем новую ширину текстуры
			texInfo.sizeX = (int)sx;

			// и новую высоту текстуры
			texInfo.sizeY = (int)sy;

			// округляем расчётные размеры до степени двойки
			zCTextureConvert::CorrectPow2(texInfo.sizeX, texInfo.sizeY);

			// ограничение минимальной ширины текстуры до 4-х пикселей
			if (texInfo.sizeX < 4)
				texInfo.sizeX = 4;

			// ограничение минимальной высоты текстуры до 4-х пикселей
			if (texInfo.sizeY < 4)
				texInfo.sizeY = 4;

			//---

			// конвертируем текстуру с её новыми пропорциональными размерами
			result = texConv->ConvertTextureFormat(texInfo);

			// используем новый цветовой набор для пикселей
			bUseOriginalColor = FALSE;
		}

		// Если результат второй конвертации текстуры - безуспешный
		if (!result)
		{
			cmd << "Convert process #2 failed!" << endl;

			// удаляем конвертёр
			delete texConv;

			// выходим
			return;
		}

		// временный цвет пикселя
		D3DCOLOR COL;

		// временный RGBA пикселя
		zVEC4 col;

		// смещение для центровки
		int offsetX = 0;
		int offsetY = 0;


		

		
		// если включено центрирование малых текстур
		if (mf.bUseCenterAligment)
		{
			
			// есть смысл центрировать только тогда,
			// когда хотябы один из размеров текстуры
			// меньше соотв. размера выходного изображения
			if (texInfo.sizeX < OUTPUT_SIZEX)
				offsetX = (OUTPUT_SIZEX - texInfo.sizeX) / 2;

			if (texInfo.sizeY < OUTPUT_SIZEY)
				offsetY = (OUTPUT_SIZEY - texInfo.sizeY) / 2;
		}

		
		int count = 0;

		//cmd << "Format: " << texInfo.texFormat << endl;




		// пробегаемся по вертикальным строкам пикселей
			for (int y = 0; y < texInfo.sizeY; y++)
			{
				// пробегаемся по горизонтальным пикселям
				for (int x = 0; x < texInfo.sizeX; x++)
				{
					// получаем RGBA цвет точки по координатам текстуры
					zVEC4 col = texConv->GetRGBAAtTexel(x, y);

					// если нужно исключить альфа-каналы у пикселей
					if (!mf.bUseAlphaChannels)
						// исключаем прозрачность
						col[3] = 255;


					//***********************************
					// Преобразоваие BGRA/RGBA -> DWORD
					//***********************************
					// если нужно преобразовать BGRA -> DWORD
					if (bUseOriginalColor)
					{
						// преобразуем цвет пикселя в 4-х байтное целое число
						COL = RGBA_2_DWORD((int)col[2], (int)col[1], (int)col[0], (int)col[3]);
					}
					else // иначе, нужно использовать преобразование RGBA -> DWORD
					{
						// преобразуем цвет пикселя в 4-х байтное целое число
						COL = RGBA_2_DWORD((int)col[0], (int)col[1], (int)col[2], (int)col[3]);
					}

					// записываем цвет пикселя в соотв. ячейку массива пикселей текстуры
					// (с учётом центровки изображения)
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
		// берём название текстуры материала
		zSTRING texName = pMat->texture->GetObjectName();
		zSTRING originalNoChangeName = texName;
		//texName = "roof_batons_alpha.TGA";

		texName.Upper();

		//cmd << "Input: " << texName << endl;

		zSTRING originalName = texName;

		originalName = originalName.Cut(originalName.Length() - 4, 4);
		//cmd << "originalName: " << originalName << endl;




		// объявляем окончание для скомпилированных текстур
		
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