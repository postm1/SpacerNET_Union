// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	bool FetchNameFromString(Common::string& name, uint& index) {
		Common::string baseName = name;
		Common::string index_s = baseName.GetWord("_", -1);
		if (index_s.IsNumber()) {
			index = index_s.ToInt32();
			name.Cut(index_s.Length() + 1);
			return true;
		}

		index = Invalid;
		return false;
	}


	void CollectWaypointsByName(Common::string baseName, uint& startIndex) {
		
		if (baseName.IsEmpty())
		{
			return;
		}

		auto* list = ogame->GetGameWorld()->wayNet->wplist.next;
		while (list) {
			zCWaypoint* wp = list->data;
			Common::string name = wp->name;
			uint index;
			if (FetchNameFromString(name, index))
				if (name == baseName)
				{
					if (index > startIndex)
					{
						startIndex = index;
					}
				}
					

			list = list->next;
		}

		// set new next index
		startIndex++;
	}

	// Checks whether the collection of waypoints has the same index
	bool HasWayPointInCollection(uint targetIndex, Common::Array<zCWaypoint*> wayPoints) {
		for (uint i = 0; i < wayPoints.GetNum(); i++) {
			Common::string baseName = wayPoints[i]->name;
			uint index;
			if (FetchNameFromString(baseName, index))
				if (index == targetIndex)
					return true;
		}

		return false;
	}

	Common::string FormatIndex(uint index) {
		Common::string index_s = index;
		for (uint i = index_s.Length(); i <= 2; i++)
			index_s = "0" + index_s;

		return index_s;
	}

	Common::string GetNextWayPointName(Common::string copiedName) {
		Common::string baseName = copiedName;
		uint index;
		FetchNameFromString(baseName, index);

		uint nextIndex = 0;

		CollectWaypointsByName(baseName, nextIndex);


	/*	for (nextIndex = 0; nextIndex < wayPoints.GetNum(); nextIndex++)
			if (!HasWayPointInCollection(nextIndex, wayPoints))
				break;*/

		Common::string nextName = baseName + "_" + FormatIndex(nextIndex);

		if (ogame->GetWorld()->wayNet->GetWaypoint(nextName))
		{
			static int errIndex = 0;

			Message::Box("ERROR: Such WP already exists!");
			nextName = "ERR_NAME_CHANGE_IT_" + Common::string(errIndex);
		}

		return nextName;
	}


	//======================================================================
	void CollectFreepointsByName(Common::string baseName, uint& nextIndex) {
		Common::Array<zCVobSpot*> freePoints;
		if (baseName.IsEmpty())
			return;

		zCArray<zCVob*> vobList;

		ogame->GetWorld()->SearchVobListByClass(zCVobSpot::classDef, vobList, 0);

		for (int i = 0; i < vobList.GetNumInList(); i++)
		{
			zCVob* vob = vobList.GetSafe(i);
			zCVobSpot* spot = dynamic_cast<zCVobSpot*>(vob);

			if (vob && spot)
			{
				Common::string name = vob->GetVobName();
				uint index;

				if (FetchNameFromString(name, index))
					if (name == baseName)
					{
						if (index > nextIndex)
						{
							nextIndex = index;
						}
					}

			}
		}

		nextIndex++;

	}

	bool HasFreepointInCollection(uint targetIndex, Common::Array<zCVobSpot*> freePoints) {
		for (uint i = 0; i < freePoints.GetNum(); i++) {
			Common::string baseName = freePoints[i]->GetVobName();
			uint index;
			if (FetchNameFromString(baseName, index))
				if (index == targetIndex)
					return true;
		}

		return false;
	}

	Common::string GetNextFreePointName(Common::string copiedName) {
		Common::string baseName = copiedName;
		uint index;
		FetchNameFromString(baseName, index);

		uint nextIndex = 0;

		CollectFreepointsByName(baseName, nextIndex);

		/*
		for (nextIndex = 0; nextIndex < freePoints.GetNum(); nextIndex++)
			if (!HasFreepointInCollection(nextIndex, freePoints))
				break;
		*/

		Common::string nextName = baseName + "_" + FormatIndex(nextIndex);
		return nextName;
	}
}