// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .



	// This code allows to skip creating OBBOX tree for 3ds LOCATION file, it saves about ~30% of 3ds load time
	// don't use for vobs! 3DS mesh location only!
	bool dontCreateOBBOXOnLocationLoad = false;


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

#if ENGINE >= Engine_G2

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