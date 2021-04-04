// Supported with union (c) 2020 Union team

// User API for zCArchiver
// Add your methods here

zCObject * __fastcall zCArchiver::ReadObjectAccount_Hook1(char const *, int, class zCObject *);
zCObject * __fastcall zCArchiver::ReadObjectAccount_Hook2(const char *file, int line, const char* chunkName, zCObject* objectUseThis);