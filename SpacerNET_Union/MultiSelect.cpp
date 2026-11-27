// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	MultiSelect::MultiSelect() {
		BoundsView = Null;
		Locked = False;
		TraceRayEnabled = False;
        AwaitingViewportInput = false;
        IgnoreActivationClick = false;
	}



	void MultiSelect::DrawBounds() {
		if (!BoundsView) {
			BoundsView = new zCView();
			BoundsView->InsertBack("Select");
			BoundsView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_ADD);
			BoundsView->SetTransparency(50);
		}

		zVEC2 v1 = PickBegin;
		zVEC2 v2 = theApp.GetMousePosVirt();

		Bounds[VA] = zVEC2(min(v1[VX], v2[VX]), min(v1[VY], v2[VY]));
		Bounds[VB] = zVEC2(max(v1[VX], v2[VX]), max(v1[VY], v2[VY]));
		zVEC2 size = Bounds[VB] - Bounds[VA];

		BoundsView->SetPos(Bounds[VA][VX], Bounds[VA][VY]);
		BoundsView->SetSize(size[VX], size[VY]);
	}



	void MultiSelect::CancelPick() {
		if (Locked && BoundsView)
			screen->RemoveItem(BoundsView);
		Locked = False;
	}

    bool MultiSelect::ViewportInputReady() {
        if (!theApp.GameFocused() || theApp.camMan.cameraRun || InputBlocked()) {
            AwaitingViewportInput = true;
            return false;
        }
        bool left = zMouseKeyPressed(Left);
        if (AwaitingViewportInput) {
            AwaitingViewportInput = false;
            IgnoreActivationClick = left;
        }
        if (IgnoreActivationClick) {
            // Consume the whole focus-acquisition gesture, including mouse-up.
            // It must not become a zero-area rectangle that clears the group.
            if (!left) IgnoreActivationClick = false;
            return false;
        }
        return true;
    }

    void MultiSelect::DoPick() {
        if (!ViewportInputReady()) {
            CancelPick();
            DrawDx11();
            return;
        }

		if (keys.KeyPressed("VOB_DISABLE_SELECT", true))
		{
			ClearSelection();
			return;

		}

		if (keys.KeyPressed("VOB_DELETE", true))
		{
			RemoveSelected();
			return;

		}

		if (!Locked && zMouseKeyPressed(Left) && !zinput->GetMouseButtonPressedRight()) {
			PickBegin = theApp.GetMousePosVirt();
			Locked = True;
			DrawBounds();
			screen->InsertItem(BoundsView);

		}
		else if (Locked && !zMouseKeyPressed(Left)) {
			TraceRayEnabled = zKeyPressed(KEY_LSHIFT);
			PickEnd = theApp.GetMousePosVirt();
            DrawBounds(); // Include the final mouse position, even after a fast drag.

			Locked = False;
			screen->RemoveItem(BoundsView);

            // A click without a rectangle must not discard a group, even if the
            // engine reports the activation mouse state a frame after focus.
            if (Bounds[VB][VX] > Bounds[VA][VX] && Bounds[VB][VY] > Bounds[VA][VY])
                PickBounds();
				
		}

		if (Locked)
			DrawBounds();


		DrawDx11();
	}

	void MultiSelect::PickSingle(zCVob* vob) {
		if (zKeyPressed(KEY_LSHIFT))
			return AddToSelection(vob);

		if (zKeyPressed(KEY_LALT))
			return RemoveFromSelection(vob);

		ClearSelection();
		AddToSelection(vob);
	}

    bool MultiSelect::MatchesFilter(zCVob* vob) {
        if (!vob || !ogame || !ogame->GetWorld()) return false;
        bool editable = vob->GetHomeWorld() == ogame->GetWorld()
            && vob->globalVobTreeNode && !vob->dontWriteIntoArchive
            && vob != ogame->GetCameraVob() && !IsSpacerVob(vob)
            && !vob->CastTo<zCVobLevelCompo>();
        bool moverAncestor = false;
        for (auto parent = vob->GetParentVob(); parent; parent = parent->GetParentVob())
            if (parent->CastTo<zCMover>()) { moverAncestor = true; break; }
        SpacerSelection::Kind kind = { editable, vob->CastTo<oCItem>() != nullptr,
            vob->CastTo<zCMover>() != nullptr, moverAncestor,
            vob->CastTo<zCVobWaypoint>() != nullptr, vob->CastTo<zCVobSpot>() != nullptr };
        kind.mob = vob->CastTo<oCMOB>() != nullptr;
        kind.dynamicCollision = vob->GetCollDetDyn() != 0;
        kind.interactiveMob = vob->CastTo<oCMobInter>() != nullptr;
        kind.light = vob->CastTo<zCVobLight>() != nullptr;
        kind.sound = vob->CastTo<zCVobSound>() != nullptr;
        auto visual = vob->GetVisual();
        kind.pfx = visual && visual->CastTo<zCParticleFX>();
        kind.decal = visual && visual->CastTo<zCDecal>();
        kind.invisible = !visual || !vob->GetShowVisual();
        kind.trigger = vob->CastTo<zCTrigger>() != nullptr;
        kind.zone = vob->CastTo<zCZone>() != nullptr;
        kind.musicZone = vob->CastTo<oCZoneMusic>() != nullptr;
        return SpacerSelection::Matches(theApp.options.GetIntVal("multiSelectionFilter"), kind);
    }

    void MultiSelect::SetFilter(int filter) {
        if (filter < 0 || filter >= SpacerSelection::FilterCount) return;
        EndTranslation();
        CancelPick();
        theApp.options.SetIntVal("multiSelectionFilter", filter);
        GetCount(); // Also prune existing selection against the new filter.
    }

    void MultiSelect::SetTransformMode(int mode) {
        if (mode < 0 || mode > 1) return;
        EndTranslation();
        CancelPick();
        theApp.options.SetIntVal("multiSelectionTransformMode", mode);
    }

    int MultiSelect::GetCount() {
        if (!ogame || !ogame->GetWorld()) {
            theApp.SelectedVobs.Clear();
            return 0;
        }
        if (!theApp.SelectedVobs.GetNum()) return 0;
        zCArray<zCVob*> live;
        ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, live, 0);
        std::unordered_set<zCVob*> members;
        for (int i = 0; i < live.GetNumInList(); ++i) members.insert(live[i]);
        for (int i = (int)theApp.SelectedVobs.GetNum() - 1; i >= 0; --i) {
            auto vob = theApp.SelectedVobs[i];
            if (!members.count(vob)) theApp.SelectedVobs ^= vob;
            else if (!MatchesFilter(vob)) RemoveFromSelection(vob);
        }
        return (int)theApp.SelectedVobs.GetNum();
    }

    int MultiSelect::ChangeSelection(zCVob* vob, int action) {
        // List clicks are intentional editor input even while the viewport has no focus.
        if (!ogame || !ogame->GetWorld() || theApp.isExit || theApp.g_bIsPlayingGame
            || theApp.camMan.cameraRun || theApp.GetPickMode() != SWM_MULTISELECT
            || action < 0 || action > 2) return -1;
        zCArray<zCVob*> live;
        ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, live, 0);
        // Validate addresses from the interface before dereferencing them.
        if (!vob || !live.IsInList(vob) || (action != 2 && !MatchesFilter(vob))) return -1;
        EndTranslation();
        CancelPick();
        GetCount();
        if (action == 0) ClearSelection();
        if (action == 2) RemoveFromSelection(vob);
        else AddToSelection(vob);
        // The list callback can run between render frames; do not depend on the
        // loop having observed the tool window's focus before the next click.
        AwaitingViewportInput = true;
        return GetCount();
    }

    void MultiSelect::PickBounds() {
        EndTranslation();
        bool add = zKeyPressed(KEY_LSHIFT);
        bool sub = zKeyPressed(KEY_LALT);
        bool through = theApp.options.GetIntVal("multiSelectionThroughWalls") != 0
            || zKeyPressed(KEY_LCONTROL);
        if (!add && !sub) ClearSelection();
        if (!ogame || !ogame->GetWorld() || !ogame->GetCameraVob()) return;

        zCWorld* world = ogame->GetWorld();
        zCVob* camera = ogame->GetCameraVob();
        zCArray<zCVob*> candidates;
        world->SearchVobListByBaseClass(zCVob::classDef, candidates, 0);
        zVEC3 cameraPosition = camera->GetPositionWorld();
        for (int i = 0; i < candidates.GetNumInList(); ++i) {
            auto vob = candidates[i];
            // Subtraction also works for objects outside the current filter.
            if (!sub && !MatchesFilter(vob)) continue;
            if (sub && !theApp.SelectedVobs.HasEqual(vob)) continue;
            zVEC3 position = vob->GetPositionWorld();
            zVEC3 direction = position - cameraPosition;
            if (direction.Dot(camera->GetAtVectorWorld()) <= 0) continue;
            int x, y;
            GetProjection(x, y, position);
            if (x < Bounds[VA][VX] || x >= Bounds[VB][VX]
                || y < Bounds[VA][VY] || y >= Bounds[VB][VY]) continue;
            if (!through && world->TraceRayNearestHit(cameraPosition, direction, (zCVob*)nullptr,
                zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE)) continue;
            if (sub) RemoveFromSelection(vob);
            else AddToSelection(vob);
        }
    }

	void MultiSelect::AddToSelection(zCVob* vob) {
		if (!MatchesFilter(vob))
			return;

		vob->SetDrawBBox3D(True);
		theApp.SelectedVobs |= vob;
	}

	void MultiSelect::RemoveFromSelection(zCVob* vob) {
		if (!vob)
			return;

		vob->SetDrawBBox3D(False);
		theApp.SelectedVobs ^= vob;
	}

    void MultiSelect::RemoveSelected() {
        EndTranslation();
        GetCount();
        std::vector<zCVob*> selected;
        for (uint i = 0; i < theApp.SelectedVobs.GetNum(); ++i) {
            auto vob = theApp.SelectedVobs[i];
            vob->AddRef();
            selected.push_back(vob);
        }
        theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
        for (auto vob : selected) {
            if (ogame && vob->GetHomeWorld() == ogame->GetWorld())
                theApp.RemoveVob(vob);
            vob->Release();
        }
        theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);
        theApp.SelectedVobs.Clear();
    }

	void MultiSelect::ClearSelection() 
	{
		EndTranslation();
		CancelPick();

		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
			theApp.SelectedVobs[i]->SetDrawBBox3D(False);

		theApp.SelectedVobs.Clear();
	}

	// fixme add later
	void MultiSelect::SaveSelected()
	{
		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
		{
			
		}
	}

	int MultiSelect::ReparentSelected(zCVob* newParent)
	{
		if (theApp.GetPickMode() != SWM_MULTISELECT || theApp.SelectedVobs.GetNum() == 0
			|| !ogame || !ogame->GetWorld())
			return 0;

		CancelPick();
		zCWorld* world = ogame->GetWorld();
		zCArray<zCVob*> worldVobs;
		world->SearchVobListByBaseClass(zCVob::classDef, worldVobs, 0);

		// Check membership before dereferencing pointers received from the interface.
		if (!newParent || !worldVobs.IsInList(newParent) || newParent == ogame->GetCameraVob()
			|| newParent->dontWriteIntoArchive || !newParent->globalVobTreeNode)
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));
			return 0;
		}

		zCArray<zCVob*> vobsToReparent;
		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
		{
			zCVob* vob = theApp.SelectedVobs[i];
			if (!vob || !worldVobs.IsInList(vob))
			{
				print.PrintRed(GetLang("MULTISELECT_INVALID_SELECTION"));
				return 0;
			}

			if (vob == newParent || vob->GetParentVob() == newParent)
				continue;

			if (vob == ogame->GetCameraVob() || vob->dontWriteIntoArchive || !vob->globalVobTreeNode)
			{
				print.PrintRed(GetLang("CANT_APPLY_PARENT"));
				return 0;
			}

			// Validate the whole batch before changing the hierarchy.
			if (!CanChangeVobParent(vob, newParent))
				return 0;
			vobsToReparent.InsertEnd(vob);
		}

		if (vobsToReparent.GetNumInList() == 0)
			return 0;

		theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, FALSE);
		for (int i = 0; i < vobsToReparent.GetNumInList(); i++)
			HandleParentChange(vobsToReparent[i], newParent, false);
		theApp.exports.toggleUIElement(UI_ALL_VOBS_TREE_LIST, TRUE);

		// The single-object helper hides boxes while changing parent.
		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
			theApp.SelectedVobs[i]->SetDrawBBox3D(True);

		print.PrintRed(GetLang("PARENT_CHANGE_OK"));
		return vobsToReparent.GetNumInList();
	}

	void MultiSelect::DrawDx11()
	{
		if (!theApp.IsDx11Active())
		{
			return;
		}


		//print.PrintRed("draw: " + Z (int)theApp.SelectedVobs.GetNum());

		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
		{
			theApp.RenderDx11_Bbox(theApp.SelectedVobs[i]);
		}
	}

    struct GroupMoveEntry {
        zCVob* vob;
        zCVob* parent;
        zVEC3 position;
        zMAT4 transform;
        bool selected, collDyn, collStat;
        int childCount;
        std::vector<zVEC3> moverKeys;
        std::vector<zCQuat> moverRotations;
    };
    struct GroupMoveOperation {
        zCWorld* world = nullptr;
        std::vector<GroupMoveEntry> entries;
        zVEC3 delta = zVEC3(0, 0, 0);
        zVEC3 pivot = zVEC3(0, 0, 0);
        SpacerSelection::Rotation rotation;
        bool rotationMode = false;
        bool HasChanges() const { return delta.Length_Sqr() > 0 || !rotation.IsIdentity(); }
    };
    static GroupMoveOperation activeGroupMove;
    static std::vector<GroupMoveOperation> groupMoveUndo;
    static bool groupKeyboardMoving = false;

    static int CountGroupChildren(zCVob* vob) {
        int count = 0;
        if (vob->globalVobTreeNode)
            for (auto node = vob->globalVobTreeNode->GetFirstChild(); node; node = node->GetNextChild())
                if (node->GetData()) ++count;
        return count;
    }

    static bool ValidateGroupMove(const GroupMoveOperation& operation) {
        if (!ogame || !ogame->GetWorld() || operation.world != ogame->GetWorld()) return false;
        zCArray<zCVob*> list;
        operation.world->SearchVobListByBaseClass(zCVob::classDef, list, 0);
        std::unordered_set<zCVob*> live;
        for (int i = 0; i < list.GetNumInList(); ++i) live.insert(list[i]);
        for (const auto& entry : operation.entries) {
            if (!live.count(entry.vob) || entry.vob->GetParentVob() != entry.parent
                || CountGroupChildren(entry.vob) != entry.childCount) return false;
            auto mover = entry.vob->CastTo<zCMover>();
            if (mover && mover->keyframeList.GetNumInList() != (int)entry.moverKeys.size()) return false;
        }
        return !operation.entries.empty();
    }

    static bool CaptureGroupMove(GroupMoveOperation& operation) {
        if (!ogame || !ogame->GetWorld() || !Selector.GetCount()) return false;
        operation = GroupMoveOperation();
        operation.world = ogame->GetWorld();
        std::unordered_set<zCVob*> selected, visited;
        for (uint i = 0; i < theApp.SelectedVobs.GetNum(); ++i)
            selected.insert(theApp.SelectedVobs[i]);
        // Traverse selected roots in hierarchy order, including ALL their descendants.
        for (auto root : selected) {
            bool ancestorSelected = false;
            for (auto parent = root->GetParentVob(); parent; parent = parent->GetParentVob())
                if (selected.count(parent)) { ancestorSelected = true; break; }
            if (ancestorSelected) continue;
            std::vector<zCVob*> pending(1, root);
            while (!pending.empty()) {
                auto vob = pending.back();
                pending.pop_back();
                if (!visited.insert(vob).second) continue;
                GroupMoveEntry entry;
                entry.vob = vob;
                entry.parent = vob->GetParentVob();
                entry.position = vob->GetPositionWorld();
                entry.transform = vob->trafoObjToWorld;
                entry.selected = selected.count(vob) != 0;
                entry.collDyn = vob->collDetectionDynamic;
                entry.collStat = vob->collDetectionStatic;
                entry.childCount = CountGroupChildren(vob);
                if (auto mover = vob->CastTo<zCMover>())
                    for (int i = 0; i < mover->keyframeList.GetNumInList(); ++i) {
                        entry.moverKeys.push_back(mover->keyframeList[i].pos);
                        entry.moverRotations.push_back(mover->keyframeList[i].quat);
                    }
                operation.entries.push_back(entry);
                if (vob->globalVobTreeNode)
                    for (auto node = vob->globalVobTreeNode->GetFirstChild(); node; node = node->GetNextChild())
                        if (node->GetData()) pending.push_back(node->GetData());
            }
        }
        double x = 0, y = 0, z = 0;
        int selectedCount = 0;
        for (const auto& entry : operation.entries) if (entry.selected) {
            x += entry.position[VX]; y += entry.position[VY]; z += entry.position[VZ];
            ++selectedCount;
        }
        if (selectedCount)
            operation.pivot = zVEC3((float)(x/selectedCount), (float)(y/selectedCount), (float)(z/selectedCount));
        return !operation.entries.empty();
    }

    struct GroupCollisionScope {
        const std::vector<GroupMoveEntry>& entries;
        explicit GroupCollisionScope(const std::vector<GroupMoveEntry>& value) : entries(value) {
            for (const auto& entry : entries) {
                entry.vob->SetCollDetDyn(FALSE);
                entry.vob->SetCollDetStat(FALSE);
            }
        }
        ~GroupCollisionScope() {
            for (const auto& entry : entries) {
                entry.vob->SetCollDetDyn(entry.collDyn);
                entry.vob->SetCollDetStat(entry.collStat);
            }
        }
    };

    static void RotateGroupBasis(zMAT4& matrix, const SpacerSelection::Rotation& rotation) {
        matrix.SetRightVector(rotation.Apply(matrix.GetRightVector()));
        matrix.SetUpVector(rotation.Apply(matrix.GetUpVector()));
        matrix.SetAtVector(rotation.Apply(matrix.GetAtVector()));
    }

    static void ApplyGroupMove(const GroupMoveOperation& operation, const zVEC3& delta,
        const SpacerSelection::Rotation& rotation = SpacerSelection::Rotation()) {
        GroupCollisionScope collisions(operation.entries);
        SpacerSelection::ApplyTransform(operation.entries, delta, rotation, operation.pivot,
            [&](const GroupMoveEntry& entry, const zVEC3& target) {
                zMAT4 transform = entry.transform;
                if (entry.selected && !rotation.IsIdentity()) RotateGroupBasis(transform, rotation);
                transform.SetTranslation(target);
                entry.vob->SetTrafoObjToWorld(transform);
                if (auto mover = entry.vob->CastTo<zCMover>())
                    for (int i = 0; i < (int)entry.moverKeys.size(); ++i) {
                        auto& key = mover->keyframeList[i];
                        key.pos = entry.moverKeys[i];
                        key.quat = entry.moverRotations[i];
                        if (entry.selected) {
                            key.pos = operation.pivot + rotation.Apply(key.pos - operation.pivot) + delta;
                            if (!rotation.IsIdentity()) {
                                zMAT4 keyMatrix;
                                key.quat.QuatToMatrix4(keyMatrix);
                                RotateGroupBasis(keyMatrix, rotation);
                                key.quat.Matrix4ToQuat(keyMatrix);
                            }
                        }
                    }
            });
        // Update dependent data after every world transform has reached its final position.
        std::unordered_set<zCCSCamera*> cameras;
        for (const auto& entry : operation.entries) {
            if (auto wp = entry.vob->CastTo<zCVobWaypoint>()) {
                if (operation.world->GetWayNet()) {
                    auto point = operation.world->GetWayNet()->SearchWaypoint(wp);
                    if (point) point->UpdatePositionWorld();
                }
            }
            if (auto key = entry.vob->CastTo<zCCamTrj_KeyFrame>())
                if (key->cscam) cameras.insert(key->cscam);
            UpdatePfxBbox(entry.vob);
        }
        for (auto camera : cameras) camera->Refresh();
        theApp.SetChangesWereMade(true);
    }

    int MultiSelect::Translate(float x, float y, float z, bool continuous) {
        return Transform(x, y, z, false, continuous);
    }

    int MultiSelect::Rotate(float x, float y, float z, bool continuous) {
        return Transform(x, y, z, true, continuous);
    }

    int MultiSelect::Transform(float x, float y, float z, bool rotationMode, bool continuous) {
        if (rotationMode ? !SpacerSelection::ValidAngles(x,y,z) : !SpacerSelection::ValidOffset(x,y,z)) return -3;
        if (!ogame || !ogame->GetWorld() || theApp.isExit || theApp.g_bIsPlayingGame
            || theApp.camMan.cameraRun || theApp.GetPickMode() != SWM_MULTISELECT) return -1;
        if (x == 0 && y == 0 && z == 0) return 0;
        if (!continuous || (!activeGroupMove.entries.empty() && activeGroupMove.rotationMode != rotationMode))
            EndTranslation();
        if (activeGroupMove.entries.empty()) {
            if (!CaptureGroupMove(activeGroupMove)) return 0;
            activeGroupMove.rotationMode = rotationMode;
            if (theApp.options.GetIntVal("checkBoxBlockComplexVob"))
                for (const auto& entry : activeGroupMove.entries)
                    if (entry.selected && entry.childCount) {
                        activeGroupMove = GroupMoveOperation();
                        return -2;
                    }
        }
        if (!ValidateGroupMove(activeGroupMove)) {
            activeGroupMove = GroupMoveOperation();
            groupMoveUndo.clear();
            return -1;
        }
        zVEC3 next = activeGroupMove.delta;
        auto rotation = activeGroupMove.rotation;
        if (rotationMode) rotation = SpacerSelection::Rotation::Degrees(x,y,z) * rotation;
        else next += zVEC3(x,y,z);
        if (!SpacerSelection::ValidOffset(next[VX], next[VY], next[VZ])) return -3;
        for (const auto& entry : activeGroupMove.entries) {
            auto target = entry.selected
                ? activeGroupMove.pivot + rotation.Apply(entry.position-activeGroupMove.pivot) + next : entry.position;
            if (!std::isfinite(target[VX]) || !std::isfinite(target[VY]) || !std::isfinite(target[VZ]))
                return -3;
        }
        ApplyGroupMove(activeGroupMove, next, rotation);
        activeGroupMove.delta = next;
        activeGroupMove.rotation = rotation;
        int count = (int)theApp.SelectedVobs.GetNum();
        if (!continuous) EndTranslation();
        return count;
    }

    void MultiSelect::EndTranslation() {
        if (!activeGroupMove.entries.empty()) {
            if (activeGroupMove.HasChanges() && ValidateGroupMove(activeGroupMove)) {
                if (groupMoveUndo.size() == 32) groupMoveUndo.erase(groupMoveUndo.begin());
                groupMoveUndo.push_back(std::move(activeGroupMove));
            }
            activeGroupMove = GroupMoveOperation();
        }
        groupKeyboardMoving = false;
    }

    bool MultiSelect::CanUndoTranslation() {
        return !groupMoveUndo.empty() || activeGroupMove.HasChanges();
    }

    int MultiSelect::UndoTranslation() {
        if (!ogame || !ogame->GetWorld() || theApp.isExit || theApp.g_bIsPlayingGame
            || theApp.camMan.cameraRun || theApp.GetPickMode() != SWM_MULTISELECT) return -1;
        EndTranslation();
        if (groupMoveUndo.empty()) return 0;
        if (!ValidateGroupMove(groupMoveUndo.back())) {
            groupMoveUndo.clear();
            return -1;
        }
        ApplyGroupMove(groupMoveUndo.back(), zVEC3(0, 0, 0));
        groupMoveUndo.pop_back();
        return 1;
    }

    void MultiSelect::InvalidateUndo() {
        EndTranslation();
        groupMoveUndo.clear();
    }

    void MultiSelect::ForgetVob(zCVob* vob) {
        // Invalidate before the object can be destroyed or its address reused.
        activeGroupMove = GroupMoveOperation();
        groupMoveUndo.clear();
        groupKeyboardMoving = false;
        theApp.SelectedVobs ^= vob;
    }

    void MultiSelect::Reset() {
        InvalidateUndo();
        ClearSelection();
        AwaitingViewportInput = false;
        IgnoreActivationClick = false;
    }

    void MultiSelect::ManageKeys() {
        if (!ViewportInputReady()) {
            EndTranslation();
            CancelPick();
            DrawDx11();
            return;
        }
        if (keys.KeyPressed("VOB_TRANSLATE", true)) {
            SetTransformMode(0);
            return;
        }
        if (keys.KeyPressed("VOB_ROTATE", true)) {
            SetTransformMode(1);
            return;
        }
        if (keys.KeyPressed("MULTI_UNDO", true)) {
            UndoTranslation();
            return;
        }
        if (keys.KeyPressed("MULTI_PARENT_CUT", true) && theApp.globalParent) {
            EndTranslation();
            theApp.vobToCopy = theApp.globalParent;
            theApp.isVobParentChange = true;
            return;
        }
        if (keys.KeyPressed("VOB_INSERT", true) && theApp.globalParent && theApp.isVobParentChange) {
            InvalidateUndo();
            ReparentSelected(theApp.globalParent);
            theApp.isVobParentChange = false;
            return;
        }
        DoPick();
        if (Locked || zinput->GetMouseButtonPressedRight()) {
            EndTranslation();
            return;
        }
        if (theApp.options.GetIntVal("multiSelectionTransformMode") == 1) {
            zVEC3 angles(0,0,0);
            if (keys.KeyPressed("VOB_ROT_FORWARD", false, true)) angles[VX] -= 1;
            if (keys.KeyPressed("VOB_ROT_BACK", false, true)) angles[VX] += 1;
            if (keys.KeyPressed("VOB_ROT_LEFT", false, true)) angles[VZ] += 1;
            if (keys.KeyPressed("VOB_ROT_RIGHT", false, true)) angles[VZ] -= 1;
            if (keys.KeyPressed("VOB_ROT_VERT_LEFT", false, true)) angles[VY] -= 1;
            if (keys.KeyPressed("VOB_ROT_VERT_RIGHT", false, true)) angles[VY] += 1;
            if (angles.Length_Sqr() > 0) {
                float speed = (float)theApp.options.GetIntVal("vobRotSpeed") / 1000.0f;
                if (keys.KeyPressed("VOB_SPEED_X10", false, true)) speed *= 3;
                if (keys.KeyPressed("VOB_SPEED_MINUS_10", false, true)) speed /= 10;
                angles = angles * (speed * ztimer->frameTimeFloat);
                int result = Rotate(angles[VX], angles[VY], angles[VZ], true);
                groupKeyboardMoving = result > 0;
                if (result == -2) print.PrintRed(GetLang("VOB_CHANGE_POS_BLOCKED"));
            } else if (groupKeyboardMoving) EndTranslation();
            return;
        }
        zVEC3 delta(0, 0, 0);
        auto camera = ogame->GetCameraVob();
        if (camera) {
            zVEC3 forward = camera->GetAtVectorWorld();
            forward[VY] = 0;
            if (forward.Length_Sqr() < 0.000001f) {
                zVEC3 right = camera->GetRightVectorWorld();
                right[VY] = 0;
                forward = zVEC3(0, 1, 0).Cross(right);
            }
            if (forward.Length_Sqr() > 0.000001f) forward.Normalize();
            zVEC3 left = forward.Cross(zVEC3(0, 1, 0));
            if (keys.KeyPressed("VOB_TRANS_FORWARD", false, true)) delta += forward;
            if (keys.KeyPressed("VOB_TRANS_BACKWARD", false, true)) delta -= forward;
            if (keys.KeyPressed("VOB_TRANS_LEFT", false, true)) delta += left;
            if (keys.KeyPressed("VOB_TRANS_RIGHT", false, true)) delta -= left;
            if (keys.KeyPressed("VOB_TRANS_UP", false, true)) delta[VY] += 1;
            if (keys.KeyPressed("VOB_TRANS_DOWN", false, true)) delta[VY] -= 1;
        }
        if (delta.Length_Sqr() > 0) {
            // Preserve group shape: one size-independent speed for every selected object.
            delta.Normalize();
            float speed = (float)theApp.options.GetIntVal("vobTransSpeed") / 100000.0f;
            if (keys.KeyPressed("VOB_SPEED_X10", false, true)) speed *= 10;
            if (keys.KeyPressed("VOB_SPEED_MINUS_10", false, true)) speed /= 10;
            int result = Translate(delta[VX] * speed * ztimer->frameTimeFloat,
                delta[VY] * speed * ztimer->frameTimeFloat,
                delta[VZ] * speed * ztimer->frameTimeFloat, true);
            groupKeyboardMoving = result > 0;
            if (result == -2) print.PrintRed(GetLang("VOB_CHANGE_POS_BLOCKED"));
        }
        else if (groupKeyboardMoving) EndTranslation();

    }
}
