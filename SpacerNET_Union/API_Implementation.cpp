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

	int zMAT4::IsUpper3x3Orthonormal() const
	{
#if ENGINE == Engine_G1
		XCALL(0x00505E40);
#elif ENGINE == Engine_G1A
		XCALL(0x00519D90);
#elif ENGINE == Engine_G2
		XCALL(0x00512C80);
#elif ENGINE == Engine_G2A
		XCALL(0x00515A50);
#endif
	}

#if ENGINE == Engine_G1
	void zCPolygon::SetMaterial(zCMaterial* mat)
	{
		if (mat == material)
			return;

		if (material)
			material->Release();

		if (mat)
			mat->AddRef();

		material = mat;
	}
#endif


#if ENGINE != Engine_G1A
	inline void zCCamera::BackProject(const int xscr, const int yscr, zVEC3& p) const {
		p.n[VX] = zREAL(xscr - zFloat2Int(vpData.xcenter)) * zREAL(viewDistanceXInv) * p.n[VZ];
		p.n[VY] = zREAL(zFloat2Int(vpData.ycenter) - yscr) * zREAL(viewDistanceYInv) * p.n[VZ];
	};
#endif

#if ENGINE <= Engine_G1A
	inline void zCCamera::ProjectClamp(zCVertexTransform* vertex, const float inv) const {
		vertex->vertScrX = vpData.xcenter + ((vertex->vertCamSpace.n[VX] * viewDistanceX) * inv);
		vertex->vertScrY = vpData.ycenter - ((vertex->vertCamSpace.n[VY] * viewDistanceY) * inv);
		zClamp(vertex->vertScrX, vpData.xminFloat, vpData.xmaxFloat);
		zClamp(vertex->vertScrY, vpData.yminFloat, vpData.ymaxFloat);
	};
#endif

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
		static zTMouseState& mouseState = *(zTMouseState*)0x0086CCAC;
#elif ENGINE == Engine_G1A
		static zTMouseState& mouseState = *(zTMouseState*)0x008B27A8;
#elif ENGINE == Engine_G2
		static zTMouseState& mouseState = *(zTMouseState*)0x008C3004;
#elif ENGINE == Engine_G2A
		static zTMouseState& mouseState = *(zTMouseState*)0x008D165C;
#endif
		mouseState.buttonPressedLeft = 0;

		
		this->ClearKey(MOUSE_BUTTONLEFT);
	}

	void zCInput::ClearKey(int key)
	{
		

#if ENGINE == Engine_G1
		static bool* keyevent = (bool*)0x0086CCC8;
		static bool* keytoggle = (bool*)0x0086CED0;
		static bool* keyRepeatEnabled = (bool*)0x0086D0D8;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x0086D2DC;

		constexpr int KEY_EVENT_TABLE_SIZE = (MAX_KEYS_AND_CODES + 1);
#elif ENGINE == Engine_G1A
		static bool* keyevent = (bool*)0x008B27C0;
		static bool* keytoggle = (bool*)0x008B29D4;
		static bool* keyRepeatEnabled = (bool*)0x008B2BE8;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x008B2E00;

		constexpr int KEY_EVENT_TABLE_SIZE = (MOUSE_XBUTTON5 + 1);
#elif ENGINE == Engine_G2
		static bool* keyevent = (bool*)0x008C3020;
		static bool* keytoggle = (bool*)0x008C3260;
		static bool* keyRepeatEnabled = (bool*)0x008C34B0;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x008C36F0;

		constexpr int KEY_EVENT_TABLE_SIZE = (JOY_BUTTON_32 + 1);
#elif ENGINE == Engine_G2A
		static bool* keyevent = (bool*)0x008D1678;
		static bool* keytoggle = (bool*)0x008D18B8;
		static bool* keyRepeatEnabled = (bool*)0x008D1B10;
		static zCArray<int>& keybuffer = *(zCArray<int>*)0x008D1D50;

		constexpr int KEY_EVENT_TABLE_SIZE = (JOY_BUTTON_32 + 1);
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

#if ENGINE < Engine_G2A
	zBOOL zTBBox3D::IsIntersecting(const zVEC3& rayOrigin, const zVEC3& rayDirection, zREAL& scaleMin, zREAL& scaleMax) const
	{
		zREAL minScale[3], maxScale[3];
		zREAL absRayDirection[3];

		absRayDirection[0] = abs(rayDirection[0]);
		absRayDirection[1] = abs(rayDirection[1]);
		absRayDirection[2] = abs(rayDirection[2]);

		for (int i = 0; i < 3; i++)
		{
			if (absRayDirection[i] < zREAL(.0001F))
			{
				if (mins[i] > rayOrigin[i] || rayOrigin[i] > maxs[i])
					return FALSE;

				minScale[i] = zREAL_MIN;
				maxScale[i] = zREAL_MAX;
			}
			else
			{
				minScale[i] = (mins[i] - rayOrigin[i]) / rayDirection[i];
				maxScale[i] = (maxs[i] - rayOrigin[i]) / rayDirection[i];

				if (maxScale[i] < minScale[i])
				{
					zREAL t = maxScale[i];
					maxScale[i] = minScale[i];
					minScale[i] = t;
				}
			}
		}

		scaleMin = max(minScale[0], max(minScale[1], minScale[2]));
		scaleMax = min(maxScale[0], min(maxScale[1], maxScale[2]));

		return scaleMin <= scaleMax;
	}
#endif
}