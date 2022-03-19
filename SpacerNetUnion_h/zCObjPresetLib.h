// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class zCObjPreset : public zCObject
	{
		zCLASS_DECLARATION(zCObjPreset)
	public:
		zCObject* obj;
		zSTRING presetName;

		zCObjPreset() : obj(0), presetName("") {};
		zCObjPreset(zCObject* _obj, zSTRING& name) : obj(_obj), presetName(name) {};
		virtual ~zCObjPreset();

		void	Archive(zCArchiver &arc);
		void	Unarchive(zCArchiver &arc);
	};

	class zCPresetView;

	class zCObjPresetLib : public zCArray<zCObjPreset*>
	{
	private:
		zCArray<zCPresetView*> viewPointer;
		zBOOL isDirty;

	public:
		zCObjPresetLib() : zCArray<zCObjPreset*>(), isDirty(FALSE), viewPointer() {};
		virtual ~zCObjPresetLib();
		void Dispose();

		// load/save
		int Load();
		int Load(zSTRING& filename);
		int Save();

		void SetDirty(zBOOL dirt = TRUE) { isDirty = dirt; };
		zBOOL IsDirty() { return isDirty; };

		// find first perset with that name
		zCObject*	GetObjectByName(zSTRING& pName);
		int			GetIndexByName(const zSTRING& name);

		// Create new view from class (do not delete!)
		zCPresetView* GetNewView(zSTRING& className);

		zBOOL AddPreset(zCObject* obj, zSTRING& name);
		zBOOL RemovePreset(int lib_index);
	};

}