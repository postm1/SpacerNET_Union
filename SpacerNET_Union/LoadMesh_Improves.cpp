// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	inline float zAbs(const float a)
	{
		return fabs(a);
	}

	static int Compare_Verts_Merge(const void* arg1, const void* arg2)
	{
		const zVALUE	ALMOST_ZERO = 0.01F;		// = cm
		const zVEC3		p1 = (*((zCVertex**)arg1))->position;
		const zVEC3		p2 = (*((zCVertex**)arg2))->position;
		const zREAL		p1x = p1.n[VX];
		const zREAL		p1y = p1.n[VY];
		const zREAL		p1z = p1.n[VZ];
		const zREAL		p2x = p2.n[VX];
		const zREAL		p2y = p2.n[VY];
		const zREAL		p2z = p2.n[VZ];

		// erster test auf gleichheit
		if ((zAbs(p1x - p2x) < ALMOST_ZERO) &&
			(zAbs(p1y - p2y) < ALMOST_ZERO) &&
			(zAbs(p1z - p2z) < ALMOST_ZERO)) return 0;			// equal	

		if ((p1x - p2x) < 0) return -1;
		else if ((p1x - p2x) > 0) return +1;
		else
		{
			// die x koords sind gleich, dann die y coords vergleichen
			if ((p1y - p2y) < 0) return -1;
			else if ((p1y - p2y) > 0) return +1;
			else
			{
				// die x und die y coords sind gleich, kann dann nur noch z unterschiedlich sein
				if ((p1z - p2z) < 0) return -1;
				else if ((p1z - p2z) > 0) return +1;
			}
		}

		//zERR_FAULT("C: Compare_Verts_Merge: Bug!!! should be never here!");
		// hier darf man niemals hinkommen
		return 0;

	}


	void __cdecl zCMesh_S_InitVertexMergeCache(zCMesh*);
	CInvoke <void(__cdecl*) (zCMesh*)> Invk_zCMesh_S_InitVertexMergeCache_Union(0x005680A0, zCMesh_S_InitVertexMergeCache, IVK_AUTO);
	void __cdecl zCMesh_S_InitVertexMergeCache(zCMesh* a_pMesh)
	{
		zERR_MESSAGE(6, 0, "C: Meshmagic: Initializing cache for vertex merging");
		zCMesh::s_vertexCache.DeleteList();

		zCMesh::s_vertexCache.SetCompare(Compare_Verts_Merge);
		// alle gespeicherten verts (falls vorhanden) 
		// in die neue sortierte liste kopieren

		cmd << "S_InitVertexMergeCache_Union: " << a_pMesh->numFeat << endl;

		for (int i = 0; i < a_pMesh->numVert; i++)
		{
			zCMesh::s_vertexCache.InsertEnd(a_pMesh->vertList[i]);
		}

		zCMesh::s_vertexCache.BestSort();

		zCMesh::s_usesVertexCache = TRUE;
	}



	static zCMesh* targetMesh;
	static TVertex3ds* vertList;
	static TPoly3ds* polyList;
	static int				numVert;
	static int				numPoly;

	static zCMaterial* actMat;
	static zCOLOR* actColor;
	static zCOLOR			dummyCol;

	static const int NNX = 0;		// 0,2,1 für original 3ds
	static const int NNY = 2;		// 0,1,2 für z.B. Landscape
	static const int NNZ = 1;

	
	
	typedef unsigned char  byte;
	typedef unsigned short word;
	typedef unsigned long  dword;

	typedef signed char  sbyte;
	typedef signed short sword;
	typedef signed long  sdword;

#pragma pack(push,2)

	typedef struct {
		word    id;
		dword   len;
	} TChunkHeader, * PChunkHeader;

