// Supported with union (c) 2020 Union team
// Union HEADER file


#include <string>
#include <chrono>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <tuple>
#include <emmintrin.h>  // SSE2 intrinsics

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

    struct
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        bool started = false;

        void Start()
        {
            //cmd << "========== DEBUG TIMER START ============" << endl;

            start_time = std::chrono::high_resolution_clock::now();
            started = true;
        }

        void Stop()
        {
            started = false;
            //cmd << "========== DEBUG TIMER STOPPED ============" << endl;
        }

        zSTRING GetDelta()
        {
            zSTRING info;

            if (!started)
            {
                return info;
            }

            auto current_time = std::chrono::high_resolution_clock::now();

            auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
                current_time - start_time).count();

            start_time = current_time;


            info += " Delta: " + zSTRING((int)delta) + " ms";

            return info;
            
        }

    } globalTimeDebug;

}