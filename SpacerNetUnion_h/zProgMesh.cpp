// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	zCArchiver* ar = 0;
	bool bMatLibLoaded = false;
	zCArray<zCMaterial*> arrMat;
	zCMaterial* pMap;
	bool bExists = false;
	const zSTRING MATLIB_FILENAME = "matlib.ini";
	const CString NO_FILTER_FOLDER = "[TRASH]";


	enum TStartUp { START_LOAD, START_PARSE };



	class spcCMatFilter
	{
		bool isDirty;
	public:

		enum TMatLibFlag { NullLib = 0 };

		CString name;
		int id;

		spcCMatFilter();
		spcCMatFilter(CString _name, int _id = -1) { Init(_name, _id); }

		~spcCMatFilter();

		void Init(CString _name, int _id = -1);
		void Deinit();

		void SetDirty(bool dirty = true)
		{
			//if (name.Find('[')<0) isDirty = dirty;
		};

		bool IsDirty()
		{
			return isDirty;
		};

		friend bool operator==(spcCMatFilter item1, spcCMatFilter item2);

		static zCArray<int>	freeMatFilterIDs;
		static zBYTE		nextMatFilterID;

	};

	zCArray<int>	spcCMatFilter::freeMatFilterIDs;
	zBYTE			spcCMatFilter::nextMatFilterID = spcCMatFilter::NullLib + 1;

	void spcCMatFilter::Init(CString _name, int _id)
	{
		name = _name;
		if (_id >= 0)
		{
			id = _id;

			//zERR_MESSAGE(4, 0, "B: SPCCFG: Defined Matfilter \"" + zSTRING(_name.GetBuffer(0)) + "\" with id " + zSTRING(id));
			if (id >= nextMatFilterID)
			{
				while (nextMatFilterID<id)
				{
					freeMatFilterIDs.InsertEnd(nextMatFilterID);
					nextMatFilterID++;
				};
				nextMatFilterID++;
			};
			return;
		}

		if (freeMatFilterIDs.GetNumInList()>0)
		{
			id = freeMatFilterIDs[0];
			freeMatFilterIDs.RemoveIndex(0);

			//zERR_MESSAGE(4, 0, "B: SPCCFG: Defined Matfilter \"" + zSTRING(_name.GetBuffer(0)) + "\" with id " + zSTRING(id) + " from freelist");
		}
		else
		{
			id = nextMatFilterID;
			nextMatFilterID++;

			//zERR_MESSAGE(4, 0, "B: SPCCFG: Defined Matfilter \"" + zSTRING(_name.GetBuffer(0)) + "\" with id " + zSTRING(id) + " from next free id");
		}

		SetDirty(false);
	}

	void spcCMatFilter::Deinit()
	{
		//zERR_MESSAGE(4, 0, "B: SPCCFG: Removed Matfilter \"" + zSTRING(name.GetBuffer(0)) + "\" with id " + zSTRING(id));
		freeMatFilterIDs.InsertEnd(id);
	};

	spcCMatFilter::~spcCMatFilter()
	{
	}

	spcCMatFilter::spcCMatFilter()
	{
		name = "";
		id = -1;
		SetDirty(false);
	};



	void LoadMatlib(CString fname)
	{
		zoptions->ChangeDir(DIR_TOOLS_DATA);
		zSTRING matfilename = fname.ToChar();
		matfilename = matfilename + ".pml";
		zFILE* file = zfactory->CreateZFile(matfilename);
		if (!file->Exists()) {
			delete file; //zerr->Warning("B: SPC: File of Material-Lib not found: " + matfilename); 
			return;
		}


		//zERR_MESSAGE(4, zERR_BEGIN, "B: SPC: Loading mat-filter " + matfilename);

		file->Open(false);
		file->Reset();

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

			if (!mat) mat = dynamic_cast<zCMaterial*>(arch->ReadObject(0));
			else
			{
				dynamic_cast<zCMaterial*>(arch->ReadObject(0));

				numLoaded++;
			}

			if (mat)
			{
				mat->AddRef();
				mat->SetUsage(zCMaterial::zMAT_USAGE_LEVEL);

				numLoaded++;
			}

		}

		//zERR_MESSAGE(4, 0, "B: SPC: Num of materials found: " + zSTRING(numLoaded));

		arch->Close();
		zRELEASE(arch);

		file->Close(); delete file;

		//zERR_MESSAGE(4, zERR_END, "");
	}

	zCArray<spcCMatFilter*> matFilterList;


	void LoadMat()
	{
		zoptions->ChangeDir(DIR_TOOLS_DATA);

		cmd << "Loading materials lib..." << endl;

		zFILE* f = zfactory->CreateZFile(MATLIB_FILENAME);
		if (f->Exists())
		{
			//zERR_MESSAGE(7, 0, "B: SPC: Configuration: Loading file " + MATLIB_FILENAME);
			if (f->Open(false) == zERR_NONE)
			{
				// Liste leeren
				matFilterList.EmptyList();

				// Trash-Filter erzuegen
				spcCMatFilter* item = new spcCMatFilter;
				item->Init(NO_FILTER_FOLDER, spcCMatFilter::NullLib);
				matFilterList.Insert(item);

				// In der INI-Datei vorkommende Filter einfuegen
				zSTRING line;
				zSTRING matName;

				//cmd << "Cycle" << endl;

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
						zSTRING idStr = line;

						//zERR_MESSAGE(7, 0, "B: SPC: Configuration: MaterialFilter " + item->name + ", #" + zSTRING(item->id));
						item = new spcCMatFilter;
						item->Init(matName.ToChar(), idStr.ToInt32());
						matFilterList.Insert(item);
						//cmd << "LoadMatlib: " << matName.ToChar() << endl;
						LoadMatlib(matName.ToChar());
					}
				} while (!f->Eof());
				f->Close();
			}
			else
			{
				//zERR_WARNING("B: SPC: Configuration: Loading file " + MATLIB_FILENAME + " failed.");
			};
			delete f;
			f = 0;
		}
		else
		{
			cmd << "B: SPC: Configuration: Loading file " << MATLIB_FILENAME << " failed. Not found!" << endl;
			//zERR_WARNING("B: SPC: Configuration: Loading file " + MATLIB_FILENAME + " failed. Not found!");
		};

		zoptions->ChangeDir(DIR_TOOLS_DATA);
		f = zfactory->CreateZFile(MATLIB_FILENAME);

		zerr->Separator("LOADENK METRIAL HUILTERS");
		if (f->Exists())
		{
			if (f->Open(false) == zERR_NONE)
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
						LoadMatlib(matName.ToChar());
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

	/*
	void LoadMatLib()
	{
	// Materialib.ini laden

	zoptions->ChangeDir(DIR_TOOLS_DATA);
	zFILE* f = zfactory->CreateZFile("matlib.ini");
	if (f->Exists())
	{

	if (f->Open(false) == zERR_NONE)
	{
	// In der INI-Datei vorkommende Filter einfuegen
	zSTRING line;
	zSTRING matName;
	do
	{
	f->Read(line);

	if (line.Search("#") > 0)
	{
	if (line.Search("\"") >= 0)
	{
	line.Delete("\"", zSTR_TO);
	matName = line.PickWord(1, "\"", "\"");
	}
	else
	{
	matName = line.PickWord(1, "=", "=");
	matName.TrimLeft();
	matName.TrimRight();
	};
	line.Delete("#", zSTR_TO);
	zSTRING idStr = line;

	ar = zarcFactory->CreateArchiverRead(matName + ".pml", 0);
	cmd << "MatName: " << matName << endl;
	if (!ar)
	return;
	while (!ar->EndOfArchive())
	arrMat.Insert((zCMaterial*)ar->ReadObject());
	ar->Close();
	ar->Release();

	}
	} while (!f->Eof());
	f->Close();

	}
	else
	{
	//zERR_WARNING("B: SPC: Configuration: Loading file matlib.ini failed.");
	};
	delete f;
	f = 0;
	}
	else
	{
	//zERR_WARNING("B: SPC: Configuration: Loading file matlib.ini failed. Not found!");
	};

	}

	zCMaterial* Search(zSTRING name)
	{
	for (int i = 0; i < arrMat.GetNum(); i++)
	if (arrMat.GetSafe(i)->objectName == name)
	return arrMat.GetSafe(i);
	return NULL;
	};

	struct zTPMProtoDirEntry
	{
	DWORD		offset;
	DWORD		size;
	void Set(DWORD _offset, DWORD _size) { offset = _offset; size = _size; };
	};

	struct zTPMProtoDirectoryMain
	{
	zTPMProtoDirEntry	posList;
	zTPMProtoDirEntry	posNormalList;

	void TransferFromProto(zCProgMeshProto *pmProto)
	{
	#define SetDirEntry(FIELD) FIELD.Set ((BYTE*)pmProto->FIELD.GetArray() - pmProto->dataPool, pmProto->FIELD.GetNum());
	SetDirEntry(posList);
	SetDirEntry(posNormalList);
	#undef SetDirEntry
	};
	void TransferToProto(zCProgMeshProto *pmProto)
	{
	#define GetFromEntry(FIELD) pmProto->FIELD.SetArray ((BYTE*)DWORD(pmProto->dataPool) + DWORD(FIELD.offset), FIELD.size);
	GetFromEntry(posList);
	GetFromEntry(posNormalList);
	#undef GetFromEntry
	};
	};

	struct zTPMProtoDirectorySubMesh
	{
	zTPMProtoDirEntry	triList;
	zTPMProtoDirEntry	wedgeList;
	zTPMProtoDirEntry	colorList;
	zTPMProtoDirEntry	triPlaneIndexList;
	zTPMProtoDirEntry	triPlaneList;
	zTPMProtoDirEntry	wedgeMap;
	zTPMProtoDirEntry	vertexUpdates;
	zTPMProtoDirEntry	triEdgeList;
	zTPMProtoDirEntry	edgeList;
	zTPMProtoDirEntry	edgeScoreList;

	void TransferFromProto(zCProgMeshProto *pmProto, zCProgMeshProto::zCSubMesh *subMesh)
	{
	#define SetDirEntry(FIELD) FIELD.Set ((BYTE*)subMesh->FIELD.GetArray()- pmProto->dataPool, subMesh->FIELD.GetNum());
	SetDirEntry(triList);
	SetDirEntry(wedgeList);
	SetDirEntry(colorList);
	SetDirEntry(triPlaneIndexList);
	SetDirEntry(triPlaneList);
	SetDirEntry(wedgeMap);
	SetDirEntry(vertexUpdates);
	SetDirEntry(triEdgeList);
	SetDirEntry(edgeList);
	SetDirEntry(edgeScoreList);
	#undef SetDirEntry
	};
	void TransferToProto(zCProgMeshProto *pmProto, zCProgMeshProto::zCSubMesh *subMesh)
	{
	#define GetFromEntry(FIELD) subMesh->FIELD.SetArray ((BYTE*)DWORD(pmProto->dataPool) + DWORD(FIELD.offset), FIELD.size);
	GetFromEntry(triList);
	GetFromEntry(wedgeList);
	GetFromEntry(colorList);
	GetFromEntry(triPlaneIndexList);
	GetFromEntry(triPlaneList);
	GetFromEntry(wedgeMap);
	GetFromEntry(vertexUpdates);
	GetFromEntry(triEdgeList);
	GetFromEntry(edgeList);
	GetFromEntry(edgeScoreList);
	#undef GetFromEntry
	};
	};
	/*

	zBOOL zCProgMeshProto::spcSave(zCFileBIN& file)
	{
	if (!bMatLibLoaded)
	{
	LoadMatLib();
	bMatLibLoaded = true;
	}
	//
	file.BinStartChunk(zFCHUNK_PROGMESH);
	{
	// ==============
	//	HEADER
	// ==============

	file.BinWriteWord(zCProgMeshProto::GetFileVersion());

	// ==============
	//	DATA
	// ==============

	// dataPool
	{
	file.BinWriteDWord(dataSize);
	file.BinWrite(dataPool, dataSize);
	};

	// dataPool - Directorys
	{
	file.BinWriteByte(numSubMeshes);
	zTPMProtoDirectoryMain			protoDir;
	protoDir.TransferFromProto(this);
	file.BinWrite(&protoDir, sizeof(protoDir));

	zTPMProtoDirectorySubMesh subMeshDir;
	for (int i = 0; i < numSubMeshes; i++)
	{
	subMeshDir.TransferFromProto(this, &(subMeshList[i]));
	file.BinWrite(&subMeshDir, sizeof(subMeshDir));
	};
	};

	// materialList
	{
	zCArchiver *arc = zarcFactory->CreateArchiverWrite(file.file, zARC_MODE_BINARY, FALSE, zARC_FLAG_WRITE_BRIEF_HEADER | zARC_FLAG_NO_SPY_MESSAGES); // noSaveGame, briefHeader
	//zERR_ASSERT_STRICT(arc);
	for (int i = 0; i < numSubMeshes; i++)
	{
	zSTRING sMatName;
	if (!(file.file->GetExt() == zSTRING("MRM").ToChar()))
	sMatName = file.file->GetFilename();
	else
	sMatName = file.file->GetFilename();


	if (Search(sMatName))
	{
	arc->WriteString("", sMatName);
	arc->WriteObject(Search(sMatName));
	}
	else
	{
	arc->WriteString("", sMatName);
	arc->WriteObject(subMeshList[i].material);
	}
	};
	arc->WriteBool("", GetAlphaTestingEnabled());
	arc->Close();
	zRELEASE(arc);
	};

	// misc
	{
	file.BinWrite(&bbox3D, sizeof(bbox3D));
	obbox3D.SaveBIN(file);
	file.BinWrite(&lodParams, sizeof(lodParams));
	};
	};

	// ==============
	//	End-Kennung
	// ==============

	file.BinStartChunk(zFCHUNK_PROGMESH_END);

	// cleanup
	file.BinEndChunk();			// kein BinClose() !

	return TRUE;
	}
	*/
}