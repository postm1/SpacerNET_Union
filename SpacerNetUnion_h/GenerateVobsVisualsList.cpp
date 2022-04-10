// Supported with union (c) 2020 Union team
// Union SOURCE file

#include <iostream>
#include <iomanip>

namespace GOTHIC_ENGINE {
	// Add your code here . . .

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

		VisualReportEntry::VisualReportEntry()
		{
			amount = 0;
			vdf = false;
			work = false;
			workOnly = false;
			notFound = false;
		}
	};
	void CreateHtmlReport(CString path);

	Common::Map<CString, VisualReportEntry*> searchVisualUniqList;



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

					searchVisualUniqList.Insert(key, entry);
				}
			}
		}


		CreateHtmlReport(path);
		/*

		// Сохраняем список в файл
		std::ofstream outfile;

		


		
		outfile.open(path, std::ios_base::trunc);
		outfile << "<=========================== There is a list of all vobs' visuals in the location (" << ogame->GetGameWorld()->GetWorldName() << ".ZEN) ===========================>\n\n";
		outfile << "==========================================================================================================================================================================\n";

		const char separator = ' ';
		const int visualLen = 70;
		const int amountLen = 10;
		const int typeLen = 16;
		const int archiveLen = 40;
		const int formatLen = 12;

		outfile << std::left << std::setw(visualLen) << std::setfill(separator) << "VisualName";
		outfile << std::left << std::setw(amountLen) << std::setfill(separator) << "Amount";
		outfile << std::left << std::setw(typeLen) << std::setfill(separator) << "Type";
		//outfile << std::left << std::setw(6) << std::setfill(separator) << result;
		outfile << std::left << std::setw(archiveLen) << std::setfill(separator) << "VDF archive of this visual";
		outfile << std::left << std::setw(formatLen) << std::setfill(separator) << "File type";
		outfile << "\n";

		outfile << "==========================================================================================================================================================================\n";


		auto arr = searchVisualUniqList.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{

				CString visualName = pair->GetKey().Upper();
				string volumeName = "";
				CString originVisualName = visualName;
				CString formatStr = "";




				if (originVisualName.EndWith(".TGA"))
				{
					formatStr = "TEXTURE";
					visualName = visualName.Replace(".TGA", "");
					visualName += "-C";
					visualName += ".TEX";
				}
				else if (originVisualName.EndWith(".ASC"))
				{
					formatStr = "ANIM";
					visualName = visualName.Replace(".ASC", ".MDL");
				}
				else if (originVisualName.EndWith(".MDS"))
				{
					formatStr = "ANIM";
					visualName = visualName.Replace(".MDS", ".MSB");
				}
				else if (originVisualName.EndWith(".MMS"))
				{
					formatStr = "ANIM";
					visualName = visualName.Replace(".MMS", ".MMB");
				}
				else
				{
					formatStr = "3DS";
					visualName = visualName.Replace(".3DS", ".MRM");
				}





				char* volumeNamePtr;
				long length = vdf_getvolumename(visualName.ToChar(), volumeNamePtr);
				string volumeNameVdf = volumeNamePtr;


				if (volumeNamePtr)
				{
					delete volumeNamePtr;
				}


				bool foundVirtual = false;

				auto result = vdf_fexists(visualName.ToChar(), VDF_VIRTUAL);

				if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
				{
					volumeName = "VDF";
					foundVirtual = true;
				}


				result = vdf_fexists(visualName.ToChar(), VDF_PHYSICAL);

				if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
				{
					if (foundVirtual)
					{
						volumeName += " & _WORK";
					}
					else
					{
						volumeName = "_WORK";
					}
				}


				if (result == 0 && !foundVirtual)
				{
					volumeName = "NOT FOUND!";
				}


				outfile << std::left << std::setw(visualLen) << std::setfill(separator) << originVisualName;
				outfile << std::left << std::setw(amountLen) << std::setfill(separator) << pair->GetValue();
				outfile << std::left << std::setw(typeLen) << std::setfill(separator) << volumeName;
				//outfile << std::left << std::setw(6) << std::setfill(separator) << result;
				outfile << std::left << std::setw(archiveLen) << std::setfill(separator) << volumeNameVdf;
				outfile << std::left << std::setw(formatLen) << std::setfill(separator) << formatStr;
				outfile << "\n";


			}

		}


		searchVisualUniqList.Clear();

		outfile.close();
		*/
	}

	void CreateHtmlReport(CString path)
	{

		const CString header = "<!DOCTYPE html><html><head><title>Vobs visuals report</title><style type=\"text/css\" media=\"screen\">html,body{font-family:serif;,sans-serif;\
color:#222222;}p{font-size:20px}table{border-collapse:collapse;border:1px solid grey;font-size:14px;}th{font-size:bold;background-color:#E1E15D;}td,th{border:1px solid grey;padding:5px}\
tr.warning{background-color:#e17a42}tr.error{background-color:red}</style></head><body><p><b>There is a list of all vobs' visuals in the location.</b></p>";

		const CString endFile = "</body></html>";

		// Сохраняем список в файл
		std::ofstream outfile;
		outfile.open(path, std::ios_base::trunc);


		outfile << header;


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

						outfile << "<p>Warning visuals table</p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";
					}

					outfile << "<tr class=\"error\">";
				}
				else if (pair->GetValue()->workOnly)
				{
					if (!foundAnyBadEntry)
					{
						foundAnyBadEntry = true;

						outfile << "<p>Warning visuals table</p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";
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
		



		//=====================================================================
		outfile << "<p>Normal visuals table</p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";

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

		searchVisualUniqList.Clear();

		outfile.close();
	}


	int SpacerApp::IsVisualInVDF(CString visualName, CString vdfName)
	{
		visualName = visualName.Upper();
		vdfName = vdfName.Upper();

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
}