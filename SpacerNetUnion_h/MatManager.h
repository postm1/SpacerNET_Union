// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	
	// Add your code here . . .
	const zSTRING MATLIB_FILENAME = "matlib.ini";
	const zSTRING MATLIB_FILENAME_BACKUP = "matlib_backup.ini";
	const CString FILTER_NAME_TRASH = "[TRASH]";
	const zSTRING MATLIBFILE_EXT = ".pml";


	class spcCMatFilter
	{

	public:

		CString name;
		int id;

		spcCMatFilter()
		{
			name = "";
			id = -1;
		}
	};

	class MatFilterRenderEntry
	{
	public:
		//zCArray<DWORD> pixels;
		DWORD pixels[128 * 128];
		zSTRING name;
		int x, y;
		int bit;
		int hasAlpha;

		bool bResizeSmallTextures;
		bool bUseAlphaChannels;
		bool bUseCenterAligment;


		MatFilterRenderEntry::MatFilterRenderEntry()
		{
			bit = 0;
			hasAlpha = 0;

			this->bResizeSmallTextures = false;
			this->bUseAlphaChannels = true;
			this->bUseCenterAligment = true;
		}
	};

	class MatFilter
	{
	public:
		zCArray<spcCMatFilter*> matFilterList;
		bool init;
		bool filterMatBlocked;
		bool removeTextureCacheNext;

		bool bResizeSmallTextures;
		bool bUseAlphaChannels;
		bool bUseCenterAligment;

		void FillInterfaceData();
		int FindMatFilterIndexByName(CString filterName);
		bool LoadPML_File(CString fname);
		void OnSelectInMatFilter(zCMaterial* mat);
		void FillMatListByFilterName(CString filter);
		void AddNewFilter(CString name);
		void SaveFilterList();
		void LoadFilterList();
		void SaveAllMatFilter();
		void SaveCurrentFilter(int index);
		void OnRenameFilter(int index, CString name);
		void Init();
		void ToggleWindow(bool value);
		void Reset();

		void Render(zCMaterial* mat);
		void Loop();

		void CreateNewMat(CString name);
		void SearchMaterialByName(CString name);
	};

	class MatManager
	{
	public:
		zCList<zCSelPoly> pList;
		zCMaterial* matSelectedInTree;
		Common::Map<CString, zCMaterial*> pMaterialsMap;

	public:
		void Init();
		void Loop();
		void CleanSelection();
		void RenderSelection();
		void OnPick(float x, float y);
		zCSelPoly* GetCurrentSelection();
		bool IsMaterialSelected();

		void CreateMatTree();
		void CreateNewSelection(zCPolygon* pl);
		void SelectMaterial(zCMaterial* mat);
		void OnSelectMaterial(zCMaterial* mat);
		void OnSelectInMatFilter(zCMaterial* mat);
		void ExtractTexture(zCMaterial* mat);
	};

}