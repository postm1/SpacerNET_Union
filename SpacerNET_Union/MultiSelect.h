// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct MultiSelect {
		zVEC2  PickBegin;
		zVEC2  PickEnd;
		zVEC2  Bounds[2];
		bool_t Locked;
		bool_t TraceRayEnabled;
        bool AwaitingViewportInput;
        bool IgnoreActivationClick;
		zCView* BoundsView;

		MultiSelect();
		void DrawBounds();
		void CancelPick();
		void DoPick();
		void PickSingle(zCVob* vob);
		void PickBounds();
		void AddToSelection(zCVob* vob);
		void RemoveFromSelection(zCVob* vob);
		void ClearSelection();
		void RemoveSelected();
		void SaveSelected();
		int ReparentSelected(zCVob* newParent);
		void DrawDx11();
        // Viewport keyboard/rectangle input; UI focus ends the current transform gesture.
        void ManageKeys();
        // Consult the managed interface. A missing callback blocks viewport editing.
        bool InputBlocked();
        // Consume focus-acquisition LMB through mouse-up, preserving the existing group.
        bool ViewportInputReady();
        // Requires a live pointer; tests editability and the current persisted filter.
        bool MatchesFilter(zCVob* vob);
        // Valid IDs: [0, FilterCount). Finish the gesture and prune nonmatching members.
        void SetFilter(int filter);
        // 0 = translation, 1 = rotation; invalid values are ignored.
        void SetTransformMode(int mode);
        // Prune stale/nonmatching pointers before returning the current group size.
        int GetCount();
        // Validate an address from Objects list: action 0 replaces, 1 adds, 2 removes.
        // Returns group size, or -1 on invalid editor state/address/action/filter.
        int ChangeSelection(zCVob* vob, int action);
        // World-space deltas. continuous=true coalesces calls until EndTranslation.
        // Transform results: >0 affected members, 0 no-op, -1 invalid state/snapshot,
        // -2 children blocked by settings, -3 invalid numeric input/accumulated offset.
        int Translate(float x, float y, float z, bool continuous = false);
        // Degrees about world X, then Y, then Z, around the mean of selected origins.
        // Same result codes and gesture semantics as Translate.
        int Rotate(float x, float y, float z, bool continuous = false);
        // Shared translation/rotation transaction; also validates hierarchy and mover keys.
        int Transform(float x, float y, float z, bool rotation, bool continuous);
        // Commit a changed, valid active gesture to the bounded 32-operation history.
        void EndTranslation();
        // Restore positions, orientations and mover keys: 1 success, 0 empty, -1 rejected.
        int UndoTranslation();
        // Availability hint; UndoTranslation performs the final state/snapshot validation.
        bool CanUndoTranslation();
        // Finish any active gesture, then discard all group-transform undo history.
        void InvalidateUndo();
        // Called before destruction: forget the pointer and invalidate every snapshot.
        void ForgetVob(zCVob* vob);
        // World/reset cleanup: selection, undo and viewport-activation state.
        void Reset();
	};

	MultiSelect Selector;

}
