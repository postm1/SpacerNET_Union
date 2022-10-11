// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .



	typedef void(*ptr_EVENTFUNC)(zCVob*);

	// массивы зарегистрированных указателей на функции, которые вызовуются при возникновении событий
	Array<ptr_EVENTFUNC> arr_OnCreateVob_RegFuncs;
	Array<ptr_EVENTFUNC> arr_OnDeleteVob_RegFuncs;
	Array<ptr_EVENTFUNC> arr_OnApplyDataToVob_RegFuncs;
	Array<ptr_EVENTFUNC> arr_OnSelectVob_RegFuncs;


	void CALL_OnCreateVob(zCVob* pVob)
	{
		// пробегаемся по всем указателям на функции
		for (uint i = 0; i < arr_OnCreateVob_RegFuncs.GetNum(); i++)
		{
			// вызываем функцию
			arr_OnCreateVob_RegFuncs[i](pVob);
		}
	}

	void CALL_OnDeleteVob(zCVob* pVob)
	{
		// пробегаемся по всем указателям на функции
		for (uint i = 0; i < arr_OnDeleteVob_RegFuncs.GetNum(); i++)
		{
			// вызываем функцию
			arr_OnDeleteVob_RegFuncs[i](pVob);
		}
	}

	void CALL_OnApplyDataToVob(zCVob* pVob)
	{
		// пробегаемся по всем указателям на функции
		for (uint i = 0; i < arr_OnApplyDataToVob_RegFuncs.GetNum(); i++)
		{
			// вызываем функцию
			arr_OnApplyDataToVob_RegFuncs[i](pVob);
		}
	}

	void CALL_OnSelectVob(zCVob* pVob)
	{
		// пробегаемся по всем указателям на функции
		for (uint i = 0; i < arr_OnSelectVob_RegFuncs.GetNum(); i++)
		{
			// вызываем функцию
			arr_OnSelectVob_RegFuncs[i](pVob);
		}
	}

	// экспорт функции для других DLL
	extern "C"
	{
		// добавляет текст в окно Информации, color в формате html #000000
		__declspec(dllexport) void SPC_SendMsgInfo(string txt, string color)
		{
			Stack_PushString(txt);
			Stack_PushString(color);
			(callVoidFunc)GetProcAddress(theApp.module, "InfoWin_AddText")();
		}


		// возвращает текущий выделенный объект
		__declspec(dllexport) zCVob* SPC_GetSelectedObject()
		{
			return theApp.GetSelectedVob();
		}


		// обновляет свойства воба в окне редактора
		__declspec(dllexport) void SPC_RefreshProps()
		{
			if (auto pVob = theApp.GetSelectedVob())
			{
				theApp.SelectObject(pVob);
			}
		}

		__declspec(dllexport) void SPC_RegEventFunc(enum SPC_EVENTLIST evt, ptr_EVENTFUNC func)
		{
			// если указателя на функцию нет
			if (!func)
				// выходим
				return;

			// Иначе, добавляем в соотв. массив функций:
			switch (evt)
			{
			case Gothic_II_Addon::SPC_EVT_OnCreateVob:
				arr_OnCreateVob_RegFuncs.Insert(func);
				break;
			case Gothic_II_Addon::SPC_EVT_OnDeleteVob:
				arr_OnDeleteVob_RegFuncs.Insert(func);
				break;
			case Gothic_II_Addon::SPC_EVT_OnApplyDataToVob:
				arr_OnApplyDataToVob_RegFuncs.Insert(func);
				break;
			case Gothic_II_Addon::SPC_EVT_OnSelectVob:
				arr_OnSelectVob_RegFuncs.Insert(func);
				break;
			default:
				break;
			}
		}


		__declspec(dllexport) void SPC_RemoveVob(zCVob* pVob)
		{
			theApp.RemoveVob(pVob);
		}


		__declspec(dllexport) void SPC_RemoveAllChilds(zCVob* pVob)
		{
			theApp.exports.toggleGlobalTree(0);
			theApp.RemoveAllChilds(pVob);
			theApp.exports.toggleGlobalTree(1);
		}


		__declspec(dllexport) void SPC_SetGlobalTreeUpdate(int toggle)
		{
			theApp.exports.toggleGlobalTree(toggle);
		}

	}
}