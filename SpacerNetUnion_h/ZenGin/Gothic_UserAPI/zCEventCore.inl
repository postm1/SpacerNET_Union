// Supported with union (c) 2020 Union team

// User API for zCEventCore
// Add your methods here




zCEventCore(const zTEventCoreSubType sub) { Clear(); subType = (uint16)(sub); };
zCEventCore(const zTEventCoreSubType sub, zCVob* inOtherVob) { Clear(); subType = (uint16)(sub); otherVob = inOtherVob; };
zCEventCore(const zTEventCoreSubType sub, zCVob* inOtherVob, zCVob* inInstigator) { Clear(); subType = (uint16)(sub); otherVob = inOtherVob; vobInstigator = inInstigator; };
zCEventCore(const zTEventCoreSubType sub,
	zCVob* inOtherVob, zCVob* inInflictorVob, const zREAL dam, const int damType = 0, const zVEC3& hitLoc = zVEC3(0))
{
	Clear(); subType = (uint16)(sub); otherVob = inOtherVob; inflictorVob = inInflictorVob; damage = dam; damageType = damType; hitLocation = hitLoc;
};