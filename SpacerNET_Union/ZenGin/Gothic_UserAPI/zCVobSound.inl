// Supported with union (c) 2020 Union team

// User API for zCVobSound
// Add your methods here

void Archive_Hook(zCArchiver &);
void Unarchive_Hook(zCArchiver &);
void ProcessZoneList_Union(zCArraySort<zCZone*> const& zoneList, zCArraySort<zCZone*> const& zoneDeactivateList, zCWorld* homeWorld);