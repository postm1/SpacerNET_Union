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

		void zCSelPoly::SetMaterial(zCMaterial* _mat)
		{
			zRELEASE(mat);
			mat = _mat;
			if (mat) mat->AddRef();
		};

		zCMaterial*		GetMaterial() { return mat; };
		zCPolygon*		GetPolygon() { return poly; };

	public:
		zCSelPoly() {};
		zCPolygon*  poly = NULL;
		zCMaterial* mat = NULL;
		zCOLOR color;
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
		zWORD		lastObjHasBeenWrittenTo;
		zWORD		remappedIndex;
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
}