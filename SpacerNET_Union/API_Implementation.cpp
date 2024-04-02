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

	void zMAT4::GetTranslation(zVEC3& t) const
	{
		t.n[VX] = v[0][3];
		t.n[VY] = v[1][3];
		t.n[VZ] = v[2][3];
	};


#if ENGINE == Engine_G1
#define ISUPPER3X3ORTHONORMAL 0x00505E40
#elif ENGINE == Engine_G1A
#define ISUPPER3X3ORTHONORMAL 0x00519D90
#elif ENGINE == Engine_G2
#define ISUPPER3X3ORTHONORMAL 0x00512C80
#elif ENGINE == Engine_G2A
#define ISUPPER3X3ORTHONORMAL 0x00515A50
#endif


	int zMAT4::IsUpper3x3Orthonormal() const
	{
		XCALL(ISUPPER3X3ORTHONORMAL);
	}



	inline void zCCamera::BackProject(const int xscr, const int yscr, zVEC3& p) const {
		p.n[VX] = zREAL(xscr - zFloat2Int(vpData.xcenter)) * zREAL(viewDistanceXInv) * p.n[VZ];
		p.n[VY] = zREAL(zFloat2Int(vpData.ycenter) - yscr) * zREAL(viewDistanceYInv) * p.n[VZ];
	};

	struct zTMouseState {
		int						xpos;				
		int						ypos;			
		int						zpos;				
		zBOOL					buttonPressedLeft;
		zBOOL					buttonPressedMid;	
		zBOOL					buttonPressedRight;
	};

	void zCInput::ClearLeftMouse()
	{


#if ENGINE == Engine_G1
		//FIXME_G1 Addr?
		int& leftMouseVal = *(int*)0x86CCB8;
		leftMouseVal = 0;
		
#else
		static zTMouseState& mouseState = *(zTMouseState*)0x8D165C;

		mouseState.buttonPressedLeft = 0;
#endif

		
		this->ClearKey(MOUSE_BUTTONLEFT);
	}

	void zCInput::ClearKey(int key)
	{
		static int KEY_EVENT_TABLE_SIZE = (JOY_BUTTON_32 + 1);

#if ENGINE == Engine_G1
		static bool* keyevent = (bool*)0x0086CCC8;
		static bool* keytoggle = (bool*)0x0086CED0;
		static bool* keyRepeatEnabled = (bool*)0x0086D0D8;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x0086D2DC;
#else
		static bool* keyevent = (bool*)0x008D1678;
		static bool* keytoggle = (bool*)0x008D18B8;
		static bool* keyRepeatEnabled = (bool*)0x008D1B10;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x008D1D50;
#endif

		

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

	bool zCVob::IsPFX() 
	{
		bool result = false;

		if (this->GetVisual() && (this->GetVisual()->GetVisualName().Contains(".pfx") || this->GetVisual()->GetVisualName().Contains(".PFX")))
		{
			result = true;
		}

		return result;
	}


}