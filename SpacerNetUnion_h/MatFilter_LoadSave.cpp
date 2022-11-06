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


		//zERR_MESSAGE(4, zERR_BEGIN, "B: SPC: Loading mat-filter " + matfilename);

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

		//zERR_MESSAGE(4, 0, "B: SPC: Num of materials found: " + zSTRING(numLoaded));

		arch->Close();
		zRELEASE(arch);

		file->Close(); delete file;

		return true;
		//zERR_MESSAGE(4, zERR_END, "");
	}

	void MatFilter::SaveCurrentFilter(int index)
	{
		if (this->filterMatBlocked == 1)
		{
			return;
		}

		if (index > 0)
		{
			zoptions->ChangeDir(DIR_TOOLS_DATA);

			zSTRING matfilename = Z matFilterList.GetSafe(index)->name + MATLIBFILE_EXT;

			matfilename = matfilename.Upper();

			zFILE* file = zfactory->CreateZFile(matfilename);

			if (file->Create() == zERR_NONE)
			{

				zCArchiver* arch = zarcFactory->CreateArchiverWrite(file, zARC_MODE_ASCII, 0, 0);

				zCMaterial *mat = 0;
				zCClassDef* matDef = zCMaterial::classDef;

				for (int matz = 0; matz<matDef->objectList.GetNumInList(); matz++)
				{
					mat = dynamic_cast<zCMaterial*>(matDef->objectList[matz]);
					if (
						(mat->libFlag == index) /*&& (mat->matUsage == zCMaterial::zMAT_USAGE_LEVEL)*/
						)
						arch->WriteObject(mat);
				}

				arch->Close();
				zRELEASE(arch);

				file->Close();
			};
			delete file; file = 0;
		}
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

		cmd << "Saving Matlib.ini" << endl;

		zoptions->ChangeDir(DIR_TOOLS_DATA);

		zFILE* f = NULL;

		f = zfactory->CreateZFile(MATLIB_FILENAME);



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

			f->Close();
		}

		SAFE_DELETE(f);
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


			//zERR_MESSAGE(7, 0, "B: SPC: Configuration: Loading file " + MATLIB_FILENAME);
			if (f->Open(false) == 0)
			{
				// Liste leeren
				matFilterList.EmptyList();

				// Trash-Filter erzuegen
				spcCMatFilter* item = new spcCMatFilter;
				item->id = 0;
				item->name = FILTER_NAME_TRASH;
				matFilterList.Insert(item);

				// In der INI-Datei vorkommende Filter einfuegen
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

						//zERR_MESSAGE(7, 0, "B: SPC: Configuration: MaterialFilter " + item->name + ", #" + zSTRING(item->id));
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
			else
			{
				//zERR_WARNING("B: SPC: Configuration: Loading file " + MATLIB_FILENAME + " failed.");
			};

		}
		else
		{
			cmd << "B: SPC: Configuration: Loading file " << MATLIB_FILENAME << " failed. Not found!" << endl;
			//zERR_WARNING("B: SPC: Configuration: Loading file " + MATLIB_FILENAME + " failed. Not found!");
		};

		DWORD attr = GetFileAttributes("_work\\tools\\data\\" +  MATLIB_FILENAME);

		if (attr != INVALID_FILE_ATTRIBUTES) {
			bool isReadOnly = attr & FILE_ATTRIBUTE_READONLY;

			if (isReadOnly)
			{
				filterMatBlocked = 2;
			}
		}


		cmd << "Loaded .PML files: " << counterLoaded << endl;


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
				//zERR_WARNING("B: SPC: Configuration: Loading file " + MATLIB_FILENAME + " failed.");
			}
			delete f;
			f = 0;
		}
		else
		{
			cmd << "B: SPC: Configuration: Loading file " << MATLIB_FILENAME << " failed. Not found!" << endl;
			//zERR_WARNING("B: SPC: Configuration: Loading file " + MATLIB_FILENAME + " failed. Not found!");
		}



	}
}