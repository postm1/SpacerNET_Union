// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	enum CSettingType
	{
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING
	};

	struct CSetting
	{
	public:
		zSTRING default_value;

		CSettingType type;
		zSTRING section;
		zSTRING name;

		int value_int;
		float value_float;
		zSTRING value_string;

		CSetting(CSettingType type, zSTRING section, zSTRING name, zSTRING default_value);
	};

	class CSettings
	{
	private:
		zCList<CSetting> list;

	public:

		void Init();
		void Load();
		void Save();
		void Apply();

		~CSettings();


		float GetFloatVal(zSTRING key);
		int GetIntVal(zSTRING key);
		zSTRING GetVal(zSTRING key);

		void SetFloatVal(zSTRING key, float val);
		void SetIntVal(zSTRING key, int val);
		void SetStringVal(zSTRING key, zSTRING val);

	};
}