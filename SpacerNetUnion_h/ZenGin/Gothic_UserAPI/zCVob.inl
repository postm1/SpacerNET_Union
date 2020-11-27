// Supported with union (c) 2020 Union team

// User API for zCVob
// Add your methods here


void				SetShowVisual(zBOOL show) { showVisual = show; };
zBOOL				GetShowVisual() const { return showVisual; };
const zTBBox3D&		GetBBox3DWorld() const { return bbox3D; };
zTVisualCamAlign	GetVisualCamAlign() const { return visualCamAlign; };
void				SetVisualCamAlign(const zTVisualCamAlign a) { visualCamAlign = a; };

void				SetVisualAlphaEnabled(const zBOOL b) { visualAlphaEnabled = b; };
zBOOL				GetVisualAlphaEnabled() const { return visualAlphaEnabled; };
void				SetVisualAlpha(const zREAL a) { visualAlpha = a; };
zREAL				GetVisualAlpha() const { return visualAlpha; };
static zBOOL		GetIgnoreVisuals() { return s_ignoreVisuals; };
static void			SetIgnoreVisuals(zBOOL b) { s_ignoreVisuals = b; };

zBOOL				GetPhysicsEnabled() const { return physicsEnabled; };

zBOOL				GetCollDetDyn() const { return collDetectionDynamic; };
zBOOL				GetCollDetStat() const { return collDetectionStatic; };
const zSTRING&		GetVobName() const { return GetObjectName(); };
zTVobSleepingMode	GetSleepingMode() const { return zTVobSleepingMode(sleepingMode); };

zBOOL				GetIgnoredByTraceRay() const { return ignoredByTraceRay; };


uint GetParent()
{
	if ((globalVobTreeNode) && (globalVobTreeNode->GetParent()) && globalVobTreeNode->GetParent()->GetData())
	{
		return (uint)globalVobTreeNode->GetParent()->GetData();
	}
	return NULL;
}

zCVob* GetParentVob()
{
	if ((globalVobTreeNode) && (globalVobTreeNode->GetParent()) && globalVobTreeNode->GetParent()->GetData())
	{
		return globalVobTreeNode->GetParent()->GetData();
	}
	return NULL;
}

zCEventManager*		GetEventManager(const zBOOL dontCreateIfNotPresent = FALSE) { return GetEM(dontCreateIfNotPresent); };


void SetVobPresetName_Hook(class zSTRING const &presetName);
void Archive_Hook(zCArchiver&);


