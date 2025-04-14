// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void ErrorReport::CheckPolygons()
	{
		zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
		zCArray<zCMaterial*> mats;
		zCArray<zCMaterial*> matsChecked;

		zCArray<zSTRING> matsNames;

		int numPolys = ogame->GetWorld()->bspTree.numPolys;

		for (int i = 0; i < numPolys; i++)
		{
			zCPolygon* poly = trisList[i];
			if (poly->flags.ghostOccluder != 0) continue;

			auto mat = poly->material;

			if (mat && mat->texture)
			{
				if (!matsChecked.IsInList(mat))
				{
					matsChecked.InsertEnd(mat);

					if (!matsNames.IsInList(mat->GetName()))
					{
						matsNames.InsertEnd(mat->GetName());
					}
					else
					{
						//cmd << "MatCollision: " << mat->GetName() << endl;
					}

					auto searchName = mat->texture->GetObjectName();

					zSTRING  originalName = searchName.Cut(searchName.Length() - 4, 4);

					originalName += "-C.TEX";

					//cmd << originalName << endl;

					bool foundVirtual = false;

					auto result = vdf_fexists(originalName.ToChar(), VDF_VIRTUAL);

					if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
					{
						foundVirtual = true;
					}


					result = vdf_fexists(originalName.ToChar(), VDF_PHYSICAL);

					if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
					{
						if (foundVirtual)
						{

						}
						else
						{

						}
					}


					if (result == 0 && !foundVirtual)
					{
						if (!mats.IsInList(mat))
						{
							mats.InsertEnd(mat);

							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_TEXTURE_NOT_FOUND);
							entry->SetObject((uint)mat);
							entry->SetVobName(mat->GetName());
							entry->SetMaterialName(mat->GetName());
							entry->SetTextureName(mat->texture->GetObjectName());

							AddEntry(entry);

						}
					}

					if (mat->GetObjectName().Contains(".0"))
					{
						if (!mats.IsInList(mat))
						{
							mats.InsertEnd(mat);

							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_INFO);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_NAME);
							entry->SetObject((uint)mat);
							entry->SetVobName(mat->GetName());
							entry->SetMaterialName(mat->GetName());
							entry->SetTextureName(mat->texture->GetObjectName());

							AddEntry(entry);
						}
					}

					if (mat->texture->GetObjectName().Contains(".TGA.TGA") || mat->texture->GetObjectName().Contains(".TGA.0"))
					{
						mats.InsertEnd(mat);

						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_TEXTURE_BAD_NAME);
						entry->SetObject((uint)mat);
						entry->SetVobName(mat->GetName());
						entry->SetMaterialName(mat->GetName());
						entry->SetTextureName(mat->texture->GetObjectName());

						AddEntry(entry);
					}


				}

			}
		}

		return;

		// check matlist
		zCClassDef* classDef = zCMaterial::classDef;
		zCMaterial* mat = 0;
		int numOfMats = classDef->objectList.GetNum();


		zCArray<zCMaterial*> pList;
		std::unordered_set<std::string> noCatalogVobsNames;


		auto matNew = new zCMaterial();
		matNew->SetName("NW_NATURE_WALDBODEN_03");
		matNew->SetTexture(Z "NW_Nature_Waldboden_02.TGA");
		matNew->AddRef();

		for (int i = 0; i < numOfMats; i++)
		{
			mat = dynamic_cast<zCMaterial*>(classDef->objectList[i]);

			if (mat /*&& mat->matUsage == zCMaterial::zMAT_USAGE_LEVEL*/)
			{
				if (mat->GetName().Length() > 0)
				{
					char* name = mat->GetName().ToChar();

					//cmd << "NameMat: " << name << endl;

					if (noCatalogVobsNames.find(name) == noCatalogVobsNames.end())
					{
						noCatalogVobsNames.insert(name);
					}
					else
					{
						..cmd << "#1" << endl;
						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MAT_NOT_UNIQ_MAT);
						entry->SetObject((uint)mat);
						entry->SetVobName(mat->GetName());
						entry->SetMaterialName(mat->GetName());
						entry->SetTextureName(mat->texture->GetObjectName());

						AddEntry(entry);
					}
				}
				else
				{

				}
				
			}
		}
	}

}