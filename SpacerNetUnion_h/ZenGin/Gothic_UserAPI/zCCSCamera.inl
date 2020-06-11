// Supported with union (c) 2020 Union team

// User API for zCCSCamera
// Add your methods here


void Unarchive_Hook(zCArchiver &);
zCVob* GetPlayerVob_Hook(zCVob* vob);

int				SearchCamKey(zCCamTrj_KeyFrame*key) { return posKeys.Search(key); };
int				SearchTargetKey(zCCamTrj_KeyFrame*key) { return targetKeys.Search(key); };