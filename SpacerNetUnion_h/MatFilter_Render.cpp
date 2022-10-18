// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	zCArray<MatFilterRenderEntry*> pListCache;
	//zCTextureConvert* texConv = NULL;

	void RenderTexture_Send(MatFilterRenderEntry* entry)
	{
		if (entry)
		{
			//cmd << "Send: " << entry->name << endl;

			DWORD* addr = &entry->pixels[0];
			int size = 128 * 128;
			uint addrSend = (uint)addr;


			Stack_PushUInt(addrSend);
			Stack_PushInt(size);

			//cmd << "size: " << size << " addr: " << addrSend << endl;


			//cmd << "first: " << entry->pixels[0] << endl;

			theApp.exports.MatFilter_SendTexture();
		}
		
	}
	void RenderTextureByPixels3(const zSTRING& texName, int posX, int posY)
	{

		cmd << "RenderTextureByPixels3: try texture: " << texName << endl;
		// Direct3D девайс для Г2А
		IDirect3DDevice7*& dev = *(IDirect3DDevice7**)0x009FC9F4;

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
		zoptions->ChangeDir(DIR_TEX);


		zCTextureConvert* texConv = zrenderer->CreateTextureConvert();
		

		// создаём загрузчик TGA-файлов (возможно и TEX тоже)
		zCTextureFileFormatTGA*    texFile = new zCTextureFileFormatTGA();

		// загружаем текстуру и копируем её в конверт менеджер
		texFile->LoadTexture(texName, texConv);

		// избавляемся от загрузчика текстуры, он нам больше не нужен
		zRELEASE(texFile);

		// берём информацию о текстуре, расположенной в памяти конверт-менеджера
		zCTextureInfo texInfo = texConv->GetTextureInfo();

		///texConv->SetTextureInfo(texInfo);

		// Конвертируем текстуру в нужный формат:
		// устанавливаем новый формат расположения пикселей текстуры
		texInfo.texFormat = zRND_TEX_FORMAT_RGBA_8888;

		// и новые размеры текстуры (лучше указывать в степени двойки):
		texInfo.sizeX = 128; // ширина
		texInfo.sizeY = 128; // высота
							 // конвертируем текстуру внутри менеджера и получаем результат операции
		zBOOL result = texConv->ConvertTextureFormat(texInfo);

		if (!result)
		{
			cmd << "Can't convert!" << endl;
			return;
		}
		// Если результат успешный, то считываем пиксели. Если нет, то выходим.
		//***************************
		// РИСОВАНИЕ ТОЧЕК НА ЭКРАНЕ
		//***************************
		

		// временный цвет пикселя
		D3DCOLOR COL;

		// временный RGBA пикселя
		zVEC4 col;

		auto entry = new MatFilterRenderEntry();
		entry->name = texName;
		//entry->pixels.AllocAbs(128 * 128);

		pListCache.InsertEnd(entry);
		int count = 0;

		// по горизонтальным строчкам пикселей (в данном случае 128)
		for (int x = 0; x < texInfo.sizeX; x++)
		{
			// по вертикальным столбцам пикселей (в данном случае тоже 128)
			for (int y = 0; y < texInfo.sizeY; y++)
			{
				// получаем RGBA цвет точки по координатам
				col = texConv->GetRGBAAtTexel(x, y);

				// цвет точки в виде DWORD числа
				// именно в таком виде его ждёт функция Clear().
				COL = RGBA(col[0], col[1], col[2], col[3]);


				//COL = RGBA2DWORD(col[0], col[1], col[2], col[3]);
				
				//entry->pixels.InsertEnd(RGBA2DWORD(col[0], col[1], col[2], col[3]));

				entry->pixels[count] = COL;
				// posX - начало рисования по оси х(в пикселях)
				// posY - начало рисования по оси y(в пикселях)
				// x - текущий пиксель по оси Х на текстуре, у - соответственно по оси Y
				// прямоугольник в виде точки для последующего закрашивания
				//D3DRECT rec = { posX + x, posY + y, posX + x + 1, posY + y + 1 };

				// закрашиваем контур с помощью функции очистки (таким образом рисуем точку)
				//dev->Clear(1, &rec, D3DCLEAR_TARGET, COL, 1.0f, 0);

				

				
				count += 1;
			}
		}

		//cmd << "Count: Image " << count << endl;
		RX_End(3);

		cmd << texName << " convert: " << RX_PerfString(3) << endl;

		

		RenderTexture_Send(entry);

		

		delete texConv;
	}

	void MatFilter::Render(zCMaterial* pMat)
	{
		// координаты вывода (виртуальные)
		int vposX = 0;
		int vposY = 0;

		int offset = 128; // пока не используется
		static int lastMat = 0; // это последний номер материала из общего списка материалов, который был выведен на экран

								// если у него нет текстуры
		if (!pMat || !pMat->texture)
			// выходим
			return;

		// берём название текстуры материала
		zSTRING texName = pMat->texture->GetObjectName();

		//texName = "RED.tga";

		// координаты вывода преобразуем обратно в пиксели
		RenderTextureByPixels3(texName, screen->nax(vposX), screen->nay(vposY));

		
	}
}