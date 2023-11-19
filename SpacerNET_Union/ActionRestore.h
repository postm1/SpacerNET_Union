// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	enum ActionRestoreType
	{
		ART_NONE,
		ART_VOB_POS
	};


	struct ActionRestoreEntry
	{
	public:
		zCVob* pVob;
		ActionRestoreType type;
		zVEC3 pos;
		zMAT4 rot;

		ActionRestoreEntry::ActionRestoreEntry()
		{
			pVob = NULL;
			type = ART_NONE;
		}

	};

	class ActionRestore
	{
	public:
		void Init();
		void Reset();
		void RemoveByVob(zCVob*);
		void GenerateVobsPos();
		void RestoreVobPos(zCVob*);
		ActionRestoreEntry* AddNewVobPos(zCVob* pVob);
		ActionRestoreEntry* Add(zCVob*, ActionRestoreType type);

		zCList<ActionRestoreEntry> pList;

	};
}