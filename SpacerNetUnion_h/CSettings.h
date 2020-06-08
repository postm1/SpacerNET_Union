// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	

	enum CSettingType
	{
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_STRING
	};

	struct CSetting
	{
	public:
		CString default_value;

		CSettingType type;
		CString section;
		CString name;

		int value_int;
		float value_float;
		CString value_string;

		CSetting(CSettingType type, CString section, CString name, CString default_value);
	};

	class CSettings
	{
	private:
		Common::Map<CString, CSetting*> list;

	public:

		void Init();
		void Load();
		void Save();
		void Apply();

		~CSettings();


		float GetFloatVal(CString key);
		int GetIntVal(CString key);
		CString GetVal(CString key);

		void SetFloatVal(CString key, float val);
		void SetIntVal(CString key, int val);
		void SetStringVal(CString key, CString val);

	};
}