// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .


	void MatManager::UV_GatherPolygons()
	{
		// if we already gathered poly, do nothing, only location RESET can empty this array
		if (uvStruct.allPolys.GetNum() > 0)
		{
			return;
		}

		zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
		int numPolys = ogame->GetWorld()->bspTree.numPolys;

		std::unordered_set<zCPolygon*> hashSet;

		uvStruct.allPolys.DeleteList();


		for (int i = 0; i < numPolys; i++)
		{
			zCPolygon* poly = trisList[i];

			if (!poly) continue;

			if (poly->flags.portalPoly != 0) continue;
			if (poly->flags.ghostOccluder != 0) continue;

			auto mat = poly->material;

			if (!mat) continue;

			if (mat->matGroup == zTMat_Group::zMAT_GROUP_WATER)
			{
				continue;
			}

			if (mat->noCollDet && uvStruct.ignoreNoColl)
			{
				continue;
			}

			if (mat->GetName().Contains("SUN_BLOCKER"))
			{
				continue;
			}

			if (mat->texture)
			{
				zSTRING name = mat->texture->GetObjectName();

				if (name == "NW_MISC_FULLALPHA_01.TGA" || name == "MODIALPHA01.TGA" || name == "AB_ALPHA.TGA")
				{
					continue;
				}
			}

			// polys in original list can have copies, so we must check them
			if (hashSet.find(poly) == hashSet.end())
			{
				hashSet.insert(poly);

				if (poly->polyNumVert == 3)
				{
					uvStruct.allPolys.InsertEnd(poly);
				}
			}

		}
	}

	void MatManager::UV_FindErrors_ClearAll()
	{
		uvStruct.badPolys.DeleteList();
		uvStruct.allPolys.DeleteList();
	}


	// ��������� ��� ���� ������������ (� ��������) � ��������� ��
	void ComputeAndSortAngles(const zVEC3& a, const zVEC3& b, const zVEC3& c, float angles[3]) {
		// ��� 3D �������������
		const zVEC3 ab = b - a;
		const zVEC3 ac = c - a;
		const zVEC3 bc = c - b;

		const float len_ab = ab.Length();
		const float len_ac = ac.Length();
		const float len_bc = bc.Length();

		// ���� ��� ������� A
		angles[0] = std::acos(ab.Dot(ac) / (len_ab * len_ac));

		// ���� ��� ������� B
		angles[1] = std::acos((-ab).Dot(bc) / (len_ab * len_bc));

		// ���� ��� ������� C
		angles[2] = std::acos(ac.Dot(bc) / (len_ac * len_bc));

		//std::sort(angles, angles + 3);
	}

	// ���������� ���� UV-������������ (2D ����������)
	void ComputeAndSortUVAngles(const zVEC3& uv_a, const zVEC3& uv_b, const zVEC3& uv_c, float angles[3]) {
		// ������� � 2D (���������� ������ u � v)
		const float dx1 = uv_b[0] - uv_a[0];
		const float dy1 = uv_b[1] - uv_a[1];
		const float dx2 = uv_c[0] - uv_a[0];
		const float dy2 = uv_c[1] - uv_a[1];
		const float dx3 = uv_c[0] - uv_b[0];
		const float dy3 = uv_c[1] - uv_b[1];

		// ����� ������
		const float len1 = std::hypot(dx1, dy1);
		const float len2 = std::hypot(dx2, dy2);
		const float len3 = std::hypot(dx3, dy3);

		// ���� ����� ������� ���������
		angles[0] = std::acos((len1 * len1 + len2 * len2 - len3 * len3) / (2 * len1 * len2));
		angles[1] = std::acos((len1 * len1 + len3 * len3 - len2 * len2) / (2 * len1 * len3));
		angles[2] = std::acos((len2 * len2 + len3 * len3 - len1 * len1) / (2 * len2 * len3));

		//std::sort(angles, angles + 3);
	}

	bool CompareTriangleAngles(const zVEC3& v0, const zVEC3& v1, const zVEC3& v2,
		const zVEC3& uv0, const zVEC3& uv1, const zVEC3& uv2,
		float max_angle_diff = 0.1745f /* ~10 �������� */)
	{
		float angles3d[3], anglesUV[3];

		// ��������� � ��������� ����
		ComputeAndSortAngles(v0, v1, v2, angles3d);
		ComputeAndSortUVAngles(uv0, uv1, uv2, anglesUV);

		// ���������� ��������������� ����
		for (int i = 0; i < 3; ++i) {
			if (std::fabs(angles3d[i] - anglesUV[i]) > max_angle_diff) {

				//cmd << (std::fabs(angles3d[i] - anglesUV[i])) * DEGREE_LONG << endl;

				return false; // ������� ������� �����������
			}
		}
		return true;
	}

	bool CompareAreas(const zVEC3& v0, const zVEC3& v1, const zVEC3& v2, const zVEC3& uv0, const zVEC3& uv1, const zVEC3& uv2, float& a, float& b, bool& sim)
	{
		const zVEC3 edge3d1 = v1 - v0;
		const zVEC3 edge3d2 = v2 - v0;
		const zVEC3 edge3d3 = v2 - v1;

		const float area3d = 0.5f * edge3d1.Cross(edge3d2).Length();


		const float u1 = uv1[0] - uv0[0], v1_uv = uv1[1] - uv0[1];
		const float u2 = uv2[0] - uv0[0], v2_uv = uv2[1] - uv0[1];


		const float areaUV = 0.5f * std::fabs(u1 * v2_uv - v1_uv * u2);

		//zSTRING result = zSTRING(area3d, 20) + " / " + zSTRING(areaUV, 20);

		//pText->Print(200, 3000, "Areas: " + result);

		//cmd << area3d << " / " << areaUV << endl;

		// if UV is too small relative to total area, or it is too big relative to total area
		if (areaUV < 0.001 || areaUV >= 5)
		{
			return false;
		}

		// length of UV edges
		float sidesUV[3] = {
			(uv1 - uv0).Length(),
			(uv2 - uv0).Length(),
			(uv2 - uv1).Length()
		};

		// length of 3d triangle edges

		float sides3d[3] = {
			edge3d1.Length(),
			edge3d2.Length(),
			edge3d3.Length()
		};


		// ��������� ������� �� �����������
		std::sort(sides3d, sides3d + 3);

		// ��������� ������� �� �����������
		std::sort(sidesUV, sidesUV + 3);

		// �������� �� ����������� ����������� �� ������������ �������
		if (sides3d[2] < 1e-9f) return false;
		if (sidesUV[2] < 1e-9f) return false;


		// ����������� ������� ������������ ����������
		auto sidesRatio3d = zVEC3(
			sides3d[0] / sides3d[2],
			sides3d[1] / sides3d[2],
			sides3d[2] / sides3d[2] // ������ 1.0
		);

		// ����������� ������� ������������ ����������
		auto sidesRatioUV = zVEC3(
			sidesUV[0] / sidesUV[2],
			sidesUV[1] / sidesUV[2],
			sidesUV[2] / sidesUV[2] // ������ 1.0
		);

		const float threshold = 0.5f;

		auto aRatio = sidesRatio3d[0];
		auto bRatio = sidesRatio3d[1];

		a = aRatio;
		b = bRatio;

		//bool similiar = std::abs(aRatio) >= threshold && std::abs(bRatio) >= threshold;

		//sim = similiar;

		//if (!similiar)
		//{
		//	//cmd << "Found !similiar: a = " << a << " b = " << b << endl;
		//	return false;
		//}


		// 0.5 / 100
		// 0.3 / 80
		// 0.1 / 25


		/*float ratioAreaSideThreshold = 0.0003f;

		if (ratioEdge1 <= ratioAreaSideThreshold || ratioEdge2 <= ratioAreaSideThreshold || ratioEdge3 <= ratioAreaSideThreshold)
		{
			cmd << "ratioAreaSideThreshold: " << ratioAreaSideThreshold << endl;
			return false;
		}*/



		return true;
	}

	void MatManager::UV_FindBadPolys()
	{
		uvStruct.badPolys.DeleteList();

		for (int i = 0; i < uvStruct.allPolys.GetNumInList(); i++)
		{
			if (auto poly = uvStruct.allPolys.GetSafe(i))
			{
				zVEC3 uv0 = zVEC3(poly->feature[0]->texu, poly->feature[0]->texv, 0);
				zVEC3 uv1 = zVEC3(poly->feature[1]->texu, poly->feature[1]->texv, 0);
				zVEC3 uv2 = zVEC3(poly->feature[2]->texu, poly->feature[2]->texv, 0);

				auto resultCheckAngles = CompareTriangleAngles(poly->vertex[0]->position, poly->vertex[1]->position, poly->vertex[2]->position,
					uv0, uv1, uv2, Radian(uvStruct.distAngle));

				if (!resultCheckAngles)
				{
					uvStruct.badPolys.InsertEnd(poly);
				}
				else
				{
					float a, b;
					bool sim = false;

					auto resultArea = CompareAreas(poly->vertex[0]->position, poly->vertex[1]->position, poly->vertex[2]->position,
						uv0, uv1, uv2, a, b, sim);

					if (!resultArea)
					{
						uvStruct.badPolys.InsertEnd(poly);
					}
				}
			}
		}
	}


	void MatManager::UV_FindErrors()
	{
		UV_GatherPolygons();

		cmd << "AllPolys: " << uvStruct.allPolys.GetNumInList() << endl;

		cmd << uvStruct.minArea << "/" << uvStruct.maxArea << "/" << uvStruct.distAngle << "/" << uvStruct.ignoreNoColl << endl;


		UV_FindBadPolys();

		cmd << "Potentially BadPolys found: " << uvStruct.badPolys.GetNumInList() << endl;
	}

	void MatManager::UV_PrintPolyData(zCPolygon* poly)
	{
		zVEC3 pos0 = poly->vertex[0]->position;
		zVEC3 pos1 = poly->vertex[1]->position;
		zVEC3 pos2 = poly->vertex[2]->position;


		zlineCache->Line3D(pos0, pos1, GFX_RED, 0);
		zlineCache->Line3D(pos1, pos2, GFX_RED, 0);
		zlineCache->Line3D(pos2, pos0, GFX_RED, 0);


		if (!pUVShowView)
		{
			pUVShowView = standardView();
			screen->InsertItem(pUVShowView);
		}


		float a, b;
		bool sim = false;

		zVEC3 uv0 = zVEC3(poly->feature[0]->texu, poly->feature[0]->texv, 0);
		zVEC3 uv1 = zVEC3(poly->feature[1]->texu, poly->feature[1]->texv, 0);
		zVEC3 uv2 = zVEC3(poly->feature[2]->texu, poly->feature[2]->texv, 0);

		auto resultArea = CompareAreas(poly->vertex[0]->position, poly->vertex[1]->position, poly->vertex[2]->position,
			uv0, uv1, uv2, a, b, sim);

		zSTRING infoTriangle = zSTRING(a, 20) + " / " + zSTRING(b, 20) + " Sim: " + Z sim;

		pUVShowView->ClrPrintwin();
		pUVShowView->Print(200, 3000, infoTriangle);
	}

	void MatManager::UV_Loop()
	{
		for (int i = 0; i < uvStruct.badPolys.GetNumInList(); i++)
		{
			if (auto poly = uvStruct.badPolys.GetSafe(i))
			{
				if (poly->GetCenter().Distance(ogame->GetCameraVob()->GetPositionWorld()) <= uvStruct.radiusShowPolys)
				{
					
					zVEC3 pos0 = poly->vertex[0]->position;
					zVEC3 pos1 = poly->vertex[1]->position;
					zVEC3 pos2 = poly->vertex[2]->position;


					zlineCache->Line3D(pos0, pos1, GFX_RED, 0);
					zlineCache->Line3D(pos1, pos2, GFX_RED, 0);
					zlineCache->Line3D(pos2, pos0, GFX_RED, 0);
					//poly->DrawWire(GFX_RED);
					//theApp.debug.AddLine(poly->GetCenter(), poly->GetCenter() + poly->GetNormal() * 500, GFX_RED, 16);
				}
			}
		}
	}
}