#pragma pack(pop)

	enum {
		CHUNK_M3DVERS = 0x0002,
		CHUNK_RGBF = 0x0010,
		CHUNK_RGBB = 0x0011,
		CHUNK_RGBB2 = 0x0012,       // ?? NOT HLS.

		CHUNK_SHORTPERC = 0x0030,

		CHUNK_PRJ = 0xC23D,
		CHUNK_MLI = 0x3DAA,

		CHUNK_MAIN = 0x4D4D,
		CHUNK_OBJMESH = 0x3D3D,
		CHUNK_MESHVERS = 0x3D3E,				// NEU
		// hier die Material-Enum
		CHUNK_MSTRSCALE = 0x0100,				// NEU
		CHUNK_BKGCOLOR = 0x1200,
		CHUNK_AMBCOLOR = 0x2100,
		CHUNK_OBJBLOCK = 0x4000,
		CHUNK_TRIMESH = 0x4100,
		CHUNK_VERTLIST = 0x4110,
		CHUNK_FACELIST = 0x4120,
		CHUNK_FACEMAT = 0x4130,
		CHUNK_MAPLIST = 0x4140,
		CHUNK_TEXINFO = 0x4170,
		CHUNK_SMOOLIST = 0x4150,
		CHUNK_TRMATRIX = 0x4160,
		CHUNK_LIGHT = 0x4600,
		CHUNK_SPOTLIGHT = 0x4610,
		CHUNK_CAMERA = 0x4700,
		CHUNK_HIERARCHY = 0x4F00,
		CHUNK_VIEWPORT = 0x7001,
		CHUNK_MATERIAL = 0xAFFF,
		CHUNK_MATNAME = 0xA000,
		CHUNK_AMBIENT = 0xA010,
		CHUNK_DIFFUSE = 0xA020,
		CHUNK_SPECULAR = 0xA030,
		CHUNK_SHININESS = 0xA040,				// NEU
		CHUNK_SHIN2PCT = 0xA041,				// NEU
		CHUNK_TRANSP = 0xA050,				// NEU
		CHUNK_SHADING = 0xA100,				// NEU
		CHUNK_TEXTURE = 0xA200,
		// CHUNK_SHORTPERC					// TextureMap strength
		CHUNK_MAPFILE = 0xA300,			// 8+3
		CHUNK_MAPTILING = 0xA351,
		CHUNK_MAPTEXBLUR = 0xA353,
		CHUNK_BUMPMAP = 0xA230,

		CHUNK_KEYFRAMER = 0xB000,
		CHUNK_AMBIENTKEY = 0xB001,
		CHUNK_TRACKINFO = 0xB002,
		CHUNK_TRACKOBJNAME = 0xB010,
		CHUNK_TRACKPIVOT = 0xB013,
		CHUNK_TRACKPOS = 0xB020,
		CHUNK_TRACKROTATE = 0xB021,
		CHUNK_TRACKSCALE = 0xB022,
		CHUNK_OBJNUMBER = 0xB030,
		CHUNK_TRACKCAMERA = 0xB003,
		CHUNK_TRACKFOV = 0xB023,
		CHUNK_TRACKROLL = 0xB024,
		CHUNK_TRACKCAMTGT = 0xB004,
		CHUNK_TRACKLIGHT = 0xB005,
		CHUNK_TRACKLIGTGT = 0xB006,
		CHUNK_TRACKSPOTL = 0xB007,
		CHUNK_FRAMES = 0xB008,
	};

	// ------------------------------------

	void TPoly3ds::CheckForWrap() {
		return;

		const int u = 0;
		const int v = 1;
		zPOINT2& tex0 = vertList[vertIndex[0]].texCoord;
		zPOINT2& tex1 = vertList[vertIndex[1]].texCoord;
		zPOINT2& tex2 = vertList[vertIndex[2]].texCoord;
		float d;
		//	wrapu=wrapv=TRUE;
		if (wrapu) {
			float maxu, minu;
			maxu = minu = tex0[u]; //tv[0].u; 

			if (tex1[u] > maxu) maxu = tex1[u]; else
				if (tex1[u] < minu) minu = tex1[u];
			if (tex2[u] > maxu) maxu = tex2[u]; else
				if (tex2[u] < minu) minu = tex2[u];
			if ((maxu - minu) > 0.8) {
				d = ceil(maxu - minu);
				if (tex0[u] < .5) tex0[u] += d;
				if (tex1[u] < .5) tex1[u] += d;
				if (tex2[u] < .5) tex2[u] += d;
			}
		}
		if (wrapv) {
			float maxv, minv;
			maxv = minv = tex0[v];
			if (tex1[v] > maxv) maxv = tex1[v]; else
				if (tex1[v] < minv) minv = tex1[v];
			if (tex2[v] > maxv) maxv = tex2[v]; else
				if (tex2[v] < minv) minv = tex2[v];
			if ((maxv - minv) > 0.8) {
				d = ceil(maxv - minv);
				if (tex0[v] < .5) tex0[v] += d;
				if (tex1[v] < .5) tex1[v] += d;
				if (tex2[v] < .5) tex2[v] += d;
			}
		}
	};

	// ------------------------------------

		// Forward declaration.
	static void ChunkReader(zFILE* f, int ind, long p);

	static void SkipReader(zFILE* f, int ind, long p) {
	}

	static void VersionReader(zFILE* f, int ind, long p) {
		long v;
		if (f->Read(&v, sizeof(v)) <= 0) return;
		//	Print ("%*sVersion: %d", ind, "", v);
		//Print(ind, "Version: " + zSTRING(v));
	}

	static void RGBFReader(zFILE* f, int ind, long p) {
		float c[3];
		if (f->Read(&c, sizeof(c)) <= 0) return;
		//printf("%*s    Red: %f, Green: %f, Blue: %f", ind, "", c[0], c[1], c[2]);
	}

	static void RGBBReader(zFILE* f, int ind, long p) {
		byte c[3];
		if (f->Read(&c, sizeof(c)) <= 0) return;
		//	Print ("%*s    Red: %d, Green: %d, Blue: %d", ind, "", c[0], c[1], c[2]);
		//Print(ind, "RGB: " + zSTRING(c[0]) + "," + zSTRING(c[1]) + "," + zSTRING(c[2]));

		// ----------ENGINE---------------------------------- 
		actColor->r = c[0];
		actColor->g = c[1];
		actColor->b = c[2];
	}

	static zSTRING ASCIIZReader(zFILE* f, int ind, long p) {
		char	c;
		zSTRING s;
		s.Clear();

		// Read ASCIIZ name
		while ((f->Read(&c, sizeof(c)) > 0) && c != '\0')
			//	while ( (f->Read (&c, sizeof(c))) != EOF && c != '\0')
			//	while ( (c = fgetc(f)) != EOF && c != '\0')
		{
			s = s + zSTRING(char(c));
		};
		return s;
	}

	// ----------ENGINE---------------------------------- BEGIN
	static void AmbColReader(zFILE* f, int ind, long p) {
		//actColor = &(actMat->color);
		actColor = &dummyCol;
		ChunkReader(f, ind, p);
	}

	static void DiffColReader(zFILE* f, int ind, long p) {
		actColor = &(actMat->color);
		ChunkReader(f, ind, p);
	}

	static void SpecColReader(zFILE* f, int ind, long p) {
		actColor = &dummyCol;
		ChunkReader(f, ind, p);
	}

	void FlushMeshBuffer();

	// ----------ENGINE---------------------------------- END

	static void ObjBlockReader(zFILE* f, int ind, long p) {
		// create New Object
		FlushMeshBuffer();

		// Read ASCIIZ object name
		zSTRING s = ASCIIZReader(f, ind, p);
		//	Print ("%*sObject name \"%s\"", ind, "", s.ToChar());
		//Print(ind, "Object name: " + s);
		// Read rest of chunks inside this one.
		ChunkReader(f, ind, p);
	}

	static void VertListReader(zFILE* f, int ind, long p) {
		word nv;
		float c[3];

		if (f->Read(&nv, sizeof(nv)) <= 0) return;
		//	Print ("%*sVertices: %d", ind, "", nv);
		//Print(ind, "numVert:  " + zSTRING(nv));
		// ----------ENGINE----------------------------------
		numVert = nv;
		vertList = zNEW(TVertex3ds)[numVert];
		nv = 0;
		while (nv < numVert) {
			if (f->Read(&c, sizeof(c)) <= 0) return;
			//		Print ("%*s    X: %f, Y: %f, Z: %f", ind, "", c[0], c[1], c[2]);
			vertList[nv].position.n[0] = (zVALUE)(c[NNX]);
			vertList[nv].position.n[1] = (zVALUE)(c[NNY]);
			vertList[nv].position.n[2] = (zVALUE)(c[NNZ]);
			vertList[nv].texCoord = zVEC2(0.0F, 0.0F);
			nv++;
		}
	}

	static void FaceListReader(zFILE* f, int ind, long p) {
		word nv;
		word c[3];
		word flags;

		if (f->Read(&nv, sizeof(nv)) <= 0) return;
		//	Print ("%*sFaces: %d", ind, "", nv);
		//Print(ind, "numTris: " + zSTRING(nv));
		// ----------ENGINE----------------------------------
		numPoly = nv;
		polyList = zNEW(TPoly3ds)[numPoly];
		nv = 0;
		while (nv < numPoly) {
			if (f->Read(&c, sizeof(c)) <= 0) return;
			if (f->Read(&flags, sizeof(flags)) <= 0) return;
			//Print ("%*s  A %d, B %d, C %d, 0x%X:", ind, "", c[0], c[1], c[2], flags);
	//        printf("%*s    AB: %d, BC: %d, CA: %d, UWrap %d, VWrap %d",
	//               ind, "",
	//        Print (" AB %d BC %d CA %d UWrap %d VWrap %d",
	//               (flags & 0x04) != 0, (flags & 0x02) != 0, (flags & 0x01) != 0,
	//               (flags & 0x08) != 0, (flags & 0x10) != 0);
			for (int i = 0; i < 3; i++)
			{
				if (c[i] >= numVert)
				{
					//zERR_FAULT("D: zFile3DS(FaceListReader): FILE CORRUPT, face has index to non-existant vertex index: " + zSTRING(c[i]) + ", numVert: " + zSTRING(numVert));
					c[i] = 0;
				};
			};

			polyList[nv].vertIndex[0] = c[0];
			polyList[nv].vertIndex[1] = c[1];
			polyList[nv].vertIndex[2] = c[2];
			polyList[nv].material = 0;
			polyList[nv].wrapu = (flags & 0x08) != 0;
			polyList[nv].wrapv = (flags & 0x10) != 0;
			nv++;
		}
		// Read rest of chunks inside this one.
		ChunkReader(f, ind, p);
	}

	static void FaceMatReader(zFILE* f, int ind, long p) {
		word n, nf;

		// ----------ENGINE----------------------------------

		// Read ASCIIZ material name
		zSTRING s = ASCIIZReader(f, ind, p);
		//zerr.Message (" mat: "+s);
		//	Print ("%*sMaterial name for faces: \"%s\"", ind, "", s.ToChar());
		//Print(ind, "Material name for faces: " + s);
		s.Upper();
		actMat = zCMaterial::SearchName(s);
		if (actMat == 0)
			//zERR_FATAL("D: File3d.FaceMatReader: ! noper !");

		if (f->Read(&n, sizeof(n)) <= 0) return;
		// printf("%*sFaces with this material: %d", ind, "", n);
		while (n-- > 0) {
			if (f->Read(&nf, sizeof(nf)) <= 0) return;
			// printf("%*s    Face %d", ind, "", nf);
			if (nf >= numPoly) {
				//zERR_FAULT("D: zFile3DS(FaceMatReader): FILE CORRUPT, material '" + s + "' is assigned to non-existant poly, index: " + zSTRING(nf));
			}
			else
				polyList[nf].material = (zCMaterial*)(actMat->AddRef());
		}
	}

	//index = 0
	//num = 1

	static void MapListReader(zFILE* f, int ind, long p) {
		// ----------ENGINE----------------------------------
		word nv;
		float c[2];

		if (f->Read(&nv, sizeof(nv)) <= 0) return;
		//	Print ("%*sVertices: %d", ind, "", nv);
		//Print(ind, "numVert: " + zSTRING(nv));


		if (nv > numVert) {
			//zERR_FAULT("D: zFile3DS(MapListReader): FILE CORRUPT, texture-coords assign to non-existant vertex");
			nv = numVert;
		};

		word ctr = 0;
		while (ctr < nv) {
			if (f->Read(&c, sizeof(c)) <= 0) return;
			//		Print ("%*s    U: %f, V: %f", ind, "", c[0], c[1]);
			vertList[ctr].texCoord[VX] = c[0];
			vertList[ctr].texCoord[VY] = 1.0F - c[1];					// invert V/Y Axis
			ctr++;
		}
	}

	static void SmooListReader(zFILE* f, int ind, long p) {
		dword s;
		int i;

		while (f->Pos() < p)
		{
			if (f->Read(&s, sizeof(s)) <= 0) return;
			//Print ("%*sSmoothing groups: ", ind, "");
			for (i = 0; i < 32; i++)
				if (s & (1 << i)) {
					//Print ("%d, ", i + 1);
				}
			//Print ("");
		}
	}

	static void TrMatrixReader(zFILE* f, int ind, long p) {
		float rot[9];
		float trans[3];
		if (f->Read(&rot, sizeof(rot)) <= 0) return;
		//printf("%*sRotation matrix:", ind, "");
		//printf("%*s    %f, %f, %f", ind, "", rot[0], rot[1], rot[2]);
		//printf("%*s    %f, %f, %f", ind, "", rot[3], rot[4], rot[5]);
		//printf("%*s    %f, %f, %f", ind, "", rot[6], rot[7], rot[8]);
		if (f->Read(&trans, sizeof(trans)) <= 0) return;
		//printf("%*sTranslation matrix: %f, %f, %f", ind, "", trans[0], trans[1], trans[2]);
	}

	static void LightReader(zFILE* f, int ind, long p) {
		float c[3];
		if (f->Read(&c, sizeof(c)) <= 0) return;
		// printf("%*s    X: %f, Y: %f, Z: %f", ind, "", c[0], c[1], c[2]);
			// Read rest of chunks inside this one.
		ChunkReader(f, ind, p);
	}

	static void SpotLightReader(zFILE* f, int ind, long p) {
		float c[5];
		if (f->Read(&c, sizeof(c)) <= 0) return;
		// printf("%*s    Target X: %f, Y: %f, Z: %f; Hotspot %f, Falloff %f", ind, "", c[0], c[1], c[2], c[3], c[4]);
	}

	static void CameraReader(zFILE* f, int ind, long p) {
		float c[8];
		if (f->Read(&c, sizeof(c)) <= 0) return;
		//printf("%*s    Position: X: %f, Y: %f, Z: %f", ind, "", c[0], c[1], c[2]);
		//printf("%*s    Target: X: %f, Y: %f, Z: %f", ind, "", c[3], c[4], c[5]);
		//printf("%*s    Bank: %f, Lens: %f", ind, "", c[6], c[7]);
	}

	static void MatNameReader(zFILE* f, int ind, long p) {
		// Read ASCIIZ object name

		// ----------ENGINE----------------------------------
		// create new material
		zSTRING s = ASCIIZReader(f, ind, p);
		//	Print ("%*sMaterial name \"%s\"", ind, "", s.ToChar());
		//Print(ind, "Material name: " + s);
		//zerr.Message (" newMat: "+s);
		s.Upper();
		//	if (zCMaterial::GetUsageDefault	()!=zCMaterial::zMAT_USAGE_LEVEL) 
		//		s += 

		actMat = zCMaterial::SearchName(s);
		if (!actMat) {
			actMat = zNEW(zCMaterial(s));
			//		actMat->SubRef();				// ist ja noch nicht von Poly referenziert worden !
		};
	}

	static void MapFileReader(zFILE* f, int ind, long p) {
		// Read ASCIIZ filename
		zSTRING s = ASCIIZReader(f, ind, p);
		//	Print ("%*sMap filename \"%s\"", ind, "", s.ToChar());
		//Print(ind, "Texture name: " + s);
		// ----------ENGINE----------------------------------
		// Material bekommt ein Texture
		//zerr.Message (s);
		actMat->SetTexture(s);
	}

	static void FramesReader(zFILE* f, int ind, long p) {
		dword c[2];
		if (f->Read(&c, sizeof(c)) <= 0) return;
		//printf("%*s    Start: %ld, End: %ld", ind, "", c[0], c[1]);
	}

	static void TrackObjNameReader(zFILE* f, int ind, long p) {
		word w[2];
		word parent;

		// Read ASCIIZ name
	//printf("%*sTrack object name \"", ind, "");
		ASCIIZReader(f, ind, p);
		if (f->Read(&w, sizeof(w)) <= 0) return;
		if (f->Read(&parent, sizeof(parent)) <= 0) return;
		//printf("%*sObject name data: Flags 0x%X, 0x%X, Parent %d", ind, "", w[0], w[1], parent);
	}

	static void PivotPointReader(zFILE* f, int ind, long p) {
		float pos[3];

		if (f->Read(&pos, sizeof(pos)) <= 0) return;
		//printf("%*s  Pivot at X: %f, Y: %f, Z: %f", ind, "",pos[0], pos[1], pos[2]);
	}

	// Key info flags for position, rotation and scaling:
	// Until I know the meaning of each bit in flags I assume all mean
	// a following float data.

		// NOTE THIS IS NOT A CHUNK, but A PART OF SEVERAL CHUNKS
	static void SplineFlagsReader(zFILE* f, int ind, word flags) {
		/*	int i;
			float dat;

			for (i = 0; i < 16; i++) {
				static const char *flagnames[] = {
					"Tension",
					"Continuity",
					"Bias",
					"Ease To",
					"Ease From",
				};
				if (flags & (1 << i)) {
					if (f->Read(&dat, sizeof(dat), 1, f) <=0) return;
					if (i < sizeof(flagnames)/sizeof(*flagnames)) {
						//printf("%*s             %-15s = %f", ind, "", flagnames[i], dat);
					} else {
						//printf("%*s             %-15s = %f", ind, "", "Unknown", dat);
					}
				}
			}*/
	}

	static void TrackPosReader(zFILE* f, int ind, long p) {
		word n, nf;
		float pos[3];
		word unkown;
		word flags;

		f->Seek(f->Pos() + 10);									//	fseek(f, 10, SEEK_CUR);
		if (f->Read(&n, sizeof(n)) <= 0) return;
		//printf("%*sPosition keys: %d", ind, "", n);
		f->Seek(f->Pos() + 2);									//	fseek(f, 2, SEEK_CUR);
		while (n-- > 0) {
			if (f->Read(&nf, sizeof(nf)) <= 0) return;
			if (f->Read(&unkown, sizeof(unkown)) <= 0) return;
			if (f->Read(&flags, sizeof(flags)) <= 0) return;
			//printf("%*s  Frame %3d: Flags 0x%X", ind, "", nf, flags);
			SplineFlagsReader(f, ind, flags);
			if (f->Read(&pos, sizeof(pos)) <= 0) return;
			//printf("%*s             X: %f, Y: %f, Z: %f", ind, "", pos[0], pos[1], pos[2]);
		}
	}

	static void TrackRotReader(zFILE* f, int ind, long p) {
		word n, nf;
		float pos[4];
		word unkown;
		word flags;

		f->Seek(f->Pos() + 10);									//	fseek(f, 10, SEEK_CUR);
		if (f->Read(&n, sizeof(n)) <= 0) return;
		//printf("%*sRotation keys: %d", ind, "", n);
		f->Seek(f->Pos() + 2);									//	fseek(f, 2, SEEK_CUR);
		while (n-- > 0) {
			if (f->Read(&nf, sizeof(nf)) <= 0) return;
			if (f->Read(&unkown, sizeof(unkown)) <= 0) return;
			if (f->Read(&flags, sizeof(flags)) <= 0) return;
			//printf("%*s  Frame %3d: Flags 0x%X", ind, "", nf, flags);
			SplineFlagsReader(f, ind, flags);
			if (f->Read(&pos, sizeof(pos)) <= 0) return;
			//printf("%*s             Angle: %f§, X: %f, Y: %f, Z: %f", ind, "", pos[0]*180.0/PI, pos[1], pos[2], pos[3]);
		}
	}

	static void TrackScaleReader(zFILE* f, int ind, long p) {
		word n, nf;
		float pos[3];
		word unkown;
		word flags;

		f->Seek(f->Pos() + 10);									//	fseek(f, 2, SEEK_CUR);
		if (f->Read(&n, sizeof(n)) <= 0) return;
		//printf("%*sScale keys: %d", ind, "", n);
		f->Seek(f->Pos() + 2);									//	fseek(f, 2, SEEK_CUR);
		while (n-- > 0) {
			if (f->Read(&nf, sizeof(nf)) <= 0) return;
			if (f->Read(&unkown, sizeof(unkown)) <= 0) return;
			if (f->Read(&flags, sizeof(flags)) <= 0) return;
			//printf("%*s  Frame %3d: Flags 0x%X", ind, "", nf, flags);
			SplineFlagsReader(f, ind, flags);
			if (f->Read(&pos, sizeof(pos)) <= 0) return;
			//printf("%*s            X: %f, Y: %f, Z: %f", ind, "", pos[0], pos[1], pos[2]);
		}
	}

	static void ObjNumberReader(zFILE* f, int ind, long p) {
		word n;

		if (f->Read(&n, sizeof(n)) <= 0) return;
		//printf("%*sObject number: %d", ind, "", n);
	}


	// ------------------------------------

	static void SKIP(zFILE* f, int ind, long p) {
	};

	struct {
		word id;
		const char* name;
		void (*func)(zFILE* f, int ind, long p);
	} ChunkNames[] = {
		{CHUNK_RGBF,        "RGB float",        RGBFReader},
		{CHUNK_RGBB,        "RGB byte",         RGBBReader},
		{CHUNK_RGBB2,       "RGB intern",       SKIP},

		{CHUNK_PRJ,         "Project",          NULL},
		{CHUNK_MLI,         "Material Library", NULL},

		{CHUNK_MAIN,        "Main",             NULL},
			{CHUNK_M3DVERS,     "3DS Version",      VersionReader},
			{CHUNK_OBJMESH,     "Object Mesh",      NULL},
				{CHUNK_MESHVERS,    "Mesh Version",		VersionReader},
				{CHUNK_MSTRSCALE,   "Master Scale",		SKIP},
				{CHUNK_BKGCOLOR,    "Background color", NULL},
				{CHUNK_AMBCOLOR,    "Ambient color",    NULL},
				{CHUNK_OBJBLOCK,    "Object Block",     ObjBlockReader},
					{CHUNK_TRIMESH,     "Tri-Mesh",         NULL},
						{CHUNK_VERTLIST,    "Vertex list",      VertListReader},
						{CHUNK_FACELIST,    "Face list",        FaceListReader},
						{CHUNK_FACEMAT,     "Face material",    FaceMatReader},
						{CHUNK_MAPLIST,     "Mappings list",    MapListReader},
						{CHUNK_TEXINFO,     "Mapping Icon",		SKIP},
						{CHUNK_SMOOLIST,    "Smoothings",       SmooListReader},
						{CHUNK_TRMATRIX,    "Matrix",           TrMatrixReader},
					{CHUNK_LIGHT,       "Light",            SKIP /*LightReader*/},
						{CHUNK_SPOTLIGHT,   "Spotlight",        SpotLightReader},
					{CHUNK_CAMERA,      "Camera",           SKIP /*CameraReader*/},
					{CHUNK_HIERARCHY,   "Hierarchy",        SKIP},

		{CHUNK_VIEWPORT,    "Viewport info",    SKIP},
		{CHUNK_MATERIAL,    "Material",         NULL},
			{CHUNK_MATNAME,     "Material name",    MatNameReader},
			{CHUNK_AMBIENT,     "Ambient color",    AmbColReader},
			{CHUNK_DIFFUSE,     "Diffuse color",    DiffColReader},
			{CHUNK_SPECULAR,    "Specular color",   SpecColReader},
			{CHUNK_SHININESS,   "Shininess",		SKIP},
			{CHUNK_SHIN2PCT,	"Shiny2 Perc",		SKIP},
			{CHUNK_TRANSP,		"Transparency",		SKIP},
			{CHUNK_SHADING,		"Shading",			SKIP},
			{CHUNK_TEXTURE,     "Texture map",      NULL},
				{CHUNK_MAPFILE,     "Map filename",     MapFileReader},
				{CHUNK_MAPTILING,   "Map tiling",		SKIP},
				{CHUNK_MAPTEXBLUR,  "Map textureblur",  SKIP},
			{CHUNK_BUMPMAP,     "Bump map",         NULL},
		{CHUNK_KEYFRAMER,   "Keyframer data",   SKIP},
			{CHUNK_AMBIENTKEY,  "Ambient key",      NULL},
			{CHUNK_TRACKINFO,   "Track info",       NULL},
			{CHUNK_FRAMES,      "Frames",           FramesReader},
			{CHUNK_TRACKOBJNAME,"Track Obj. Name",  TrackObjNameReader},
			{CHUNK_TRACKPIVOT,  "Pivot point",      PivotPointReader},
			{CHUNK_TRACKPOS,    "Position keys",    TrackPosReader},
			{CHUNK_TRACKROTATE, "Rotation keys",    TrackRotReader},
			{CHUNK_TRACKSCALE,  "Scale keys",       TrackScaleReader},
			{CHUNK_OBJNUMBER,   "Object number",    ObjNumberReader},

			{CHUNK_TRACKCAMERA, "Camera track",             NULL},
			{CHUNK_TRACKCAMTGT, "Camera target track",      NULL},
			{CHUNK_TRACKLIGHT,  "Pointlight track",         NULL},
			{CHUNK_TRACKLIGTGT, "Pointlight target track",  NULL},
			{CHUNK_TRACKSPOTL,  "Spotlight track",          NULL},
			{CHUNK_TRACKFOV,    "FOV track",                NULL},
			{CHUNK_TRACKROLL,   "Roll track",               NULL},
	};

	static int FindChunk(word id) {
		int i;
		for (i = 0; i < sizeof(ChunkNames) / sizeof(ChunkNames[0]); i++)
			if (id == ChunkNames[i].id)
				return i;
		return -1;
	}

	// ------------------------------------

	// Der Chunk-Reader fuer Bloecke, denen nicht explizit eine 
	// Auswertungs-Funktion zugeordnet ist.
	//
	// Bloecke, die von hier aus ausgewertet werden, haben Unter-Hierarchien.

	static void ChunkReader(zFILE* f, int ind, long p) {
		TChunkHeader h;
		int n;
		long pc;

		while (f->Pos() < p) {
			pc = f->Pos();
			if (f->Read(&h, sizeof(h)) <= 0) return;
			if (h.len == 0) return;
			n = FindChunk(h.id);
			if (n < 0) {
				//			Print ("%*sUnknown chunk: 0x%04X, offset 0x%lX, size: %d bytes.", ind, "", h.id, pc, h.len);
				//Print(ind, "unknown chunk: " + zSTRING(h.id) + ", offset: " + zSTRING(pc) + ", size: " + zSTRING(h.len));

				f->Seek(pc + h.len);
				//			fseek(f, pc + h.len, SEEK_SET);
			}
			else {
				//			Print ("%*sChunk type \"%s\", size %d bytes",ind, "", ChunkNames[n].name, h.len);
				//Print(ind, "chunk type: " + zSTRING(ChunkNames[n].name) + ", bytes: " + zSTRING(h.len));
				if (ChunkNames[n].func == SKIP) {
					pc = pc + h.len;
					f->Seek(pc);
					//	            fseek(f, pc, SEEK_SET);
				}
				else {
					if (ChunkNames[n].func == NULL) {
						//Print ("%*sChunk type \"%s\", offset 0x%lX, size %d bytes",ind, "", ChunkNames[n].name, pc, h.len);
						//Print ("%*sChunk type \"%s\", size %d bytes",ind, "", ChunkNames[n].name, h.len);
					};
					pc = pc + h.len;
					if (ChunkNames[n].func) ChunkNames[n].func(f, ind + 2, pc);
					else					ChunkReader(f, ind + 2, pc);
					f->Seek(pc);
					//				fseek(f, pc, SEEK_SET);
				};
			}
			//		if (ferror(f))	break;
		}
	}

	static void FlushMeshBuffer() {
		if (numPoly == 0) return;

		//	zerr.Message ("D: 1) Flushing MeshBuffer V: "+zSTRING(numVert)+" P: "+zSTRING(numPoly));
			// make it a mesh
		int i;

		// Vertices
		targetMesh->AllocVerts(numVert);
		zCVertex** newVertList = zNEW(zCVertex*)[numVert];
		zERR_MESSAGE(3, zERR_BEGIN, "D: FlushMeshBuffer (" + zSTRING(numVert) + " verts)");

		for (i = 0; i < numVert; i++) {
			//		newVertList[i]= targetMesh->AddVertex (vertList[i].position);
			newVertList[i] = targetMesh->AddVertexSmart(vertList[i].position);
			if ((i != 0) && ((i & 4095) == 0)) zERR_MESSAGE(3, 0, "D: ... still working (" + zSTRING(i) + " verts)");
		};
		zERR_MESSAGE(3, zERR_END, "");


		// Polys 
		targetMesh->AllocPolys(numPoly);
		zCPolygon* poly;
		for (i = 0; i < numPoly; i++) {
			polyList[i].CheckForWrap();
			poly =
				targetMesh->AddPoly(newVertList[polyList[i].vertIndex[0]],
					newVertList[polyList[i].vertIndex[1]],
					newVertList[polyList[i].vertIndex[2]]);
			// Tex-Coords setzen
			poly->SetMapping(0, vertList[polyList[i].vertIndex[0]].texCoord);
			poly->SetMapping(1, vertList[polyList[i].vertIndex[1]].texCoord);
			poly->SetMapping(2, vertList[polyList[i].vertIndex[2]].texCoord);
			// AddRef wurde oben bereits erledigt, hier lediglich umkopieren
			poly->material = polyList[i].material;
			if (poly->material == 0) {
				//zERR_FATAL("D: zFile3d(): Mesh has polys without material !");
				//			poly->material = zNEW(zCMaterial());
			};

			// Jedes Material, dass hier durchkommt wird noch einmal mit dem DefaultUsage versehen.
			// Falls es in diesem Loader erzeugt worden ist, enthaelt es bereits die richtige Usage aus
			// dem Mat-Constructor. Nur falls es vorher bereits vorhanden war (Vob? Model?) wird es nochmals
			// nachtraeglich geflagt.
			if (poly->material) {
				poly->material->matUsage = poly->material->s_matUsageDefault;
				//			poly->material->SetSmooth (FALSE);			// DARF NICHT !! Sonst wird ein .PMF ueberschrieben !!
			};

			// nicht hier !! sonst funktioniert 'ConvertTrisToNPolys()' nicht korrekt !!
			//poly->TexCorrectUV();


			//	if (poly->material!=0) poly->material->texture = zCTexture :: Load(TEX_NAME);
			//		poly->SetSmoothing (FALSE);
			/*		poly->SetMapping (vertList [ polyList[i].vertIndex[0] ].texCoord,
									  vertList [ polyList[i].vertIndex[1] ].texCoord,
									  vertList [ polyList[i].vertIndex[2] ].texCoord);*/
		};

		delete[] newVertList;
		delete[] polyList;
		delete[] vertList;
		polyList = 0;
		vertList = 0;
		numVert = 0;
		numPoly = 0;
	};

	static void Load3dsFile(const zSTRING& fileName) {
		long	p;
		zFILE* f = zfactory->CreateZFile(fileName);

		//
		if (f->Exists())
		{
			f->Open(FALSE);
			// Find file size.
			p = f->Size();

			// Go!
			ChunkReader(f, 0, p);
			f->Close();
		}
		else {
			//zERR_FATAL("D: zFile3d(::Load3dsFile): Can't open " + fileName);
		};

		//
		delete f;
	}


	HOOK Ivk_zCFile3DS_Load3DS AS(&zCFile3DS::Load3DS, &zCFile3DS::Load3DS_Union);

	void zCFile3DS::Load3DS_Union(zSTRING const& fileName, zCMesh* tMesh)
	{
		zERR_MESSAGE(3, zERR_BEGIN, "D: Loading 3DS-File: \"" + fileName + "\" ... ");

		// hohe FPU Precision sicherstellen
		const zDWORD fpuControlWord = zfpuControler->GetCurrentControlWord();
		zfpuControler->SetPrecision_53();

		// init
		targetMesh = tMesh;
		numVert = 0;
		numPoly = 0;
		actColor = &dummyCol;

		cmd << "Load3ds: " << fileName << endl;

		// doit
		Load3dsFile(fileName);

		// cleanup
		if (!zoptions->Parm("ZLOAD3DSOLDMETHOD"))
			zCMesh::S_InitVertexMergeCache(targetMesh);

		cmd << "FlushMeshBufferEnd: " << fileName << endl;

		FlushMeshBuffer();

		if (!zoptions->Parm("ZLOAD3DSOLDMETHOD"))
			zCMesh::S_DeleteVertexMergeCache();

		cmd << "CalcBBox3D: " << fileName << endl;

		//tMesh->CalcBBox3D(FALSE);
		tMesh->ArraysToLists();
		tMesh->UnshareFeatures();
		tMesh->bbox3D = bb
		cmd << "CalcVertexNormals: " << fileName << endl;

		//	if (zCMaterial::GetUsageDefault	()!=zCMaterial::zMAT_USAGE_LEVEL) 
		{
			// Wenn man die VertNormals nicht setzt, sieht das frische Mesh
			// voellig wirr aus (Beleuchtungs-technisch)
			tMesh->CalcVertexNormals(zCMesh::zMSH_VERTNORMAL_FACET, NULL);
			//		tMesh->CalcVertexNormals(zCMesh::zMSH_VERTNORMAL_SMOOTH); 
		};

		cmd << "PrelightMesh: " << fileName << endl;

		// FIXME: hier/so ?? 
		tMesh->PrelightMesh(NULL);

		//	tMesh->SetDefaultPlanarMapping(1.0F,1.0F);
		//	tMesh->FitPolyVertAlloc();
			//
		zPATH path(fileName);
		tMesh->meshName = (path.GetFilename() + "." + path.GetExt());


#ifdef COMPILING_SPACER
		// [EDENFELD] 1.09 Fix für den Spacer: falscher Speicherort des Level-3DS bei "Save ZEN"
		tMesh->pathToMesh = fileName;
#endif

		cmd << "End: " << fileName << endl;

		//
	//	Print ("Finished. Verts: %d Polys: %d Mats: %d Textures: %d.",tMesh->numVert, tMesh->numPoly,zCMaterial :: NumInList(), zCTexture :: NumInList()); 

		//
	//	zerr.Message ("D: ... Finished. Verts: "+zSTRING(tMesh->numVert) +
	//		" Polys: "+zSTRING(tMesh->numPoly)
	//		);
	/*	zerr.Message ("D: ... Finished. Verts: "+zSTRING(tMesh->numVert) +
			" Polys: "+zSTRING(tMesh->numPoly) +
			" Mat: "+zSTRING(zCMaterial::NumInList()) +
			" Tex: "+zSTRING(zCTexture::NumInList())
			);*/
		zERR_MESSAGE(3, zERR_END, "");

		//
		zfpuControler->SetControlWord(fpuControlWord);
	}
}