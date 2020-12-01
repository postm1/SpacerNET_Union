// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class MatManager
	{
		zCList<zCSelPoly>* selPolyList;
		zCPolygon*	pickedPoly;
		zCMaterial*	SelectMaterial;
		zCVertex* selectedVertext;


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
	};

}