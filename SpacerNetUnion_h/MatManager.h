// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	class MatManager
	{
	public:
		zCList<zCSelPoly> pList;
		zCMaterial* matSelectedInTree;
		Common::Map<CString, zCMaterial*> pMaterialsMap;

	public:
		void Init();
		void Loop();
		void CleanSelection();
		void RenderSelection();
		void OnPick(float x, float y);
		zCSelPoly* GetCurrentSelection();
		bool IsMaterialSelected();

		void CreateMatTree();
		void CreateNewSelection(zCPolygon* pl);
		void SelectMaterial(zCMaterial* mat);
	};

}