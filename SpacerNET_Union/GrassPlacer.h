// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class GrassPlacer
	{
		zCArray<zVEC3> pointsVec;
	public:
		void GeneratePointsCircle(zVEC3 startPos, int amount, int radius);

		void DoPlace();
	};
}