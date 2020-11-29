// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {


	class zCSelPoly
	{
	public:
		zCSelPoly::zCSelPoly(zCPolygon* _poly)
		{
			zERR_ASSERT(_poly);
			poly = _poly;
			mat = 0;
			if (poly)
			{
				mat = poly->material;
				if (mat) mat->AddRef();
			}
		};

		zCSelPoly::~zCSelPoly()
		{
			poly = 0;
			zRELEASE(mat);
		};

		void			SetMaterial(zCMaterial* _mat);
		zCMaterial*		GetMaterial() { return mat; };
		zCPolygon*		GetPolygon() { return poly; };

	protected:
		zCSelPoly() {};
		zCPolygon*  poly;
		zCMaterial* mat;
	};

	enum TLibFLags
	{
		MATLIB_VISIBLE, MATLIB_INVISIBLE
	};


	// Add your code here . . .
	typedef zVEC2	zPOINT2;
	typedef zVEC3	zPOINT3;
	typedef zREAL	zVALUE;

	struct TVertex3ds {
		zPOINT3		position;
		zPOINT2		texCoord;
		zWORD		lastObjHasBeenWrittenTo;			// save: vert fuer dieses Objekt bereits geschrieben worden ?
		zWORD		remappedIndex;						// save: wenn Mesh in mehrere Objekte gesplittet werden muss, dann muessen auch die VertIndices remapped werden
		int			mappedIndex;
		int			leafIndex;
		TVertex3ds() {
			texCoord = zVEC2(0);
			lastObjHasBeenWrittenTo = 0;
			remappedIndex = 0;
			mappedIndex = 0;
			leafIndex = 0;
		};
	};

	struct TPoly3ds {
		int			vertIndex[3];
		zCMaterial	*material;
		zBOOL		wrapu;
		zBOOL		wrapv;
		zWORD		flags;
		void CheckForWrap();
		TPoly3ds() {
			vertIndex[0] = vertIndex[1] = vertIndex[2] = 0;
			material = 0;
		};
	};

	class zCMesh3DS {
	public:
		zSTRING					meshName;
		zCArray<int>			vertIndList3DS;
		zCArray<int>			polyIndList3DS;

		struct zTMatEntry {
			zCMaterial			*material;
			zCArray<int>		polyIndList;
		};
		zCArray<zTMatEntry*>	matList;

		~zCMesh3DS() {
			Clear();
		};
		void Clear() {
			meshName.Clear();;
			vertIndList3DS.EmptyList();
			polyIndList3DS.EmptyList();
			for (int i = 0; i<matList.GetNum(); i++)
				delete matList[i];
			matList.EmptyList();
		};
		zBOOL IsEmpty() const { return vertIndList3DS.IsEmpty() || polyIndList3DS.IsEmpty(); };
	};


	class zCBinTree3DS;

	static zCBinTree3DS  *binTree3DS = 0;

	struct zTBinTreeNode {
		zCArray<int>	vertIndList3DS;
		zCArray<int>	polyIndList3DS;
	};

	class zCBinTree3DS {
	public:

		zCBinTree3DS();
		~zCBinTree3DS();
		void					CreateNodeList(int numVert, int numPoly);
		int						GetVert3DSIndex(const zVEC3& pos, const zVEC2& texUV);
		void					DropMesh(zCMesh *mesh);
		int						CalcNum3DSObject();
		void					BuildMesh3DS(int objectNr, int numObject, zCMesh3DS *mesh3DS);
		void					GetMatListComplete(zCArray<zCMaterial*> &matList);

		int						numVert3DS;
		int						numPoly3DS;
		zCArraySort<TVertex3ds> vertList3DS;
		//TVertex3ds				*vertList3DS;
		TPoly3ds				*polyList3DS;
		zBOOL					dumpTexCoords;

	private:
		int						act3DSObject;
		zCArray<zTBinTreeNode>	nodeList;		// FIXME: eigentlich brauchen nur alle 'leafs' allokiert werden, nicht alle 'nodes'..
		zTBBox3D				bbox3D;

		zBOOL					IsLeaf(int nodeIndex) const { return (nodeIndex * 2) >= nodeList.GetNum(); };
		int						GetLeafIndex(const zVEC3& pos);
		void					CalcNumPolyVertSubtree(int nodeIndex, int& numVert, int& numPoly);
		void					GetPolyListFor3DSObj(int objectNr, zCArray<int> &polyIndexList, zCArray<int> &vertIndList);
	};
}