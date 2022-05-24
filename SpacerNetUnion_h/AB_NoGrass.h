// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct GridEntry
	{
		zCArray<zCVob*> pList;
		zVEC3 p1;
		zVEC3 p2;
		zVEC3 p3;
		zVEC3 p4;

		zVEC3 center;

		// находитс€ ли воб в €чейке
		bool IsPosInGrid(zVEC3 pos)
		{

			return pos.n[0] <= p1.n[0]
				&& pos.n[0] >= p2.n[0]

				&& pos.n[2] >= p3.n[2]
				&& pos.n[2] <= p2.n[2];
		}
	};

	class AB_NoGrass
	{
		zVEC3 bariCenter;
		zCArray<zCVob*> vobListManage;
		zCArray<zCVob*> vobListGrass;
		zCList<zCVob> ignoreList;


		zCArray<GridEntry*> entries;

		zCArray<zSTRING> grassList;
		zCArray<zSTRING> bushList;

		bool hideBush;
		bool hideGrass;
		int hiddenAmount;
		int percent;

	public:
		void Init();
		void Loop();
		void CollectVobsFromLocation();

		void ManageObjects();
		void HideObjects();
		void ShowObjects();
		void Clear();
	};

}