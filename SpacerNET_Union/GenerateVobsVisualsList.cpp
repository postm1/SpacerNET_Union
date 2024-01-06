// Supported with union (c) 2020 Union team
// Union SOURCE file

#include <iostream>
#include <iomanip>

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	// generates HMTL report about used models in the current location
	struct VisualReportEntry
	{
		int amount;
		bool vdf;
		bool work;
		bool notFound;
		bool workOnly;
		CString vdfOrWork;
		CString vdfName;
		CString fileType;
		zCArray<CString> texturesNames;
		zCVob* pVob;

		VisualReportEntry::VisualReportEntry()
		{
			amount = 0;
			vdf = false;
			work = false;
			workOnly = false;
			notFound = false;
			pVob = false;
		}
	};

	struct VisualReportEntryBadTexture
	{
		CString name;
		int type;
		zCArray<CString> textures;
	};

	void CreateHtmlReport(CString path);

	Common::Map<CString, VisualReportEntry*> searchVisualUniqList;
	Common::Map<CString, VisualReportEntryBadTexture*> badTextures;

	void SpacerApp::FindVobsVisualsUnique(CString path)
	{

		zCArray<zCVob*> result;


		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, result, 0);


		int num = result.GetNumInList();

		for (int i = 0; i < num; i++)
		{
			auto pVob = result.GetSafe(i);

			if (pVob && pVob->visual)
			{
				if (pVob->IsPFX() || dynamic_cast<zCVobLevelCompo*>(pVob)) continue;

				CString key = pVob->visual->GetVisualName();

				

				auto& foundPair = searchVisualUniqList[key];

				if (!foundPair.IsNull())
				{
					foundPair.GetValue()->amount += 1;
				}
				else
				{
					auto entry = new VisualReportEntry();
					entry->amount = 1;
					
					entry->pVob = pVob;
					searchVisualUniqList.Insert(key, entry);
				}
			}
		}


		CreateHtmlReport(path);
	}

	void CreateHtmlReport(CString path)
	{

		const CString header = "<!DOCTYPE html><html><head><title>Vobs visuals report</title><style type=\"text/css\" media=\"screen\">html,body{font-family:serif;,sans-serif;\
color:#222222;}p{font-size:20px}table{border-collapse:collapse;border:1px solid grey;font-size:14px;}th{font-size:bold;background-color:#E1E15D;}td,th{border:1px solid grey;padding:5px}\
tr.warning{background-color:#e17a42}tr.error{background-color:red}.texture_word_orange{color:#FF9900;} .texture_word_red{color:#FF001E;}</style></head><body><p><b>There is a list of all vobs' visuals in the location.</b></p>";

		const CString endFile = "</body></html>";

		// Сохраняем список в файл
		std::ofstream outfile;
		outfile.open(path, std::ios_base::trunc);


		outfile << header;

		badTextures.Clear();

		auto arr = searchVisualUniqList.GetArray();

		bool foundAnyBadEntry = false;
		

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{
				CString originVisualName = pair->GetKey().Upper();
				CString searchName = originVisualName;


				// search name & filetype
				if (originVisualName.EndWith(".TGA"))
				{
					pair->GetValue()->fileType = "TEXTURE";
					searchName = searchName.Replace(".TGA", "");
					searchName += "-C";
					searchName += ".TEX";
				}
				else if (originVisualName.EndWith(".ASC"))
				{
					pair->GetValue()->fileType = "ANIM";
					searchName = searchName.Replace(".ASC", ".MDL");
				}
				else if (originVisualName.EndWith(".MDS"))
				{
					pair->GetValue()->fileType = "ANIM";
					searchName = searchName.Replace(".MDS", ".MSB");
				}
				else if (originVisualName.EndWith(".MMS"))
				{
					pair->GetValue()->fileType = "ANIM";
					searchName = searchName.Replace(".MMS", ".MMB");
				}
				else
				{
					pair->GetValue()->fileType = "3DS";
					searchName = searchName.Replace(".3DS", ".MRM");
				}

				// adding information about textures from submeshes
				if (pair->GetValue()->fileType == "3DS" && pair->GetValue()->pVob != NULL)
				{
					auto curVob = pair->GetValue()->pVob;

					if (auto visual = curVob->GetVisual())
					{
						if (auto mesh = visual->CastTo<zCProgMeshProto>())
						{
							//cmd << "mesh! " << curVob->GetVobName() << " visual: " << visual->GetVisualName() << endl;

							for (int i = 0; i < mesh->numSubMeshes; i++)
							{
								auto mat = mesh->subMeshList[i].material;

								if (mat && mat->texture)
								{
									//cmd << mat->texture->GetObjectName() << endl;
									pair->GetValue()->texturesNames.InsertEnd(mat->texture->GetObjectName());
								}
							}

							//cmd << "===========" << endl;
						}
					}
				}

				// VDF NAME
				char* volumeNamePtr;
				long length = vdf_getvolumename(searchName.ToChar(), volumeNamePtr);
				string volumeNameVdf = volumeNamePtr;

				pair->GetValue()->vdfName = volumeNameVdf;

				if (volumeNamePtr)
				{
					delete volumeNamePtr;
				}

				


				bool foundVirtual = false;

				auto result = vdf_fexists(searchName.ToChar(), VDF_VIRTUAL);

				if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
				{
					pair->GetValue()->vdfOrWork = "VDF";
					pair->GetValue()->vdf = true;
					foundVirtual = true;
				}


				result = vdf_fexists(searchName.ToChar(), VDF_PHYSICAL);

				if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
				{
					if (foundVirtual)
					{
						pair->GetValue()->vdfOrWork += " & _WORK";
						pair->GetValue()->work = true;
					}
					else
					{
						pair->GetValue()->vdfOrWork = "_WORK";
						pair->GetValue()->vdfName = "-";
						pair->GetValue()->workOnly = true;
					}
				}


				if (result == 0 && !foundVirtual)
				{
					pair->GetValue()->vdfOrWork = "NOT FOUND";
					pair->GetValue()->notFound = true;
					pair->GetValue()->vdfName = "-";
					//cmd << "NotFound: '" << pair->GetKey() << "'\n";
				}
			}
		}


		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{

				CString visualName = pair->GetKey().Upper();


				if (pair->GetValue()->notFound)
				{
					if (!foundAnyBadEntry)
					{
						foundAnyBadEntry = true;

						outfile << "<p><b>Warning visuals table</b></p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";
					}

					outfile << "<tr class=\"error\">";
				}
				else if (pair->GetValue()->workOnly)
				{
					if (!foundAnyBadEntry)
					{
						foundAnyBadEntry = true;

						outfile << "<p><b>Warning visuals table</b></p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";
					}

					outfile << "<tr class=\"warning\">";
				}
				else
				{
					continue;
				}


				


				outfile << "<td>'" << pair->GetKey().Upper() << "'</td>";
				outfile << "<td>" << pair->GetValue()->amount << "</td>";
				outfile << "<td>" << pair->GetValue()->vdfOrWork << "</td>";
				outfile << "<td>" << pair->GetValue()->vdfName << "</td>";
				outfile << "<td>" << pair->GetValue()->fileType << "</td>";
				

				
				
				outfile << "</tr>";
			}

		}

		if (foundAnyBadEntry)
		{
			outfile << "</table><br>";
		}

		outfile << "<p><b>Not found or _WORK textures</b></p><table id=\"table_bad_tex\"><tr><th>Visual name</th><th>Texture TEX</th><th>Texture TGA</th></tr>";

		//auto arrBad = badTextures.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);
			bool foundOk = false;

			if (!pair->IsNull() && pair->GetValue()->pVob)
			{
				
				for (int k = 0; k < pair->GetValue()->texturesNames.GetNumInList(); k++)
				{
					auto nameTexture = pair->GetValue()->texturesNames.GetSafe(k);
					auto originalName = nameTexture;

					nameTexture = nameTexture.Replace(".TGA", "");
					nameTexture += "-C";
					nameTexture += ".TEX";

					int fileTypeExist = Union_FileExists(nameTexture);

					//cmd << nameTexture << " Type: " << fileTypeExist << endl;

					if (fileTypeExist == 2)
					{
						//outfile << originalName << "<br>";
					}
					else if (fileTypeExist == 1)
					{
						if (!foundOk)
						{
							outfile << "<tr>";
							outfile << "<td>'" << pair->GetKey().Upper() << "'</td>";
						}

						


						outfile << "<td>";
						outfile << "<span class=\"texture_word_orange\">" << nameTexture << "</span>";
						outfile << "</td>";
						outfile << "<td>";
						outfile << "<span class=\"texture_word_orange\">" << originalName << "</span>";
						outfile << "</td>";
						outfile << "</tr>";
							
					}
					else
					{
						if (!foundOk)
						{
							outfile << "<tr>";
							outfile << "<td>'" << pair->GetKey().Upper() << "'</td>";
						}


						outfile << "<td>";
						outfile << "<span class=\"texture_word_red\">" << nameTexture << "</span>";
						outfile << "</td>";
						outfile << "<td>";
						outfile << "<span class=\"texture_word_red\">" << originalName << "</span>";
						outfile << "</td>";
						outfile << "</tr>";
					}



				}

				if (foundOk)
				{
					//outfile << "</td>";
					//outfile << "</tr>";
				}
				
			}

		}

		outfile << "</table><br>";
		



		//=====================================================================
		outfile << "<p><b>Normal visuals table</b></p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{

				CString visualName = pair->GetKey().Upper();


				if (!pair->GetValue()->notFound && !pair->GetValue()->workOnly)
				{
					outfile << "<tr>";
				}
				else
				{
					continue;
				}


				outfile << "<td>'" << pair->GetKey().Upper() << "'</td>";
				outfile << "<td>" << pair->GetValue()->amount << "</td>";
				outfile << "<td>" << pair->GetValue()->vdfOrWork << "</td>";
				outfile << "<td>" << pair->GetValue()->vdfName << "</td>";
				outfile << "<td>" << pair->GetValue()->fileType << "</td>";


				

				outfile << "</tr>";
			}

		}

		outfile << "</table><br>";


		outfile << endFile;

		DeleteAndClearMap(searchVisualUniqList);


		outfile.close();
	}


	int SpacerApp::IsVisualInVDF(CString visualName, CString vdfName)
	{
		visualName = visualName.Upper();
		vdfName = vdfName.Upper();

		if (vdfName == "") return 0;

		CString searchName = visualName;

		if (visualName.EndWith(".TGA"))
		{
			searchName += "-C";
			searchName += ".TEX";
		}
		else if (visualName.EndWith(".ASC"))
		{
			searchName = searchName.Replace(".ASC", ".MDL");
		}
		else if (visualName.EndWith(".MDS"))
		{
			searchName = searchName.Replace(".MDS", ".MSB");
		}
		else if (visualName.EndWith(".MMS"))
		{
			searchName = searchName.Replace(".MMS", ".MMB");
		}
		else
		{
			searchName = searchName.Replace(".3DS", ".MRM");
		}


		char* volumeNamePtr;
		long length = vdf_getvolumename(searchName.ToChar(), volumeNamePtr);
		string volumeNameVdf = volumeNamePtr;

		if (volumeNamePtr)
		{
			delete volumeNamePtr;
		}

		return (volumeNameVdf.Upper() == vdfName);
	}

	void SpacerApp::ToggleNoGrass()
	{
		nograss.hideActive = !nograss.hideActive;
		nograss.SetPercentFromSettings(true);
	}
}