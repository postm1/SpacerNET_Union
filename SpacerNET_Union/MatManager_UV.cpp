// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void MatManager::UV_GatherPolygons()
	{

	}
	void MatManager::UV_FindErrors()
	{
		print.PrintGreen("TEST");
		cmd << uvSettings.minArea << "/" << uvSettings.maxArea << "/" << uvSettings.distAngle << "/" << uvSettings.ignoreNoColl << endl;
	}
}