// Supported with union (c) 2020 Union team
// Union HEADER file


#include <string>
#include <chrono>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <tuple>
#include <emmintrin.h>  // SSE2 intrinsics

namespace GOTHIC_ENGINE {
	// Add your code here . . .

    const zREAL zALG_EQUAL_EPSILON = zREAL(0.001);

    inline float zAbs(const float a)
    {
        return fabs(a);
    }

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
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_global;
        bool started = false;

        void Start()
        {
            //cmd << "========== DEBUG TIMER START ============" << endl;

            start_time = std::chrono::high_resolution_clock::now();
            start_time_global = start_time;
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


            info += " (Delta: " + zSTRING((int)delta) + " ms)";

            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                    current_time - start_time_global).count();

            info += " (GlobalTime: " + Z (int)diff + " ms)";

            if (delta >= 50)
            {
                info += " <---- Long time";
            }

            return info;
            
        }

    } globalTimeDebug;

}