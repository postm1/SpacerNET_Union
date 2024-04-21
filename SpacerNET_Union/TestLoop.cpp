// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .



	// test loop, some test code always here...
	void SpacerApp::TestLoop()
	{
		return;
		//test code for now
		if (KeyClick(KEY_F6))
		{
			zinput->ClearKeyBuffer();

			
			

			zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
			zCArray<zCMaterial*> mats;
			zCArray<zCMaterial*> matsChecked;
			zCArray<int> reasons;

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

						auto searchName = mat->texture->GetObjectName();

						bool foundVirtual = false;

						auto result = vdf_fexists(searchName.ToChar(), VDF_VIRTUAL);

						if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
						{

							foundVirtual = true;
						}


						result = vdf_fexists(searchName.ToChar(), VDF_PHYSICAL);

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
								reasons.InsertEnd(4);
							}
						}

						if (mat->GetObjectName().contains(".0"))
						{
							if (!mats.IsInList(mat))
							{
								mats.InsertEnd(mat);
								reasons.InsertEnd(1);
							}
						}

						if (mat->texture->GetObjectName().contains(".TGA.TGA"))
						{
							mats.InsertEnd(mat);
							reasons.InsertEnd(2);
						}

						if (mat->texture->GetObjectName().contains(".TGA.0"))
						{
							mats.InsertEnd(mat);
							reasons.InsertEnd(3);
						}

					}
					
				}
			}


			print.PrintRed("WarnMaterials: " + Z  mats.GetNumInList());

			for (int i = 0; i < mats.GetNumInList(); i++)
			{
				if (auto mat = mats.GetSafe(i))
				{
					if (reasons[i] > 1)
					{
						cmd << mat->GetName() + " " + mat->texture->GetObjectName() << " Reason: " << reasons[i] << " !!!!!!!!!!!!!!" << endl;
					}
					else
					{
						cmd << mat->GetName() + " " + mat->texture->GetObjectName() << " Reason: " << reasons[i] << endl;
					}
					


				}
			}

		}
		return;

		// return is always here
	}
	
}