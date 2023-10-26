// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	bool MatFilter::LoadPML_File(CString fname)
	{
		zoptions->ChangeDir(DIR_TOOLS_DATA);
		zSTRING matfilename = fname.ToChar();
		matfilename = matfilename + ".pml";

		//cmd << "Read PML file: " << fname << endl;

		zFILE* file = zfactory->CreateZFile(matfilename);
		if (!file->Exists()) {
			delete file; //zerr->Warning("B: SPC: File of Material-Lib not found: " + matfilename); 
			return false;
		}

		file->Open(false);
		file->Reset();

		if (file->Size() == 0)
		{
			delete file;
			return false;
		}

		zCArchiver* arch = zarcFactory->CreateArchiverRead(file, 0);

		zSTRING cn, matName;
		zWORD cv;
		zCMaterial* mat = 0;
		long    pos;
		int        numLoaded = 0;
		while (file->Pos() + 3<file->Size())
		{
			pos = file->Pos();
			arch->ReadChunkStart(cn, cv);

			matName = arch->ReadString("name");

			file->Seek(pos);
			mat = zCMaterial::SearchName(matName);

			if (!mat)
			{
				mat = dynamic_cast<zCMaterial*>(arch->ReadObject(0));
				//cmd << "No mat: " << mat->GetName() << endl;
			}
			else
			{
				dynamic_cast<zCMaterial*>(arch->ReadObject(0));

				numLoaded++;
			}

			if (mat)
			{
				mat->AddRef();
				mat->SetUsage(zCMaterial::zMAT_USAGE_LEVEL);

				int index = FindMatFilterIndexByName(fname);

				if (index >= 0)
				{

					mat->libFlag = index;
				}

				numLoaded++;
			}

		}

		arch->Close();
		zRELEASE(arch);

		file->Close(); delete file;

		return true;
	}


	/*
	bool blockedSaveTex = false;

	//zBOOL zCTextureConvert::SaveToFileFormat(const zSTRING& fileName)
	HOOK Invk_zCTextureConvertt_SaveToFileFormat AS(&zCTextureConvert::SaveToFileFormat, &zCTextureConvert::SaveToFileFormat_Union);
	zBOOL zCTextureConvert::SaveToFileFormat_Union(const zSTRING& fileName)
	{
		if (blockedSaveTex)
		{
			return FALSE;
		}

		return THISCALL(Invk_zCTextureConvertt_SaveToFileFormat)(fileName);
	}
	*/

	void MatFilter::SaveCurrentFilter(int index)
	{
		if (this->filterMatBlocked == 1)
		{
			return;
		}


		//cmd << "SaveCurrentFilter index: " << index  << endl;
		RX_Begin(10);

		if (index > 0)
		{
			zoptions->ChangeDir(DIR_TOOLS_DATA);


			CreateDirectory("_work\\tools\\data\\spacernet_backups", NULL);


			// временный файл
			zSTRING fileNameTempWork = Z matFilterList.GetSafe(index)->name + MATLIBFILE_EXT + "_spacernet";
			//реальный файл
			zSTRING realFileName = Z matFilterList.GetSafe(index)->name + MATLIBFILE_EXT;


			fileNameTempWork = fileNameTempWork.Upper();
			realFileName = realFileName.Upper();

			zSTRING matfilenameBackup = "spacernet_backups/" + realFileName;

			// временный файл
			zFILE* fileTempWork = zfactory->CreateZFile(fileNameTempWork);
			// создаем в первый раз копию PML файла
			zFILE* fileBackup = zfactory->CreateZFile(matfilenameBackup);

			if (!fileBackup->Exists())
			{
				// реальный файл для создания копии
				zFILE* fileReal = zfactory->CreateZFile(realFileName);
				if (fileReal->Exists())
				{
					//cmd << "!!!!!!!!!!!Make copy...." << endl;

					zFILE_FILE backupfile(matfilenameBackup);
					fileReal->FileCopy(&backupfile);
					SAFE_DELETE(fileReal);
				}
				
			}

			fileBackup->Close();

			

			zoptions->ChangeDir(DIR_TOOLS_DATA);

			int countWrite = 0;


			auto ShowLoadingForm = (loadForm)GetProcAddress(theApp.module, "ShowLoadingForm");
			

			bool windowShown = false;
			auto updateText = (callVoidFunc)GetProcAddress(theApp.module, "LoadingForm_UpdateText");

			if (fileTempWork->Create() == zERR_NONE)
			{

				zCArchiver* arch = zarcFactory->CreateArchiverWrite(fileTempWork, zARC_MODE_ASCII, 0, 0);

				zCMaterial *mat = 0;
				zCClassDef* matDef = zCMaterial::classDef;
				int maxCount = matDef->objectList.GetNumInList();
				int percent = 0;


				float totalTime = 0;
				//blockedSaveTex = true;

				zCArray<zSTRING> toConvertList;
				zCArray<zSTRING> convertedList;
				zSTRING cTEX = "-C.TEX";

				for (int matz = 0; matz < maxCount; matz++)
				{
					auto mat = dynamic_cast<zCMaterial*>(matDef->objectList[matz]);

					if (mat && mat->texture && mat->libFlag == index)
					{
						zSTRING pureName = mat->texture->GetObjectName();
						zSTRING originalName = mat->texture->GetObjectName();
						zSTRING name = originalName.Cut(originalName.Length() - 4, 4);
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

						if (!texFound && !toConvertList.IsInList(pureName))
						{
							//cmd << originalName << endl;
							toConvertList.Insert(pureName);
						}
					}
				}

				int convertNumList = toConvertList.GetNumInList();
				int curConvertCount = 0;

				if (convertNumList > 1)
				{
					ShowLoadingForm(4);

					//cmd << "ConvertTex: " << convertNumList << endl;
				}

				CString convText = GetLang("WIN_MATFILTER_CONV_TGA");

				for (int matz = 0; matz<maxCount; matz++)
				{
					mat = dynamic_cast<zCMaterial*>(matDef->objectList[matz]);

					RX_Begin(11);
					if (mat && (mat->libFlag == index) /*&& (mat->matUsage == zCMaterial::zMAT_USAGE_LEVEL)*/)
					{
						arch->WriteObject(mat);
						//cmd << "Write object: " << mat->GetName() << endl;
					}
					RX_End(11);
					percent = matz * 100 / maxCount;

					totalTime += (perf[11] / 1000);


					if (totalTime >= 1000 && !windowShown)
					{
						if (!windowShown)
						{
							
							windowShown = true;
						}
					}

					//if (windowShown && totalTime >= 500)
					if (convertNumList > 0 
						&& mat->texture 
						&& toConvertList.IsInList(mat->texture->GetObjectName())
						&& !convertedList.IsInList(mat->texture->GetObjectName())
					)
					{
						convertedList.InsertEnd(mat->texture->GetObjectName());

						
						totalTime = 0;
						curConvertCount += 1;

						if (curConvertCount > convertNumList)
						{
							curConvertCount = convertNumList;
						}

						//cmd << "Конвертация TGA текстур: " << Z curConvertCount << "/" << Z convertNumList << " " << mat->texture->GetObjectName() << endl;

						

						Stack_PushString(Z convText + Z curConvertCount + "/" + Z convertNumList);
						updateText();
					}
						
				}


				arch->Close();
				zRELEASE(arch);

				// закрываем, чтобы файл можно было скопировать в другой
				fileTempWork->Close();

				// пишем в реальный файл
				fileTempWork->FileCopy(realFileName, true);
				

				// удаляем временный файл
				zFILE_FILE fileWorkRemove(fileNameTempWork);

				if (fileWorkRemove.Exists())
				{
					fileWorkRemove.FileDelete();
				}
				


			};
			//fileTempWork->Close();
			SAFE_DELETE(fileTempWork);

			(callVoidFunc)GetProcAddress(theApp.module, "CloseLoadingForm")();
		}

		RX_End(10);
		//cmd << "SaveCurrentFilter index: " << index << " " << RX_PerfString(10) << endl;
	}

	void MatFilter::SaveFilterList()
	{
		if (this->filterMatBlocked != 0)
		{
			return;
		}

		if (!worldWasLoadedOnce)
		{
			return;
		}

		//RX_Begin(11);

		cmd << "Saving Matlib.ini" << endl;

		zoptions->ChangeDir(DIR_TOOLS_DATA);

		zFILE* f = zfactory->CreateZFile(MATLIB_FILENAME);

		if (f->Create() == zERR_NONE)
		{
			spcCMatFilter* item = 0;

			zSTRING line;
			f->Write("[MATERIALFILTER]\r\n");

			for (int i = 0; i <matFilterList.GetNumInList(); i++)
			{
				if (i != 0)
				{
					item = matFilterList.GetSafe(i);

					if (item)
					{
						line = Z item->name + Z "=#" + zSTRING(item->id);
						f->Write(line + "\r\n");
					}
				}
			}

			
		}
		else
		{
			cmd << "Can't save matlib.ini!" << endl;
		}

		f->Close();
		SAFE_DELETE(f);

		//RX_End(11);
		//cmd << "SaveFilterList ok" << endl;
	}

	void MatFilter::SaveAllMatFilter()
	{

	}

	void MatFilter::LoadFilterList()
	{
		zoptions->ChangeDir(DIR_TOOLS_DATA);

		cmd << "Loading materials lib..." << endl;

		int counterLoaded = 0;
		int filterReadCount = 0;


		zFILE* f = zfactory->CreateZFile(MATLIB_FILENAME);
		zFILE* fileBackup = zfactory->CreateZFile(MATLIB_FILENAME_BACKUP);

		// делает копию файла MATLIB_FILENAME
		if (!fileBackup->Exists() && f->Exists())
		{
			zFILE_FILE backupfile(MATLIB_FILENAME_BACKUP);
			f->FileCopy(&backupfile);
		}



		SAFE_DELETE(fileBackup);

		if (f->Exists())
		{
			if (f->Open(false) == 0)
			{
				matFilterList.EmptyList();

				spcCMatFilter* item = new spcCMatFilter;
				item->id = 0;
				item->name = FILTER_NAME_TRASH;
				matFilterList.Insert(item);

				zSTRING line;
				zSTRING filterName;

				//cmd << "Cycle" << endl;

				do
				{
					f->Read(line);

					if (line.Search("#", 1)>0)
					{
						if (line.Search("\"", 1) >= 0)
						{
							line.Delete("\"", zSTR_TO);
							filterName = line.PickWord(1, "\"", "\"");
						}
						else
						{
							filterName = line.PickWord(1, "=", "=");
							filterName.TrimLeft(' ');
							filterName.TrimRight(' ');
						};
						line.Delete("#", zSTR_TO);
						zSTRING idStr = line;

						item = new spcCMatFilter;



						//item->Init(filterName.ToChar(), idStr.ToInt32());
						item->name = filterName.ToChar();
						item->id = idStr.ToInt32();

						filterReadCount += 1;

						/*
						if (filterReadCount != item->id)
						{
							filterMatBlocked = 1;
							cmd << "FILTER FORMAT ERROROR" << endl;
						}
						*/

						matFilterList.Insert(item);



						//cmd << "MatLib entry: " << item->name << endl;

						if (LoadPML_File(filterName.ToChar()))
						{
							counterLoaded++;
						}
					}
				} while (!f->Eof());
				f->Close();
			}

		}
		else
		{
			cmd << "B: SPC: Configuration: Loading file " << MATLIB_FILENAME << " failed. Not found!" << endl;
		};

		DWORD attr = GetFileAttributes("_work\\tools\\data\\" +  MATLIB_FILENAME);

		if (attr != INVALID_FILE_ATTRIBUTES) {
			bool isReadOnly = attr & FILE_ATTRIBUTE_READONLY;

			if (isReadOnly)
			{
				filterMatBlocked = 2;
			}
		}


		cmd << "MatFilter: Loaded " << counterLoaded << " .PML files" << endl;


		SAFE_DELETE(f);




		counterLoaded = 0;

		zoptions->ChangeDir(DIR_TOOLS_DATA);
		f = zfactory->CreateZFile(MATLIB_FILENAME);

		zerr->Separator("LOADENK METRIAL HUILTERS");
		if (f->Exists())
		{
			if (f->Open(false) == 0)
			{
				zSTRING line;
				zSTRING matName;
				do
				{
					f->Read(line);

					if (line.Search("#", 1)>0)
					{
						if (line.Search("\"", 1) >= 0)
						{
							line.Delete("\"", zSTR_TO);
							matName = line.PickWord(1, "\"", "\"");
						}
						else
						{
							matName = line.PickWord(1, "=", "=");
							matName.TrimLeft(' ');
							matName.TrimRight(' ');
						};
						line.Delete("#", zSTR_TO);
						//cmd << "LoadMatlib: " << matName.ToChar()  << endl;
						if (LoadPML_File(matName.ToChar()))
						{
							counterLoaded++;
						}
					}
				} while (!f->Eof());
				f->Close();
			}
			else
			{
				cmd << "B: SPC: Configuration: Loading file " << MATLIB_FILENAME << " failed." << endl;
			}
			delete f;
			f = 0;
		}
		else
		{
			cmd << "B: SPC: Configuration: Loading file " << MATLIB_FILENAME << " failed. Not found!" << endl;
		}



	}
}