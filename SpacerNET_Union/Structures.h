// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	struct RGBA
	{
	public:
		int R, G, B, Alpha;
	};

	struct SearchVobEntry
	{
		CString fieldName;
		CString groupName;
		TPropEditType type;
		CString value;
		int fastCheckValueInt;
		float fastCheckValueFloat;
		std::regex patternSearch;
	};
}