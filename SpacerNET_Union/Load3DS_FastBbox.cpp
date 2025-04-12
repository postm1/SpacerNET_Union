// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

#if ENGINE >= Engine_G2
	void FlushMeshBuffer_Union();
	extern void ClearVertexMap();

	// main pointers to engine's vars
	static zCOLOR*& actColor = *(zCOLOR**)0x8D85E0;
	static zCOLOR& dummyCol = *(zCOLOR*)0x8D85D0;
	static int& numPoly = *(int*)0x8D85DC;
	static int& numVert = *(int*)0x8D85C8;
	static zCMesh*& targetMesh = *(zCMesh**)0x8D85D8;

	//pointer for ChunkReader = loading 3ds function, reading from the disk
	typedef void (*ChunkReaderFunc)(zFILE*, int, long);
	ChunkReaderFunc ChunkReader = (ChunkReaderFunc)0x55CBC0;

	bool isLoading3DSNow = false;

	static void Load3dsFile(const zSTRING& fileName) {
		long	p;
		zFILE* f = zfactory->CreateZFile(fileName);

		//
		if (f->Exists())
		{
			f->Open(0);
			// Find file size.
			p = f->Size();

			// Go!
			ChunkReader(f, 0, p);
			f->Close();
		}
		else {
			zERR_FATAL("D: zFile3d(::Load3dsFile): Can't open " + fileName);
		};

		//
		delete f;
	}



	// This code allows to skip creating OBBOX tree for 3ds LOCATION file, it saves about ~30% of 3ds load time
	// don't use for vobs! 3DS mesh location only!
	bool dontCreateOBBOXOnLocationLoad = false;

	HOOK ivk_zCFile3DS_Load3DS AS(&zCFile3DS::Load3DS, &zCFile3DS::Load3DS_Union);

	void zCFile3DS::Load3DS_Union(zSTRING const& fileName, zCMesh* tMesh)
	{
		bool debugInfo = true;

		if (debugInfo) cmd << "Load3DS_Union: " << fileName;


		dontCreateOBBOXOnLocationLoad = theApp.options.GetIntVal("bFastLoad3DSLocation") && (theApp.isMergingMeshNow || theApp.isLoadingMeshNow);


		if (dontCreateOBBOXOnLocationLoad)
		{
			(callVoidFunc)GetProcAddress(theApp.module, "ShowSkip3DSWarning")();

			if (debugInfo)
			{
				cmd << " (--- Skip OBBOX)";
			}
		}

		if (debugInfo) cmd << endl;

		//THISCALL(ivk_zCFile3DS_Load3DS)(fileName, tMesh);

		
		zERR_MESSAGE(3, zERR_BEGIN, "D: Loading 3DS-File: \"" + fileName + "\" ... ");

		// hohe FPU Precision sicherstellen
		const zDWORD fpuControlWord = zfpuControler->GetCurrentControlWord();
		zfpuControler->SetPrecision_53();

		// init
		targetMesh = tMesh;
		numVert = 0;
		numPoly = 0;
		actColor = &dummyCol;

		ClearVertexMap();
		isLoading3DSNow = true;
		cmd << "Load 3ds from disk..." << endl;
		Load3dsFile(fileName);
		cmd << "Load 3ds end..." << endl;

		FlushMeshBuffer_Union();

		ClearVertexMap();

		isLoading3DSNow = false;

		tMesh->CalcBBox3D(0);
		
		tMesh->CalcVertexNormals(zCMesh::zMSH_VERTNORMAL_FACET, 0);

		tMesh->PrelightMesh(0);

		zPATH path(fileName);
		tMesh->meshName = (path.GetFilename() + "." + path.GetExt());


		zERR_MESSAGE(3, zERR_END, "Load3DS_Union Completed");

		zfpuControler->SetControlWord(fpuControlWord);
		

		cmd << "Load3DS_Union Completed" << endl;

		dontCreateOBBOXOnLocationLoad = false;
	}



	HOOK ivk_zCMesh_CalcBBox3D AS(&zCMesh::CalcBBox3D, &zCMesh::CalcBBox3D_Union);
	void zCMesh::CalcBBox3D_Union(const zBOOL fastApprox)
	{
		ArraysToLists();
		UnshareFeatures();

		// if dontCreateOBBOXOnLocationLoad => use usual AABB for fast loading a location
		if (fastApprox || dontCreateOBBOXOnLocationLoad)
		{

			zERR_MESSAGE(3, 0, "D: ... CalcBBox3D_Union");

			bbox3D.Init();
			for (int vertCtr = 0; vertCtr < numVert; vertCtr++) {
				zCVertex* vert = vertList[vertCtr];
				for (int j = 0; j < 3; j++) {
					bbox3D.mins[j] = zMin(bbox3D.mins[j], vert->position.n[j]);
					bbox3D.maxs[j] = zMax(bbox3D.maxs[j], vert->position.n[j]);
				};
			};

			zERR_MESSAGE(3, 0, "D: ... CalcBBox3D_Union FINISHED...");
		}
		else {
			obbox3D.BuildOBBTree(this, 3);
			bbox3D = obbox3D.GetBBox3D();
		};
		//
		if ((numVert == 0) || (numPoly == 0)) {
			zREAL D = 0.1F;
			bbox3D.mins = -zVEC3(D, D, D);
			bbox3D.maxs = zVEC3(D, D, D);
		};
	};
#endif

#if ENGINE == Engine_G1

	HOOK ivk_zCFile3DS_Load3DS AS(&zCFile3DS::Load3DS, &zCFile3DS::Load3DS_Union);

	void zCFile3DS::Load3DS_Union(zSTRING const& fileName, zCMesh* tMesh)
	{
		bool debugInfo = false;

		if (debugInfo) cmd << "Load3DS_Union: " << fileName;


		dontCreateOBBOXOnLocationLoad = theApp.options.GetIntVal("bFastLoad3DSLocation") && (theApp.isMergingMeshNow || theApp.isLoadingMeshNow);


		if (dontCreateOBBOXOnLocationLoad)
		{
			(callVoidFunc)GetProcAddress(theApp.module, "ShowSkip3DSWarning")();

			if (debugInfo)
			{
				cmd << " (--- Skip OBBOX)";
			}
		}


		if (debugInfo) cmd << endl;

		THISCALL(ivk_zCFile3DS_Load3DS)(fileName, tMesh);

		dontCreateOBBOXOnLocationLoad = false;
	}

	HOOK ivk_zCMesh_CalcBBox3D AS(&zCMesh::CalcBBox3D, &zCMesh::CalcBBox3D_Union);
	void zCMesh::CalcBBox3D_Union(const zBOOL bGreat)
	{

		UnshareFeatures();
		if (bGreat || dontCreateOBBOXOnLocationLoad)
		{
			bbox3D.Init();
			for (int i = 0; i < numVert; i++)
				bbox3D.AddPoint(vertList[i]->position);
		}
		else
		{
			obbox3D.BuildOBBTree(this, 3);
			bbox3D = obbox3D.GetBBox3D();
		}
		if (numVert && numPoly)
			return;

		zREAL D = 0.1F;
		bbox3D.mins = -zVEC3(D, D, D);
		bbox3D.maxs = zVEC3(D, D, D);
	};

#endif
}