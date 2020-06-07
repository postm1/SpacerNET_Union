// Supported with union (c) 2020 Union team

// User API for oCWorld
// Add your methods here

typedef zCListSort<zCVob>		oTVobList;
typedef zCListSort<oCNpc>		oTVobListNpcs;
typedef zCListSort<oCItem>		oTVobListItems;


oTVobList*				GetVobList() { return voblist; };
oTVobListNpcs*			GetVobListNpcs() { return voblist_npcs; };
oTVobListItems*			GetVobListItems() { return voblist_items; };

int SaveWorld_Hook(zSTRING const & fileName, enum zCWorld::zTWorldSaveMode saveMode, int writeBinary, int _saveLevelMesh);