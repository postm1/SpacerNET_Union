// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	zCArray<MatFilterRenderEntry*> pListCache;
	const int OUTPUT_SIZE = 128;


	// рендер на экране по пикселям
	void RenderOnScreen(int x, int y, D3DCOLOR COL)
	{
		static IDirect3DDevice7*& dev = *(IDirect3DDevice7**)0x009FC9F4;

		D3DRECT rec = { 500 + x, 400 + y, 500 + x + 1, 400 + y + 1 };

		dev->Clear(1, &rec, D3DCLEAR_TARGET, COL, 1.0f, 0);
	}


	// посылаем текстуру в интерфейс
	void RenderTexture_Send(MatFilterRenderEntry* entry)
	{
		if (entry)
		{
			//cmd << "Send: " << entry->name << endl;

			DWORD* addr = &entry->pixels[0];
			int size = OUTPUT_SIZE * OUTPUT_SIZE;
			uint addrSend = (uint)addr;

			for (int x = 0; x < OUTPUT_SIZE; x++)
			{
				for (int y = 0; y < OUTPUT_SIZE; y++)
				{
					int pos = x * OUTPUT_SIZE + y;

					auto COL = entry->pixels[pos];

					RenderOnScreen(x, y, COL);
				}
			}

			Stack_PushUInt(addrSend);
			Stack_PushInt(size);

			theApp.exports.MatFilter_SendTexture();
		}
		
	}

	

	void RenderTextureByPixels3(const zSTRING& texName)
	{

		//cmd << "RenderTextureByPixels3: try texture: " << texName << endl;
		// Direct3D девайс для Г2А
		

		//cmd << "RenderTextureByPixels3: " << pListCache.GetNumInList() << endl;

		
		
		for (int i = 0; i < pListCache.GetNumInList(); i++)
		{
			if (auto entry = pListCache.GetSafe(i))
			{
				if (entry->name == texName)
				{
					//cmd << "Texture " << texName << " found" << endl;
					RenderTexture_Send(entry);
					return;
				}
			}
		}
		

		RX_Begin(3);
		// ставим актуальную директорию - директорию с текстурами (не знаю на сколько это важно!)
		// видимо как-то упрощает поиск в подкаталогах
		//zoptions->ChangeDir(DIR_TEX);


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
		
		// по умолчанию включён старый цветовой набор для цвета пикселя
		BOOL bUseOriginalColor = TRUE;

		//**************
		// Конвертация
		//**************
		// берём информацию о текстуре, расположенной в памяти конверт-менеджера
		zCTextureInfo texInfo = texConv->GetTextureInfo();

		cmd << "Original: " << texInfo.texFormat << endl;
		
		zTRnd_TextureFormat originalFormat = texInfo.texFormat;

		// Конвертируем текстуру в нужный формат:
		// устанавливаем новый формат расположения пикселей текстуры
		texInfo.texFormat = zRND_TEX_FORMAT_RGBA_8888;

		texInfo.numMipMap = 1;

		int result = texConv->ConvertTextureFormat(texInfo);


		// если конвертация текстуры прошла успешно
		// и её размеры отличаются от 128х128
		if (result && texInfo.sizeX != OUTPUT_SIZE && texInfo.sizeY != OUTPUT_SIZE)
		{
			// Выполняем повторное конвертирование:
			// новые размеры текстуры (лучше указывать в степени двойки)
			texInfo.sizeX = OUTPUT_SIZE; // ширина
			texInfo.sizeY = OUTPUT_SIZE; // высота

								 // результат конвертации текстуры
			result = texConv->ConvertTextureFormat(texInfo);

			// используем новый цветовой набор RGBA для пикселей
			bUseOriginalColor = FALSE;
		}

							 // конвертируем текстуру внутри менеджера и получаем результат операции
		

		// Если результат успешный, то считываем пиксели
		// Если нет, то выходим
		if (!result)
		{
			// удаляем конвертёр
			delete texConv;
			cmd << "Can't convert! " << texName << endl;
			// выходим
			return;
		}


		// временный цвет пикселя
		D3DCOLOR COL;

		// временный RGBA пикселя
		zVEC4 col;

		auto entry = new MatFilterRenderEntry();
		entry->name = texName;

		pListCache.InsertEnd(entry);
		int count = 0;

		cmd << "Format: " << texInfo.texFormat << endl;

		// по горизонтальным строчкам пикселей (в данном случае 128)
		for (int x = 0; x < texInfo.sizeX; x++)
		{
			// по вертикальным столбцам пикселей (в данном случае тоже 128)
			for (int y = 0; y < texInfo.sizeY; y++)
			{
				// получаем RGBA цвет точки по координатам
				col = texConv->GetRGBAAtTexel(x, y);


				// текстура без альфы, но конвертер ее ломает
				if (originalFormat == zRND_TEX_FORMAT_DXT1)
				{
					col.n[3] = 255.0f;
				}
				//cmd << "R: " << col.n[0] << " G: " << col.n[1] << " B: " << col.n[2] << " A: " << col.n[3] << endl;

				if (bUseOriginalColor)
					COL = RGBA(col[0], col[1], col[2], col[3]); // старый
				else
					COL = RGBA(col[2], col[1], col[0], col[3]); // новый


				
				entry->pixels[count] = COL;
				// posX - начало рисования по оси х(в пикселях)
				// posY - начало рисования по оси y(в пикселях)
				// x - текущий пиксель по оси Х на текстуре, у - соответственно по оси Y
				// прямоугольник в виде точки для последующего закрашивания

				RenderOnScreen(x, y, COL);
				
				

				
				count += 1;
			}
		}
		
		for (int i = 0; i < OUTPUT_SIZE * OUTPUT_SIZE; i++)
		{
			RGBA col = DWORD2RGBA(entry->pixels[i]);

			//cmd << "R: " << col.R << " G: " << col.G << " B: " << col.B << " A: " << col.Alpha  << endl;
		}
		//cmd << "Count: Image " << count << endl;
		RX_End(3);

		cmd << texName << " convert: " << RX_PerfString(3) << " originalColor: " << bUseOriginalColor << endl;

		

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
			// выходим
			return;


		// посылаем размер текстуры в интерфейс
		int x, y;
		pMat->texture->GetPixelSize(x, y);
		CString sizeStr = Z x + "x" + Z y;


		Stack_PushString(pMat->texture->GetObjectName());
		Stack_PushString(sizeStr);
		theApp.exports.MatFilter_UpdateTextureSize();
	





		zSTRING cTEX = "-C.TEX";
		// берём название текстуры материала
		zSTRING texName = pMat->texture->GetObjectName();

		//texName = "roof_batons_alpha.TGA";

		texName.Upper();

		cmd << "Input: " << texName << endl;

		zSTRING originalName = texName;

		originalName = originalName.Cut(originalName.Length() - 4, 4);
		//cmd << "originalName: " << originalName << endl;





		// объявляем окончание для скомпилированных текстур
		
		zSTRING innerFormatName = originalName + cTEX;

		auto result = vdf_fexists(innerFormatName.ToChar(), VDF_DEFAULT);
		bool texFound = false;

		if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
		{
			cmd << innerFormatName << " TEX found in VDF" << endl;
			texFound = true;
		}

		if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
		{
			cmd << innerFormatName << " TEX found in _WORK" << endl;
			texFound = true;
		}

		if (texFound)
		{
			lastName = innerFormatName;
			RenderTextureByPixels3(innerFormatName);
		}
		else
		{
			auto result = vdf_fexists(texName.ToChar(), VDF_DEFAULT);
			bool TGAFound = false;

			if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
			{
				cmd << texName << " TGA found in VDF" << endl;
				TGAFound = true;
			}

			if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
			{
				cmd << texName << " TGA found in _WORK" << endl;
				TGAFound = true;
			}

			if (TGAFound)
			{
				RenderTextureByPixels3(texName);
			}
		}


	}
}