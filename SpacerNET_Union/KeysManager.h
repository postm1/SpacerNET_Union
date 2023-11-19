// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	enum KeyMod
	{
		LControl = 2 << 0,
		Alt = 2 << 1,
		LShift = 2 << 2,
		RShift = 2 << 3,
		RAlt = 2 << 4,
		RCtrl = 2 << 5
	};

	class KeyEntry
	{
	public:
		int keyCode;
		int modifier;
	};

	class KeysManager
	{
	public:


		bool KeyPressed(CString keyType, bool clearKey = false, bool ignoreModifiers = false);

		void InitValues();

		void UpdateKey(CString keyType, int code, int mod);

		CString GetIniValue(CString key);

		void ResetDefault();
	};
}