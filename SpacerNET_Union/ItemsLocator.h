// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct ItemsLocatorEntry
	{
		oCItem* pVob;
		zCView* pView;

		ItemsLocatorEntry::ItemsLocatorEntry()
		{
			pView = NULL;
			pVob = NULL;
		}
	};
	class ItemsLocator
	{
	public:
		struct
		{
			bool show;
			bool init;
			zCArray<oCItem*> pListItems;
			zCArray<ItemsLocatorEntry*> pListEntries;
			int sx, sy;
			zCView* pLocatorView;
			bool showOnlyProbablyBad;
			int offsetVertical;
			int searchType;
		};
	public:
		void Loop();
		void Init();
		void Toggle();
		void Show();
		void Hide();
		void Reset();
		void Draw(ItemsLocatorEntry* drawEntry);

		void RemoveByItem(zCVob*);

		zVEC3 GetFocus(zCVob* vob);
		bool IsBadItem(zCVob* vob);
		bool IsItemMatchCategory(oCItem* pItem);

		ItemsLocator();
	};
}