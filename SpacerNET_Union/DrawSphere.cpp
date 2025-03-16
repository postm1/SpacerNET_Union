// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	// ���������� ����� ��������� ��� ���������� ����������� �����,
// ��������� ������ ��������� ��������� "sphereRadius" 
	float GetNumSphereSegmentsAuto(float sphereRadius)
	{
		float numSeg;		// ������������ ����� ���������
		float maxSeg;		// ������� ����������� ����� ���������
		float minSeg;		// ������ ����������� ����� ���������
		float targetRadius;	// �� ���� ������� ����������� ������� ����� ���������
		float targetSeg;	// ������� ��� ����. ����� ���������
		float kSpeed;		// �������. ����. �������� ���������� ����� ��������� (0.1-0.99)
							// ��� ������ kSpeed, ��� ������� ���������� ��� �����

		// ��� ����������� ����� - ���� ���������
		if (theApp.lightSphereSettings.sphereDrawMode == eSphereDrawMode::DRAW_ONLY_ORBITS)
		{
			minSeg = 4.0f;
			maxSeg = 360.0f;
			kSpeed = 0.5f;
			targetRadius = 20000.0f;
			targetSeg = maxSeg;
		}
		// ��� ������ ����� - ����
		else
		{
			minSeg = 6.0f;
			maxSeg = 60.0f;
			kSpeed = 0.35f;
			targetRadius = 8000.0f;
			targetSeg = maxSeg;
		}

		// ��������� ����� ��������� ��� ����������� �����
		numSeg = floorf(targetSeg * pow((sphereRadius / targetRadius), kSpeed));

		// ������������ ���./����. ����������
		zClamp(numSeg, minSeg, maxSeg);

		// ���������� ���������
		return numSeg;
	}



	// ������ ��� ���������� �����, �� ���� XYZ, ���:
	// sphere - ��������� � ����������� � ����� ���������(����� � � ������),
	// wireColor - ���� ����� �����������,
	// trafo - ��������� �� ������� ������������� ������� (��� ����� ��� ��������)
	void DrawSphereOrbits(zTBSphere3D& sphere, const zCOLOR& wireColor, zMAT4* trafo = NULL)
	{
		// �������� ����� ��������� �� ������� �����
		float numSegments = GetNumSphereSegmentsAuto(sphere.radius);

		// ����� ���� ������ ���������� �� ����� � ���������,
		// ��� ����� ������� ��� ��������� ����,
		// ��� ��������� ������� ���� ����������
		float angle_step = 360.0f / numSegments;

		// ���������� ����� ������������ ����������
		// 0, 1, 2 -> OK, 3 -> Exit
		int nCircle = 0;

		// ��������� ���� (� ��������),
		// ���������� � ������� "angle_step"
		float angle = 0;

		// ��������� ����� �� ����������
		// (� ������� �����������)
		zVEC3 current_point;

		// ���������� �����
		// (� ������� �����������)
		zVEC3 prev_point;

		// �������� ����� � ������ ����������� � ���������� ����
		BOOL bUseWireGeometryInTheWorld = TRUE;

		// ������ ��� ���������� (�.�. �� ���� ��� ����������)
		while (nCircle < 3)
		{
			// ��������� ���� �� �������� � �������
			float rad = angle / DEGREE;

			// ������� ��������� ���������� ����� �� ����������
			float x = sphere.radius * cos(rad);
			float y = sphere.radius * sin(rad);


			// �������� ��������� ����� �� ����������,
			// � ����������� �� ��������� (0, 1, 2)
			if (nCircle == 0)
				current_point = zVEC3(x, y, 0);
			else if (nCircle == 1)
				current_point = zVEC3(x, 0, y);
			else if (nCircle == 2)
				current_point = zVEC3(0, x, y);

			// ��������� ����� �� ���������� �� ��������� ������� � �������,
			// ���� � ������� ������� �������������, ���� ��������� ������ ����������
			current_point = trafo ? (*trafo * current_point) : (sphere.center + current_point);


			// �� ������� ������� - ������ ���������� �����,
			// � ��������� ������� - ��������� ����� �� ����������
			if (angle > 0)
				// ��������� ��� ����� �� ����������
				zlineCache->Line3D(prev_point, current_point, wireColor, bUseWireGeometryInTheWorld);


			// ���� �� ��� �� ���������� ������ ����
			if (angle < 360.0f)
			{
				// ���������� ������� �����
				prev_point = current_point;

				// ����������� ���� �� ������
				angle += angle_step;
			}
			else // �����, ���������� ����������
			{
				// ��������� � ���������
				nCircle++;

				// ���������� ����
				angle = 0;
			}
		} /* end of loop by circles */
	}




	// ������ ������ ����� �����, ���:
	// sphere - ��������� ����� (����� � � ������),
	// wireColor - ���� ����� �����
	// trafo - ��������� �� ������� ������������� ������� (��� ����� ��� ��������)
	void DrawWireSphere(zTBSphere3D& sphere, const zCOLOR& wireColor, zMAT4* trafo = NULL)
	{
		// ������ �����
		float R = sphere.radius;

		// ���������� � ������
		zVEC3 vPos = sphere.center;

		// ��������� ���������� ������� �����
		zVEC3 vCap = zVEC3(0, R, 0);

		// ���������� ����� �������� �����
		zVEC3 v1, v2, v3, v4;

		// �������� ����� � ������ ����������� � ���������� ����
		BOOL bUseWireGeometryInTheWorld = TRUE;

		// �������� ����� ��������� �� ������� �����
		float numSegments = GetNumSphereSegmentsAuto(R);

		// ��������� �����
		zVEC3 pt;

		// ���� ��������� ������ ���� ��������
		// (0 - ��� �� ���������, 1 - ��� ���������)
		BOOL LSegDrawed = 0;

		// �������� ���� �������������� �����
		float h_step = (2.0f * PI) / numSegments;

		// �������� ���� ������������ �����
		float v_step = h_step / 1.3f;

		// 2 ��
		float calc2PI = 2.0f * PI;

		// �������� ��
		float calcHalfPI = PI / 2.0f;

		// ������� ����� �� ������������ ����-������, ��� ���������� �������������� �����
		// (������ ����� ����� �����)
		for (float v = -calcHalfPI + v_step; v < calcHalfPI - v_step; v += v_step)
		{
			// ������� ��������� ����� �� ������������ ����������,
			// ����� ��������� �������������� ��� ��������� ������ �����
			float next_v = v + v_step;

			// ������� ����� �� �������������� ������ ������ �����
			for (float h = 0; h <= calc2PI; h += h_step)
			{
				// ������� ����������
				v2 = zVEC3(R * cos(v) * cos(h), R * sin(v), R * cos(v) * sin(h));

				// ��������� ����������
				v4 = zVEC3(R * cos(next_v) * cos(h), R * sin(next_v), R * cos(next_v) * sin(h));

				// ���� ����� �������� �����
				if (trafo)
				{
					// ���������� ������� �������
					// ��� ���������� ���������
					v2 = *trafo * v2;
					v4 = *trafo * v4;
				}
				else
				{
					// �����, ���������� ������ �����������
					v2 = vPos + v2;
					v4 = vPos + v4;
				}


				// ������ ������� �����, ��������� ��������� �����: v1, v2, v3, v4
				// ������ �������� ����������, ������ ��� ���������� ����� v1 � v3 - ����������,
				// ��� ������������� ����, ����� ����� ��������� ����� � ����� �������� ������ � ��������� ��������
				if (h > 0)
				{
					// ������� �����
					zlineCache->Line3D(v1, v2, wireColor, bUseWireGeometryInTheWorld);

					// ������ �����
					zlineCache->Line3D(v3, v4, wireColor, bUseWireGeometryInTheWorld);

					// ���� ����� ����� �������� ��� �� ����������
					if (LSegDrawed == 0)
					{
						// ������ �
						zlineCache->Line3D(v1, v3, wireColor, bUseWireGeometryInTheWorld);

						// ������ ����, ������� ��������� �������� ����� � ����� �������
						// ��������� ��������� � ���� �������������� ����
						// ����� ������� ������ ������� �������� ���������, � ��������� - ��������,
						// ����� �� ���� ������ �����
						LSegDrawed = 1;
					}

					// ������ �����
					zlineCache->Line3D(v2, v4, wireColor, bUseWireGeometryInTheWorld);
				}


				/******************************************************/
				// ����������� ��������� ������ � ������� ����� �����
				/******************************************************/
				// ���� ��� ������ ��� ��������� ��������� �����
				if ((v + calcHalfPI - v_step) < 0.01f)
				{
					// ���� ����� ��������� �������� �����, ��������� ���, �����, ���������� ������� �����������
					pt = trafo ? (*trafo * (-vCap)) : vPos - vCap;

					// ������ ������ �����, ���� ���������� ���������� ����� � ������ ����� �����
					zlineCache->Line3D(pt, v2, wireColor, bUseWireGeometryInTheWorld);
				}
				// ��� ���� ��� ��������� ��� ��������� ���������
				else if ((v + v_step) >= (calcHalfPI - v_step))
				{
					// ���� ����� ��������� �������� �����,
					// ��������� ���, ��������� ������� ������������� �������,
					// �����, ���������� ������� �����������
					pt = trafo ? (*trafo * vCap) : vPos + vCap;

					// ������ ������� �����, ���� ���������� ���������� ����� � ������� ����� �����
					zlineCache->Line3D(pt, v4, wireColor, bUseWireGeometryInTheWorld);
				}



				// ���������� ����� �� ������� ����������,
				// ��� ������������� � ��������� �������� �����
				v1 = v2;

				// ���������� ����� �� ��������� ����������,
				// ��� ������������� � ��������� �������� �����
				v3 = v4;


				// ���� �������� �� �������������� � ������������ ������� ����� �������� �����
				if (((h + h_step) > calc2PI) && ((v + v_step) >= (calcHalfPI - v_step)))
				{
					// ������� ���������� ������� �����
					pt = zVEC3(R * cos(v + v_step) * cos(0.0f), R * sin(v + v_step), R * cos(v + v_step) * sin(0.0f));

					// � ���� ����� ��������� �������� �����, ��������� ���,
					// �����, ���������� ������� �����������
					pt = trafo ? (*trafo * pt) : vPos + pt;

					// ��������� ����� ���������� ��������������� ����� (������)
					zlineCache->Line3D(v4, pt, wireColor, bUseWireGeometryInTheWorld);
				}
			}


			// ����� ����, ��� ���������� �������������� ��� ���������,
			// ���������� ���� ��������� ������ ���� ��������,
			// ����� ��� ����� ���� ����� ��������
			LSegDrawed = 0;


			// ��������� ������� ������� ��������������� �����:
			// ������� ���������� ������� �����
			pt = zVEC3(R * cos(v) * cos(0.0f), R * sin(v), R * cos(v) * sin(0.0f));

			// � ���� ����� ��������� �������� �����, ��������� ���,
			// �����, ���������� ������� �����������
			pt = trafo ? (*trafo * pt) : vPos + pt;

			// �������� �������������� ������
			zlineCache->Line3D(v2, pt, wireColor, bUseWireGeometryInTheWorld);
		}
	}



	



	// � ����� ��������� ���������� �����
	void DrawLightSphere_Loop()
	{
		// ���� ��������� ����� ���������
		if (theApp.lightSphereSettings.sphereDrawMode == eSphereDrawMode::DRAW_NONE)
			// �������
			return;

		// �����, �������� ���������� ������ � �������� ������������� ��� � �������� �����
		zCVobLight* pLight = dynamic_cast<zCVobLight*>(SPC_GetSelectedObject());

		// ���� ��� �� "����"
		if (!pLight)
			// �������
			return;

		// �����, ��������� �������� ������
		// �� ���� ��������� �����:
		// ������ ��������� � ��� ���������� � ����
		zTBSphere3D s;
		s.radius = pLight->lightData.range;
		s.center = pLight->GetPositionWorld();

		// ������� ������������� ��� ����� �����;
		// ���� ����, ���� � ���, � ����������� �� ����� "bUseSphereTrafo";
		// ���� ����, �� ����� ����� ����� ����� ��������� ������ � ����������;
		zMAT4* sphere_trafo = theApp.lightSphereSettings.bUseSphereTrafo ? &pLight->trafoObjToWorld : NULL;


		/**********************************************/
		//  ����� ��������� (������ ��� ������ �����)
		/**********************************************/
		// � ������ ����������� �����
		if (theApp.lightSphereSettings.sphereDrawMode == eSphereDrawMode::DRAW_ONLY_ORBITS)
			// ������ ������ ������
			DrawSphereOrbits(s, theApp.lightSphereSettings.sphereWireColor, sphere_trafo);
		else // � ������ ����������� ������ ����� �����,
			// ������ ��� ����� �����
			DrawWireSphere(s, theApp.lightSphereSettings.sphereWireColor, sphere_trafo);


		/***************************/
		//  ������������ ��� �����
		/***************************/
		// ��������� ��������� ������ �� ������ �� ������� �����
		zVEC3 pt1(0, s.radius, 0);

		// � ����� �� ������, ������ �� ��� �����
		zVEC3 pt2 = -pt1;

		// ���� �������� ����� �� �����
		if (!sphere_trafo)
		{
			// ����������� ���������� �����
			// �� ��������� � ������� ������ � ������� �����������
			pt1 += s.center;
			pt2 += s.center;
		}
		else
		{
			// �����, ����������� ���������� � ������� ������� �������������,
			// ����������� � �������� ����
			pt1 = *sphere_trafo * pt1;
			pt2 = *sphere_trafo * pt2;
		}

		// ������ �����, ����������� ������� � ��� �����
		zlineCache->Line3D(pt1, pt2, theApp.lightSphereSettings.sphereWireColor, 1);


		/***************************/
		//  �������� ������� �����
		/***************************/
		// ���� ����� ������� ������ �����
		if (theApp.lightSphereSettings.bDrawRadiusValue == TRUE)
		{
			// ��������� ������ �������, � ������ ������� ������ ������ "radiusTextFmt"
			zSTRING txt = string::Combine(theApp.lightSphereSettings.radiusTextFmt.ToChar(), string(s.radius, 0));

			int x, y;
			// �������� �������� ��������� ��������� �� ������
			GetProjection(x, y, s.center);

			// ���� �������� � �������� ��������� ������
			if (!(x < 0 || x > 8192 || y < 0 || y > 8192 || (x == 0 && y == 0)))
			{
				// �������� ������ ������ (� ����. �����������)
				int txt_vsizex = screen->FontSize(txt);

				// ������� �������� ������� �� �����
				screen->Print(x - txt_vsizex / 2, y + zPixelY(32 / 2 + 7), txt);
			}
		}
	}
}