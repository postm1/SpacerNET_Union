// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	inline float zAbs(const float a)
	{
		return fabs(a);
	}

	static int Compare_Verts3DS(const void *arg1, const void *arg2)
	{
		const zVALUE	VERT_EPS = zREAL(0.1F);
		const zVALUE	UV_EPS = zREAL(0.001F);

		const zVEC3		p1 = (*((TVertex3ds*)arg1)).position;
		const zVEC3		p2 = (*((TVertex3ds*)arg2)).position;
		const zVEC2		uv1 = (*((TVertex3ds*)arg1)).texCoord;
		const zVEC2		uv2 = (*((TVertex3ds*)arg2)).texCoord;
		const int		leafIndex1 = (*((TVertex3ds*)arg1)).leafIndex;
		const int		leafIndex2 = (*((TVertex3ds*)arg2)).leafIndex;

		const zREAL		p1x = p1.n[VX];
		const zREAL		p1y = p1.n[VY];
		const zREAL		p1z = p1.n[VZ];
		const zREAL		p2x = p2.n[VX];
		const zREAL		p2y = p2.n[VY];
		const zREAL		p2z = p2.n[VZ];
		const zREAL		uv1x = uv1.n[VX];
		const zREAL		uv1y = uv1.n[VY];
		const zREAL		uv2x = uv2.n[VX];
		const zREAL		uv2y = uv2.n[VY];


		// erster test auf gleichheit
		if (leafIndex1 == leafIndex2)
			if ((zAbs(p1x - p2x)	< VERT_EPS) &&
				(zAbs(p1y - p2y)	< VERT_EPS) &&
				(zAbs(p1z - p2z)	< VERT_EPS) &&
				(zAbs(uv1x - uv2x)	< UV_EPS) &&
				(zAbs(uv1y - uv2y)	< UV_EPS))
			{
				return 0;
			}


		if ((p1x - p2x) < 0) return -1;
		else if ((p1x - p2x) > 0) return +1;
		else
		{
			// die x koords sind gleich, dann die y coords vergleichen
			if ((p1y - p2y) < 0) return -1;
			else if ((p1y - p2y) > 0) return +1;
			else
			{
				// die x und die y coords sind gleich, z vergleichen
				if ((p1z - p2z) < 0) return -1;
				else if ((p1z - p2z) > 0) return +1;
				else
				{
					// alle koords gleich, uv mappings können aber unterschiedlich sein
					if ((uv1x - uv2x)<0) return -1;
					else if ((uv1x - uv2x)>0) return +1;
					else
					{
						// alle koords gleich, uv mappings können aber unterschiedlich sein
						if ((uv1y - uv2y)<0) return -1;
						else if ((uv1y - uv2y)>0) return +1;
					}
				}
			}
		}

		//zERR_WARNING("C: Compare_Verts3DS: Bug! Should be never here!");
		// hier darf man niemals hinkommen
		return 0;
	}

	zCBinTree3DS::zCBinTree3DS()
	{
		bbox3D.InitZero();
		act3DSObject = 0;
		numVert3DS = 0;
		numPoly3DS = 0;
		vertList3DS.SetCompare(Compare_Verts3DS);
		polyList3DS = 0;
		dumpTexCoords = FALSE;
	};

	zCBinTree3DS::~zCBinTree3DS()
	{
		numVert3DS = 0;
		numPoly3DS = 0;
		vertList3DS.DeleteList();
		//delete[]	vertList3DS;
		delete[]	polyList3DS;
	};

	void zCBinTree3DS::GetMatListComplete(zCArray<zCMaterial*> &matList)
	{
		// linear Search, zu langsam ?
		for (int i = 0; i<numPoly3DS; i++)
		{
			if (polyList3DS[i].material)
				if (!matList.IsInList(polyList3DS[i].material))
					matList.Insert(polyList3DS[i].material);
		};
	};

	void zCBinTree3DS::CalcNumPolyVertSubtree(int nodeIndex, int& numVert, int& numPoly)
	{
		if (IsLeaf(nodeIndex)) {
			numPoly += nodeList[nodeIndex].polyIndList3DS.GetNum();
			numVert += nodeList[nodeIndex].vertIndList3DS.GetNum();
		}
		else {
			CalcNumPolyVertSubtree(nodeIndex * 2, numVert, numPoly);
			CalcNumPolyVertSubtree(nodeIndex * 2 + 1, numVert, numPoly);
		};
	};

	int zCBinTree3DS::CalcNum3DSObject()
	{
		// Breitensuche
		int index = 1;
		do {
			int lastIndex = (index * 2 - 1);
			zBOOL	ok = TRUE;
			for (int i = index; i <= lastIndex; i++)
			{
				int numVert = 0;
				int numPoly = 0;
				CalcNumPolyVertSubtree(i, numVert, numPoly);
				ok = ok && ((numVert <= 65535) && (numPoly <= 65535));		// 3DS Limits
																			//			ok = ok && ((numVert<=50) && (numPoly<=65535));			// 3DS Limits
				if (!ok) break;
			};
			if (ok)	break;
			index *= 2;
		} while (!IsLeaf(index));
		return index;
	};

	void zCBinTree3DS::GetPolyListFor3DSObj(int nodeIndex, zCArray<int> &polyIndexList, zCArray<int> &vertIndList)
	{
		if (IsLeaf(nodeIndex))
		{
			for (int i = 0; i<nodeList[nodeIndex].polyIndList3DS.GetNum(); i++)
			{
				// Poly eintragen
				polyIndexList.Insert(nodeList[nodeIndex].polyIndList3DS[i]);
				// Vertices eintragen
				int polyInd = nodeList[nodeIndex].polyIndList3DS[i];
				for (int j = 0; j<3; j++)
				{
					int vertInd = polyList3DS[polyInd].vertIndex[j];
					if (vertList3DS[vertInd].lastObjHasBeenWrittenTo != this->act3DSObject)
					{
						vertIndList.Insert(vertInd);
						vertList3DS[vertInd].remappedIndex = vertIndList.GetNum() - 1;
						vertList3DS[vertInd].lastObjHasBeenWrittenTo = this->act3DSObject;
					};
					polyList3DS[polyInd].vertIndex[j] = vertList3DS[vertInd].remappedIndex;
				};
			};
		}
		else {
			GetPolyListFor3DSObj(nodeIndex * 2, polyIndexList, vertIndList);
			GetPolyListFor3DSObj(nodeIndex * 2 + 1, polyIndexList, vertIndList);
		};
	};

	void zCBinTree3DS::BuildMesh3DS(int objectNr, int numObject, zCMesh3DS *mesh3DS)
	{
		mesh3DS->Clear();
		mesh3DS->meshName = ToStr "zenGin" + zSTRING(objectNr);

		// Poly und Vert Listen
		int nodeIndex = numObject + objectNr;
		act3DSObject = objectNr + 1;
		GetPolyListFor3DSObj(nodeIndex, mesh3DS->polyIndList3DS, mesh3DS->vertIndList3DS);

		// BuildUsedMatList
		// erstmal alle verwendeten Materials sammeln
		for (int i = 0; i<mesh3DS->polyIndList3DS.GetNum(); i++)
		{
			zCMaterial *mat = polyList3DS[mesh3DS->polyIndList3DS[i]].material;
			if (mat) {
				for (int j = 0; j<mesh3DS->matList.GetNum(); j++)
				{
					if (mesh3DS->matList[j]->material == mat)
					{
						//					mesh3DS->matList[j]->polyIndList.Insert (mesh3DS->polyIndList3DS[i]);
						mesh3DS->matList[j]->polyIndList.Insert(i);
						goto foundMat;
					};
				};
				{
					zCMesh3DS::zTMatEntry *matEntry = zNEW(zCMesh3DS::zTMatEntry);
					matEntry->material = mat;
					//				matEntry->polyIndList.Insert	(mesh3DS->polyIndList3DS[i]);
					matEntry->polyIndList.Insert(i);							// bezieht sich auf Reihenfolge in 'Mesh3DS' und nicht BinTree3DS!!
					mesh3DS->matList.Insert(matEntry);
				};
			foundMat:;
			};
		};

	};


	inline int zGetHighestBit(int value) {
		int mask = 1 << (sizeof(int) * 8 - 1);
		int bit = 31;

		while (((value & mask) == 0) && (bit>0)) {
			bit--;
			mask = mask >> 1;
		}

		/* wenig effizient:
		// das höchstwertigste bit suchen
		int last,i;
		last = i = 0;
		do {
		if (value & 1) last = i;
		value = value >> 1;
		i++;
		} while (value!=0);

		if (bit != last)
		abort();*/

		return bit;
	};

	inline int zMakePowerOf2Lower(int value) {
		return 1 << zGetHighestBit(value);
	};

	inline int zMakePowerOf2Higher(int value) {
		// das höchstwertigste bit+1 suchen
		int i = zMakePowerOf2Lower(value);
		if (value>i) i = i << 1;

		return i;
	};


	void zCBinTree3DS::CreateNodeList(int numVert, int numPoly)
	{
		// 15 verts pro Leaf im Schnitt
		int	numLeafs = zMakePowerOf2Higher(int(float(numVert) / 15.0F));
		zClamp(numLeafs, 512, 8192);
		int	numNodes = numLeafs * 2;
		//	int depth			= 10;
		//	int numNodes		= int(pow (2, double(depth)));
		nodeList.DeleteList();				// call constructors
		nodeList.AllocAbs(numNodes);
		nodeList.MarkNumAllocUsed();
	};

	int zCBinTree3DS::GetLeafIndex(const zVEC3& pos)
	{
		int			axisTable[3] = { 0, 2, 1 };		// erst in der XZ Eben splitten, dann erst Y
		int			oldIndex = 1;
		int			index = 1;
		int			axis = 0;
		zTBBox3D	box = bbox3D;
		while (index<nodeList.GetNum())
		{
			int		realAxis = axisTable[axis];
			zREAL	boxMid = (box.maxs[realAxis] + box.mins[realAxis]) * 0.5F;
			oldIndex = index;
			if (pos[realAxis]<boxMid)
			{
				box.maxs[realAxis] = boxMid;
				index = 2 * index;
			}
			else {
				box.mins[realAxis] = boxMid;
				index = 2 * index + 1;
			};
			realAxis = (realAxis + 1) % 3;
		};
		return oldIndex;
	};


	int zCBinTree3DS::GetVert3DSIndex(const zVEC3& pos, const zVEC2& texUV)
	{
		const zREAL	VERT_EPS = zREAL(0.1F);
		const zREAL	UV_EPS = zREAL(0.001F);

		//
		int leafIndex = GetLeafIndex(pos);
		for (int i = 0; i<nodeList[leafIndex].vertIndList3DS.GetNum(); i++)
		{
			TVertex3ds	*vert3DS = &(vertList3DS[nodeList[leafIndex].vertIndList3DS[i]]);
			if ((zAbs(zREAL(pos.n[0] - vert3DS->position.n[0])) < VERT_EPS) &&
				(zAbs(zREAL(pos.n[1] - vert3DS->position.n[1])) < VERT_EPS) &&
				(zAbs(zREAL(pos.n[2] - vert3DS->position.n[2])) < VERT_EPS) &&
				(zAbs(zREAL(texUV[0] - vert3DS->texCoord.n[0])) < UV_EPS) &&
				(zAbs(zREAL(texUV[1] - vert3DS->texCoord.n[1])) < UV_EPS))
			{
				return nodeList[leafIndex].vertIndList3DS[i];
			};
		};
		// Vertex nicht gefunden, neuen erzeugen
		vertList3DS[numVert3DS].position = pos;
		vertList3DS[numVert3DS].texCoord = texUV;
		numVert3DS++;
		nodeList[leafIndex].vertIndList3DS.Insert(numVert3DS - 1);
		return numVert3DS - 1;
	};


	void zCBinTree3DS::DropMesh(zCMesh *mesh)
	{
		//
		int numMaxVert = 0;
		int numMaxPoly = 0;
		numPoly3DS = 0;
		numVert3DS = 0;
		dumpTexCoords = FALSE;

		if (!mesh->polyList)
		{
			std::cout << " polyList err" << std::endl;
		}

		for (int i = 0; i<mesh->numPoly; i++) {


			numMaxVert += mesh->Poly(i)->polyNumVert;
			numMaxPoly += mesh->Poly(i)->polyNumVert - 2;
		};
		//delete[] vertList3DS;

		vertList3DS.DeleteList();
		delete[] polyList3DS;
		vertList3DS.AllocAbs(numMaxVert);
		polyList3DS = zNEW(TPoly3ds)[numMaxPoly];

		bbox3D = mesh->GetBBox3D();

		CreateNodeList(mesh->numVert, mesh->numPoly);

		// Create:
		// vertList3DS, FACELIST
		// Achtung: wenn Verts mit verschiedenen Texture-Koordinaten vorkommen,
		// muessen diese Verts auch mehrfach rausgeschrieben werden !

		zBOOL meshHasNPolys = FALSE;

		//zERR_MESSAGE(3, zERR_BEGIN, "D: 3DS-EXP: Drop mesh \"" + mesh->GetMeshName() + "\" (" + zSTRING(mesh->numPoly) + " polys) ...");
		int vertInd[256];
		for (int i = 0; i<mesh->numPoly; i++) {

			if ((i & 1023) == 0) {} // zERR_MESSAGE(4, 0, "D: 3DS-EXP: ... still working (" + zSTRING(i) + " polys)");

			zCPolygon *poly = mesh->Poly(i);

			// Verts in die VertList eintragen
			for (int j = 0; j<poly->polyNumVert; j++)
			{
				if (poly->polyNumVert>3) meshHasNPolys = TRUE;
				zCVertex		*vert = poly->GetVertexPtr(j);
				zCVertFeature	*feat = (poly->feature) ? poly->feature[j] : 0;
				// keine Texture ? UV auf 0 setzen!
				if (!poly->material->texture) {
					feat->texu = 0;
					feat->texv = 0;
				};
				vertInd[j] = GetVert3DSIndex(vert->position, zVEC2(feat->texu, feat->texv));
			};

			// FIXME: ACHTUNG, was bei degenerierten Polys ? (3 Verts kolinear)

			for (int j = 2; j<poly->polyNumVert; j++) {
				// Verts eintragen
				polyList3DS[numPoly3DS].vertIndex[0] = vertInd[0];
				polyList3DS[numPoly3DS].vertIndex[1] = vertInd[j - 1];
				polyList3DS[numPoly3DS].vertIndex[2] = vertInd[j];
				polyList3DS[numPoly3DS].material = poly->material;
				polyList3DS[numPoly3DS].wrapu = FALSE;				// FIXME: ????
				polyList3DS[numPoly3DS].wrapv = FALSE;
				WORD flags = 0x02;										// BC
				if (j == 2)					flags |= 0x01 | 0x02;		// AB BC
				if (j == poly->polyNumVert - 1) flags |= 0x02 | 0x04;		// BC CA
				polyList3DS[numPoly3DS].flags = flags;
				if (poly->material)
					if (poly->material->texture)
						dumpTexCoords = TRUE;
				// Poly in Baum einfuegen
				{
					int leafIndex = GetLeafIndex(poly->GetCenter());
					nodeList[leafIndex].polyIndList3DS.Insert(numPoly3DS);
				};
				numPoly3DS++;
			};
		};

		//zERR_MESSAGE(3, zERR_END, "");
	};
}