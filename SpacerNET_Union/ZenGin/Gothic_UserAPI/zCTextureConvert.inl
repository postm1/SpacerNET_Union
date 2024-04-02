// Supported with union (c) 2020 Union team

// User API for zCTextureConvert
// Add your methods here


#if ENGINE == Engine_G2A
int ConvertTextureFormat(zCTextureInfo const&) zCall(0x006598E0);
void SetDetailTextureMode(int) zCall(0x0065B7A0);
#else
int ConvertTextureFormat(zCTextureInfo const&) zCall(0x00720DC0);
void SetDetailTextureMode(int) zCall(0x00722BD0);
#endif


zBOOL zCTextureConvert::SaveToFileFormat_Union(const zSTRING& fileName);