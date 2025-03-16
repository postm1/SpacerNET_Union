// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	// Возвращает число сегментов для построения окружностей сферы,
// используя радиус источника освещения "sphereRadius" 
	float GetNumSphereSegmentsAuto(float sphereRadius)
	{
		float numSeg;		// получившееся число сегментов
		float maxSeg;		// верхнее ограничение числа сегментов
		float minSeg;		// нижнее ограничение числа сегментов
		float targetRadius;	// на этом радиусе достигается целевое число сегментов
		float targetSeg;	// целевое или макс. число сегментов
		float kSpeed;		// инверсн. коэф. скорости нарастания числа сегментов (0.1-0.99)
							// чем меньше kSpeed, тем круглей окружности или сфера

		// для орбитальной сферы - свои параметры
		if (theApp.lightSphereSettings.sphereDrawMode == eSphereDrawMode::DRAW_ONLY_ORBITS)
		{
			minSeg = 4.0f;
			maxSeg = 360.0f;
			kSpeed = 0.5f;
			targetRadius = 20000.0f;
			targetSeg = maxSeg;
		}
		// для полной сферы - свои
		else
		{
			minSeg = 6.0f;
			maxSeg = 60.0f;
			kSpeed = 0.35f;
			targetRadius = 8000.0f;
			targetSeg = maxSeg;
		}

		// вычисляем число сегментов для окружностей сферы
		numSeg = floorf(targetSeg * pow((sphereRadius / targetRadius), kSpeed));

		// ограничиваем мин./макс. значениями
		zClamp(numSeg, minSeg, maxSeg);

		// возвращаем результат
		return numSeg;
	}



	// Рисует три окружности сферы, по осям XYZ, где:
	// sphere - структура с информацией о сфере источника(центр и её радиус),
	// wireColor - цвет сетки окружностей,
	// trafo - указатель на матрицу трансформаций объекта (для учёта его вращения)
	void DrawSphereOrbits(zTBSphere3D& sphere, const zCOLOR& wireColor, zMAT4* trafo = NULL)
	{
		// получаем число сегментов по радиусу сферы
		float numSegments = GetNumSphereSegmentsAuto(sphere.radius);

		// делим весь контур окружности на число её сегментов,
		// тем самым получая шаг изменения угла,
		// для рисования контура этой окружности
		float angle_step = 360.0f / numSegments;

		// порядковый номер изображаемой окружности
		// 0, 1, 2 -> OK, 3 -> Exit
		int nCircle = 0;

		// временный угол (в градусах),
		// изменяется с помощью "angle_step"
		float angle = 0;

		// временная точка на окружности
		// (в мировых координатах)
		zVEC3 current_point;

		// предыдущая точка
		// (в мировых координатах)
		zVEC3 prev_point;

		// рисовать сетку с учётом пересечения с полигонами мира
		BOOL bUseWireGeometryInTheWorld = TRUE;

		// рисуем три окружности (т.е. во всех трёх плоскостях)
		while (nCircle < 3)
		{
			// переводим угол из градусов в радианы
			float rad = angle / DEGREE;

			// находим локальные координаты точки на окружности
			float x = sphere.radius * cos(rad);
			float y = sphere.radius * sin(rad);


			// образуем локальную точку на окружности,
			// в зависимости от плоскости (0, 1, 2)
			if (nCircle == 0)
				current_point = zVEC3(x, y, 0);
			else if (nCircle == 1)
				current_point = zVEC3(x, 0, y);
			else if (nCircle == 2)
				current_point = zVEC3(0, x, y);

			// переводим точку на окружности из локальной системы в мировую,
			// либо с помощью матрицы трансформаций, либо переносом центра окружности
			current_point = trafo ? (*trafo * current_point) : (sphere.center + current_point);


			// на нулевом градусе - просто запоминаем точку,
			// в остальных случаях - соединяем точки на окружности
			if (angle > 0)
				// соединяем две точки на окружности
				zlineCache->Line3D(prev_point, current_point, wireColor, bUseWireGeometryInTheWorld);


			// если мы ещё не нарисовали полный круг
			if (angle < 360.0f)
			{
				// запоминаем прошлую точку
				prev_point = current_point;

				// увеличиваем угол на градус
				angle += angle_step;
			}
			else // иначе, окружность нарисована
			{
				// переходим к следующей
				nCircle++;

				// сбрасываем угол
				angle = 0;
			}
		} /* end of loop by circles */
	}




	// Рисует полную сетку сферы, где:
	// sphere - параметры сферы (центр и её радиус),
	// wireColor - цвет сетки сферы
	// trafo - указатель на матрицу трансформаций объекта (для учёта его вращения)
	void DrawWireSphere(zTBSphere3D& sphere, const zCOLOR& wireColor, zMAT4* trafo = NULL)
	{
		// радиус сферы
		float R = sphere.radius;

		// координаты её центра
		zVEC3 vPos = sphere.center;

		// локальные координаты вершины сферы
		zVEC3 vCap = zVEC3(0, R, 0);

		// координаты точек сегмента сферы
		zVEC3 v1, v2, v3, v4;

		// рисовать сетку с учётом пересечения с полигонами мира
		BOOL bUseWireGeometryInTheWorld = TRUE;

		// получаем число сегментов по радиусу сферы
		float numSegments = GetNumSphereSegmentsAuto(R);

		// временная точка
		zVEC3 pt;

		// флаг отрисовки левого края сегмента
		// (0 - ещё не нарисован, 1 - уже нарисован)
		BOOL LSegDrawed = 0;

		// подгонка шага горизонтальных орбит
		float h_step = (2.0f * PI) / numSegments;

		// подгонка шага вертикальных орбит
		float v_step = h_step / 1.3f;

		// 2 ПИ
		float calc2PI = 2.0f * PI;

		// половина ПИ
		float calcHalfPI = PI / 2.0f;

		// находим точки на вертикальной полу-орбите, для построения горизонтальных орбит
		// (рисуем сферу снизу вверх)
		for (float v = -calcHalfPI + v_step; v < calcHalfPI - v_step; v += v_step)
		{
			// находим следующую точку на вертикальной полуорбите,
			// чтобы построить горизонтальный ряд сегментов вокруг сферы
			float next_v = v + v_step;

			// находим точки на горизонтальной полной орбите сферы
			for (float h = 0; h <= calc2PI; h += h_step)
			{
				// текущая окружность
				v2 = zVEC3(R * cos(v) * cos(h), R * sin(v), R * cos(v) * sin(h));

				// следующая окружность
				v4 = zVEC3(R * cos(next_v) * cos(h), R * sin(next_v), R * cos(next_v) * sin(h));

				// если нужно вращение сетки
				if (trafo)
				{
					// используем матрицу объекта
					// для вычисления координат
					v2 = *trafo * v2;
					v4 = *trafo * v4;
				}
				else
				{
					// иначе, используем только перемещение
					v2 = vPos + v2;
					v4 = vPos + v4;
				}


				// рисуем сегмент сферы, используя найденные точки: v1, v2, v3, v4
				// первую итерацию пропускаем, потому что координаты точек v1 и v3 - неизвестны,
				// они расчитываются ниже, после этого условного блока и будут доступны только в следующей итерации
				if (h > 0)
				{
					// верхняя линия
					zlineCache->Line3D(v1, v2, wireColor, bUseWireGeometryInTheWorld);

					// нижняя линия
					zlineCache->Line3D(v3, v4, wireColor, bUseWireGeometryInTheWorld);

					// если левая линия сегмента ещё не нарисована
					if (LSegDrawed == 0)
					{
						// рисуем её
						zlineCache->Line3D(v1, v3, wireColor, bUseWireGeometryInTheWorld);

						// ставим флаг, который запрещает рисовать линии с левой стороны
						// следующих сегментов в этом горизонтальном ряду
						// таким образом первый сегмент рисуется полностью, а остальные - частично,
						// чтобы не было лишних линий
						LSegDrawed = 1;
					}

					// правая линия
					zlineCache->Line3D(v2, v4, wireColor, bUseWireGeometryInTheWorld);
				}


				/******************************************************/
				// Постепенное рисование нижней и верхней шапки сферы
				/******************************************************/
				// если это первый ряд рисования сегментов сферы
				if ((v + calcHalfPI - v_step) < 0.01f)
				{
					// если нужно учитывать вращение сетки, применяем его, иначе, используем простое перемещение
					pt = trafo ? (*trafo * (-vCap)) : vPos - vCap;

					// рисуем нижнюю шапку, путём соединения полученных точек и нижней точки сферы
					zlineCache->Line3D(pt, v2, wireColor, bUseWireGeometryInTheWorld);
				}
				// или если это последний ряд рисования сегментов
				else if ((v + v_step) >= (calcHalfPI - v_step))
				{
					// если нужно учитывать вращение сетки,
					// применяем его, используя матрицу трансформаций объекта,
					// иначе, используем простое перемещение
					pt = trafo ? (*trafo * vCap) : vPos + vCap;

					// рисуем верхнюю шапку, путём соединения полученных точек и верхней точки сферы
					zlineCache->Line3D(pt, v4, wireColor, bUseWireGeometryInTheWorld);
				}



				// запоминаем точку на текущей окружности,
				// для использования в следующей итерации цикла
				v1 = v2;

				// запоминаем точку на следующей окружности,
				// для использования в следующей итерации цикла
				v3 = v4;


				// если движение по горизонтальным и вертикальным орбирам сферы достигло конца
				if (((h + h_step) > calc2PI) && ((v + v_step) >= (calcHalfPI - v_step)))
				{
					// находим координату крайней точки
					pt = zVEC3(R * cos(v + v_step) * cos(0.0f), R * sin(v + v_step), R * cos(v + v_step) * sin(0.0f));

					// и если нужно учитывать вращение сетки, применяем его,
					// иначе, используем простое перемещение
					pt = trafo ? (*trafo * pt) : vPos + pt;

					// соединяем концы последнего горизонтального круга (сверху)
					zlineCache->Line3D(v4, pt, wireColor, bUseWireGeometryInTheWorld);
				}
			}


			// после того, как нарисовали горизонтальный ряд сегментов,
			// сбрасываем флаг отрисовки левого края сегмента,
			// чтобы его можно было снова рисовать
			LSegDrawed = 0;


			// замыкание контура каждого горизонтального круга:
			// находим координату крайней точки
			pt = zVEC3(R * cos(v) * cos(0.0f), R * sin(v), R * cos(v) * sin(0.0f));

			// и если нужно учитывать вращение сетки, применяем его,
			// иначе, используем простое перемещение
			pt = trafo ? (*trafo * pt) : vPos + pt;

			// замыкаем горизонтальный контур
			zlineCache->Line3D(v2, pt, wireColor, bUseWireGeometryInTheWorld);
		}
	}



	



	// В цикле обработки источников света
	void DrawLightSphere_Loop()
	{
		// если отрисовка сетки выключена
		if (theApp.lightSphereSettings.sphereDrawMode == eSphereDrawMode::DRAW_NONE)
			// выходим
			return;

		// иначе, получаем выделенный объект и пытаемся преобразовать его в источник света
		zCVobLight* pLight = dynamic_cast<zCVobLight*>(SPC_GetSelectedObject());

		// если это не "свет"
		if (!pLight)
			// выходим
			return;

		// иначе, формируем основные данные
		// об этом источнике света:
		// радиус освещения и его координаты в мире
		zTBSphere3D s;
		s.radius = pLight->lightData.range;
		s.center = pLight->GetPositionWorld();

		// матрица трансформаций для сетки сферы;
		// либо есть, либо её нет, в зависимости от флага "bUseSphereTrafo";
		// если есть, то сетка сферы будет также вращаться вместе с источником;
		zMAT4* sphere_trafo = theApp.lightSphereSettings.bUseSphereTrafo ? &pLight->trafoObjToWorld : NULL;


		/**********************************************/
		//  Сетка источника (орбиты или полная сфера)
		/**********************************************/
		// в режиме отображения орбит
		if (theApp.lightSphereSettings.sphereDrawMode == eSphereDrawMode::DRAW_ONLY_ORBITS)
			// рисуем только орбиты
			DrawSphereOrbits(s, theApp.lightSphereSettings.sphereWireColor, sphere_trafo);
		else // в режиме отображения полной сетки сферы,
			// рисуем всю сетку сферы
			DrawWireSphere(s, theApp.lightSphereSettings.sphereWireColor, sphere_trafo);


		/***************************/
		//  Вертикальная ось сферы
		/***************************/
		// формируем локальный вектор от центра до вершины сферы
		zVEC3 pt1(0, s.radius, 0);

		// и такой же вектор, только до дна сферы
		zVEC3 pt2 = -pt1;

		// если вращение сетки не нужно
		if (!sphere_trafo)
		{
			// преобразуем координаты точек
			// из локальных в мировые только с помощью перемещения
			pt1 += s.center;
			pt2 += s.center;
		}
		else
		{
			// иначе, преобразуем координаты с помощью матрицы трансформаций,
			// учитывающей и вращение тоже
			pt1 = *sphere_trafo * pt1;
			pt2 = *sphere_trafo * pt2;
		}

		// рисуем линию, соединяющую вершину и дно сферы
		zlineCache->Line3D(pt1, pt2, theApp.lightSphereSettings.sphereWireColor, 1);


		/***************************/
		//  Значение радиуса сферы
		/***************************/
		// если нужно вывести радиус сферы
		if (theApp.lightSphereSettings.bDrawRadiusValue == TRUE)
		{
			// заполняем строку текстом, с учётом формата вывода строки "radiusTextFmt"
			zSTRING txt = string::Combine(theApp.lightSphereSettings.radiusTextFmt.ToChar(), string(s.radius, 0));

			int x, y;
			// получаем проекцию координат источника на экране
			GetProjection(x, y, s.center);

			// если источник в пределах видимости камеры
			if (!(x < 0 || x > 8192 || y < 0 || y > 8192 || (x == 0 && y == 0)))
			{
				// получаем ширину текста (в вирт. координатах)
				int txt_vsizex = screen->FontSize(txt);

				// выводим значение радиуса на экран
				screen->Print(x - txt_vsizex / 2, y + zPixelY(32 / 2 + 7), txt);
			}
		}
	}
}