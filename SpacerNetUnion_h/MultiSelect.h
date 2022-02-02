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
		zCView* BoundsView;

		MultiSelect();
		void DrawBounds();
		void DoPick();
		void PickSingle(zCVob* vob);
		void PickBounds();
		void AddToSelection(zCVob* vob);
		void RemoveFromSelection(zCVob* vob);
		void ClearSelection();
		void RemoveSelected();
		void DrawDx11();
	};

	MultiSelect Selector;

}