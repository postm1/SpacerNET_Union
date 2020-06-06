// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	inline int zFloat2Int(const float f)
	{
		int i;
		__asm fld[f]
			__asm fistp[i]
			return i;
	}


	void zCCamera::SetVob(zCVob* vob)
	{
		connectedVob = vob;
	}

	zCVob* zCCamera::GetVob()
	{
		return connectedVob;
	}

	zVEC3 zCCamera::Transform(const zVEC3& point)
	{
		return camMatrix * point;
	}

	inline void zCCamera::BackProject(const int xscr, const int yscr, zVEC3& p) const {
		// coord-trafo: screen->3d
		// Assumption : p.n[VZ] must have been set before call !
		p.n[VX] = zREAL(xscr - zFloat2Int(vpData.xcenter)) * zREAL(viewDistanceXInv) * p.n[VZ];
		p.n[VY] = zREAL(zFloat2Int(vpData.ycenter) - yscr) * zREAL(viewDistanceYInv) * p.n[VZ];
	};


	void zCInput::ClearKey(int key)
	{
		static int KEY_EVENT_TABLE_SIZE = (JOY_BUTTON_32 + 1);
		static bool* keyevent = (bool*)0x008D1678;
		static bool* keytoggle = (bool*)0x008D18B8;
		static bool* keyRepeatEnabled = (bool*)0x008D1B10;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x008D1D50;

		if (key < 0 || key > KEY_EVENT_TABLE_SIZE)
			return;

		keyevent[key] = 0;
		keytoggle[key] = 0;
		keyRepeatEnabled[key] = 1;

		for (int i = 0; i < keybuffer.GetNum(); i++) {
			int keyB = keybuffer.GetSafe(i);
			if (keyB == key) {
				keybuffer.DeleteList();
				return;
			}
		}
	}


}