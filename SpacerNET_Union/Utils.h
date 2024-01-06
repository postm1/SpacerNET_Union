template<typename Key, typename Value>
void DeleteAndClearMap(Common::Map<Key, Value*>& map)
{
	for (auto& pointer : map)
	{
		pointer.Delete();
	}

	map.Clear();
}
