// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class MatManager
	{
	public:
		zCList<zCSelPoly> pList;

	public:
		void Init();
		void Loop();
		void CleanSelection();
		void RenderSelection();
		void OnPick(float x, float y);
		zCSelPoly* GetCurrentSelection();
		bool IsMaterialSelected();
	};

}