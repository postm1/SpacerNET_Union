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

bool zCVob::HasChildrenRec(zCVob* pVobFutureParent, int maxDepth = 100)
{
    // Защита от nullptr и проверка на самого себя
    if (!pVobFutureParent || pVobFutureParent == this)
        return true; // Нельзя перемещать в себя или в nullptr

    if (maxDepth <= 0)
        return false; // Защита от бесконечной рекурсии (на случай циклов)

    auto node = this->globalVobTreeNode;
    if (!node)
        return false; // Нет детей - pVobFutureParent не может быть ребенком

    // Обходим всех детей текущего воба
    zCTree<zCVob>* childNode = node->GetFirstChild();
    while (childNode)
    {
        zCVob* childVob = childNode->GetData();

        // Если нашли pVobFutureParent среди детей
        if (childVob == pVobFutureParent)
            return true;

        // Рекурсивно проверяем поддерево этого ребенка
        if (childVob && childVob->HasChildrenRec(pVobFutureParent, maxDepth - 1))
            return true;

        // Переходим к следующему ребенку на том же уровне
        childNode = childNode->GetNextChild();
    }

    return false; // pVobFutureParent не найден в поддереве
}

zCEventManager*		GetEventManager(const zBOOL dontCreateIfNotPresent = FALSE) { return GetEM(dontCreateIfNotPresent); };


void SetVobPresetName_Hook(class zSTRING const &presetName);
void Archive_Hook(zCArchiver&);
void zCVob::Unarchive_Union(zCArchiver& arc);


struct zTBBox3D zCVob::GetBBox3DLocal_Union(void);


bool IsPFX();

bool HasChildren()
{
	auto node = this->globalVobTreeNode;

	if (node)
	{
		return node->GetFirstChild();
	}

	return NULL;
}

BOOL __fastcall Render_Union(zTRenderContext &renderContext);

void zCVob::SetVisual_Union(const zSTRING& visualFileName);