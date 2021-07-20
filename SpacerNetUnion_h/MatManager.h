// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class MatManager
	{
	public:
		zCList<zCSelPoly>* selPolyList;
		zCPolygon*	pickedPoly;
		zCMaterial*	SelectMaterial;
		zCVertex* selectedVertext;

		
		zCMaterial* copyMat;

	public:
		void Init();
		void CleanSelectMaterial();
		zCSelPoly* PolyIsSelected(zCPolygon * xPoly);
		bool PolySelect(bool select, zCSelPoly* selPoly);
		void Loop();
		void OnPick(float x, float y);
		void DrawAxes();
		void ShowInfo();
		void CopyTextureName();
		void DrawLine(zVEC3 wsPoint1, zVEC3 wsPoint2, zCOLOR col);
		void CalcPolyCenter(zPOINT2& center);
		void OnPolyApplyTexture();
		void PolyApplyMapping();
		void TextureScale(zVALUE x, zVALUE y);
		void ResetUV();
		void RestoreMat();
	};

}