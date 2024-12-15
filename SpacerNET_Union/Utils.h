// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	template<typename Key, typename Value>
	void DeleteAndClearMap(Common::Map<Key, Value*>& map)
	{
		for (auto& pointer : map)
		{
			pointer.Delete();
		}

		map.Clear();
	}
}