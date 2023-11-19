// Supported with union (c) 2020 Union team

// User API for zCObject
// Add your methods here

static zCClassDef*		GetClassDef(const zSTRING& className) { return zCClassDef::GetClassDef(className); };
static zCClassDef*		GetBaseClassDef(const zSTRING& className);


zCClassDef*				GetClassDef()	const { return _GetClassDef(); };