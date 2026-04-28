// Supported with union (c) 2020 Union team
// Union SOURCE file

#include <iostream>
#include <iomanip>

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	struct VisualReportTextureInfo
	{
		CString textureNameTGA;
		CString textureNameTEX;

		CString textureSizeInfoStr;
		bool isBigTexture;
		zCMaterial* material;

		VisualReportTextureInfo::VisualReportTextureInfo()
		{
			isBigTexture = false;
			material = NULL;
		}

		void FillInfo(zCMaterial* mat)
		{
			if (!mat) return;

			material = mat;

			textureNameTGA = A mat->texture->GetObjectName();

			// TGA -> TEX renaming
			zSTRING temp = textureNameTGA;
			temp = temp.Replace(".TGA", "");
			temp += "-C";
			temp += ".TEX";
			textureNameTEX = A temp;
			
			textureSizeInfoStr = A GetTextureSizeInfo(mat, isBigTexture);
		}
	};

	enum VisualReportFileType
	{
		VDF_FILE_TYPE_NOT_FOUND = 0,
		VDF_FILE_TYPE_WORK = 1,
		VDF_FILE_TYPE_VDF = 2,
		VDF_FILE_TYPE_BOTH = 3,
	};

	// generates HMTL report about used models in the current location
	struct VisualReportEntry
	{
		int amount;
		VisualReportFileType foundType;
		CString vdfName;
		CString visualName;
		std::vector<VisualReportTextureInfo> texturesList;
		std::vector<zCMaterial*> materialsList;
		zCVob* pVob;
		int polygons;
		bool isLocationMesh;

		VisualReportEntry::VisualReportEntry()
		{
			amount = 0;
			polygons = 0;
			foundType = VDF_FILE_TYPE_NOT_FOUND;
			pVob = false;
			isLocationMesh = false;
			vdfName = "-";
		}

		zSTRING GetFileTypeFound()
		{
			switch (foundType)
			{
			case VDF_FILE_TYPE_VDF:       return "VDF";
			case VDF_FILE_TYPE_WORK:      return "_WORK";
			case VDF_FILE_TYPE_BOTH:      return "_WORK/VDF";
			default:                      return "NOT FOUND";
			}
		}


		zSTRING GetProblemType()
		{
			switch (foundType)
			{
			case VDF_FILE_TYPE_VDF:       return "-";
			case VDF_FILE_TYPE_WORK:      return "FILE ONLY IN _WORK";
			default:                      return "FILE NOT FOUND";
			}
		}
		
		void PrintData()
		{
			if (isLocationMesh)
			{
				cmd << "[MESH]: " << " -> " << GetFileTypeFound() << " | " << endl;

			
				for (auto& it : texturesList)
				{
					cmd << "\t" << it.material->GetName() << " -> " << it.textureNameTEX << " " << it.textureSizeInfoStr;
				}

				cmd << endl;
			}
			else
			{
				cmd << "[" << visualName << "] -> " << GetFileTypeFound() << " | Amount: " << amount << " | Polygons: " << polygons << endl;

				for (auto& it : texturesList)
				{
					cmd << "\t" << it.material->GetName() << " -> " << it.textureNameTEX << " | " << it.textureSizeInfoStr << endl;;
				}

				cmd << endl;
			}
			
		}
	};

	struct VisualReportEntryItem
	{
		zSTRING name;
		int count;
		bool inContainer = false;
		zVEC3 coords;
		zSTRING contName;
	};

	void CreateHtmlReport(CString path);
	void GetLocationMeshTexturesList();
	void GatherLocationUniqVisualsList();
	void ExtractVisualInfo(zCVisual* visual, VisualReportEntry& reportInfo);


	//=========================================================================
	std::vector<VisualReportEntry> pListReport;

	void SpacerApp::GenerateLocationReport(CString path)
	{

		pListReport.clear();
		pListReport.reserve(10000);

		// Gathers vobs unique visuals
		GatherLocationUniqVisualsList();

		// Gathers info from location mesh
		GetLocationMeshTexturesList();

		// Creates html report about everything
		CreateHtmlReport(path);
	}

	zSTRING GetRealFileName(zSTRING originVisualName)
	{
		zSTRING searchName = originVisualName;

		if (originVisualName.EndWith(".TGA"))
		{
			searchName = searchName.Replace(".TGA", "");
			searchName += "-C.TEX";
		}
		else if (originVisualName.EndWith(".ASC"))
		{
			searchName = searchName.Replace(".ASC", ".MDL");
		}
		else if (originVisualName.EndWith(".MDS"))
		{
			searchName = searchName.Replace(".MDS", ".MSB");
		}
		else if (originVisualName.EndWith(".MMS"))
		{
			searchName = searchName.Replace(".MMS", ".MMB");
		}
		else if (originVisualName.EndWith(".3DS"))
		{
			searchName = searchName.Replace(".3DS", ".MRM");
		}

		return searchName;
	}

	void GatherLocationUniqVisualsList()
	{
		static const zSTRING INVISIBLE_FILEPREFIX = "INVISIBLE_";
		zCArray<zCVob*> result;


		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, result, 0);

		std::unordered_map<std::string, VisualReportEntry> visualMap;

		//Gathering vobs by its visuals
		int num = result.GetNumInList();

		for (int i = 0; i < num; i++)
		{
			auto pVob = result.GetSafe(i);

			if (pVob && pVob->visual && !pVob->visual->GetObjectName().HasWord(INVISIBLE_FILEPREFIX))
			{
				if (pVob->IsPFX() || dynamic_cast<zCVobLevelCompo*>(pVob)) continue;

				std::string visualName = pVob->visual->GetVisualName().ToChar();
				CString checkNameEmpty = pVob->visual->GetVisualName();

				if (checkNameEmpty.Length() == 0 || checkNameEmpty == ' ' || checkNameEmpty.Shrink().Length() == 0)
				{
					continue;
				}


				auto it = visualMap.find(visualName);

				if (it != visualMap.end())
				{
					it->second.amount++;
					continue;
				}


				


				VisualReportEntry entry;


				entry.amount = 1;
				entry.pVob = pVob;
				entry.visualName = A pVob->visual->GetVisualName();

				zSTRING checkName = GetRealFileName(pVob->visual->GetVisualName());
				char* checkNameReal = checkName.ToChar();

				bool hasVirtual = (vdf_fexists(const_cast<char*>(checkNameReal), VDF_VIRTUAL) & VDF_VIRTUAL) == VDF_VIRTUAL;
				bool hasPhysical = (vdf_fexists(const_cast<char*>(checkNameReal), VDF_PHYSICAL) & VDF_PHYSICAL) == VDF_PHYSICAL;

				
				entry.foundType = VDF_FILE_TYPE_NOT_FOUND;

				if (hasVirtual)
				{
					entry.foundType = VDF_FILE_TYPE_VDF;

					char* volumeNamePtr = NULL;

					long length = vdf_getvolumename(checkNameReal, volumeNamePtr);

					if (volumeNamePtr && length > 0)
					{
						entry.vdfName = A volumeNamePtr;
						delete[] volumeNamePtr;
					}
				}

				if (hasPhysical)
					entry.foundType = (entry.foundType == VDF_FILE_TYPE_VDF) ? VDF_FILE_TYPE_BOTH : VDF_FILE_TYPE_WORK;


				if (pVob)
				{
					ExtractVisualInfo(pVob->GetVisual(), entry);
				}

				visualMap[visualName] = entry;
			}
		}


		for (auto& pair : visualMap)
		{
			pListReport.push_back(pair.second);
		}
	}

	void GetLocationMeshTexturesList()
	{
		zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
		int numPolys = ogame->GetWorld()->bspTree.numPolys;

		std::unordered_set<zCMaterial*> checkedMaterials;
		std::unordered_set<std::string> uniqueTextures;

		for (int i = 0; i < numPolys; i++)
		{
			zCPolygon* poly = trisList[i];
			if (poly->flags.ghostOccluder != 0) continue;

			zCMaterial* mat = poly->material;

			// Пропускаем если нет материала или текстуры
			if (!mat || !mat->texture) continue;

			// Проверяем не обрабатывали ли уже этот материал
			if (checkedMaterials.find(mat) != checkedMaterials.end())
				continue;

			checkedMaterials.insert(mat);

			// Получаем имя текстуры и формируем имя для проверки
			std::string texName = A mat->texture->GetObjectName();

			// Проверяем есть ли текстура вообще в uniqueTextures
			if (uniqueTextures.find(texName) != uniqueTextures.end())
				continue;

			uniqueTextures.insert(texName);

			// Формируем имя для проверки физического файла
			zSTRING checkName = GetRealFileName(mat->texture->GetObjectName()).ToChar();
			char* checkNameReal = checkName.ToChar();

			//cmd << "MeshCheckName: '" << checkName << "'" << endl;


			CString checkNameEmpty = checkName;

			if (checkNameEmpty.Length() == 0 || checkNameEmpty == ' ' || checkNameEmpty.Shrink().Length() == 0)
			{
				continue;
			}

			// Проверяем наличие текстуры
			bool hasVirtual = (vdf_fexists(const_cast<char*>(checkNameReal), VDF_VIRTUAL) & VDF_VIRTUAL) == VDF_VIRTUAL;
			bool hasPhysical = (vdf_fexists(const_cast<char*>(checkNameReal), VDF_PHYSICAL) & VDF_PHYSICAL) == VDF_PHYSICAL;

			VisualReportEntry entry;
			
			entry.isLocationMesh = true;
			entry.foundType = VDF_FILE_TYPE_NOT_FOUND;

			if (hasVirtual)
			{
				entry.foundType = VDF_FILE_TYPE_VDF;


				char* volumeNamePtr = NULL;

				long length = vdf_getvolumename(checkNameReal, volumeNamePtr);

				if (volumeNamePtr && length > 0)
				{
					entry.vdfName = A volumeNamePtr;
					delete[] volumeNamePtr;
				}
			
			}
				
			if (hasPhysical)
				entry.foundType = (entry.foundType == VDF_FILE_TYPE_VDF) ? VDF_FILE_TYPE_BOTH : VDF_FILE_TYPE_WORK;


			VisualReportTextureInfo texInfo;
			texInfo.FillInfo(mat);
			entry.texturesList.push_back(texInfo);
			entry.materialsList.push_back(mat);


				
			pListReport.push_back(entry);
		}


		for (auto& it : pListReport)
		{
			it.PrintData();
			
		}
	}

	void ExtractVisualInfo(zCVisual* visual, VisualReportEntry& reportInfo)
	{
		if (!visual)
		{
			return;
		}


		// TGA Decal
		if (auto pDecal = visual->CastTo<zCDecal>())
		{
			auto mat = pDecal->decalMaterial;

			if (mat && mat->texture)
			{
				VisualReportTextureInfo info;
				info.FillInfo(mat);

				reportInfo.texturesList.push_back(info);
				reportInfo.materialsList.push_back(mat);
				reportInfo.polygons = 2;
			}
		}

		// 3DS
		if (auto pProgMesh = visual->CastTo<zCProgMeshProto>())
		{
			//cmd << "mesh! " << curVob->GetVobName() << " visual: " << visual->GetVisualName() << endl;

			reportInfo.polygons += pProgMesh->GetNumTri();

			for (int i = 0; i < pProgMesh->numSubMeshes; i++)
			{

				auto mat = pProgMesh->subMeshList[i].material;

				if (mat && mat->texture)
				{
					//cmd << mat->texture->GetObjectName() << endl;
					VisualReportTextureInfo info;
					info.FillInfo(mat);

					reportInfo.texturesList.push_back(info);
					reportInfo.materialsList.push_back(mat);
				}
			}

			//cmd << "===========" << endl;
		}

		// MMS
		if (auto pMorph = visual->CastTo<zCMorphMesh>())
		{

			if (pMorph->morphMesh)
			{
				for (int i = 0; i < pMorph->morphMesh->numSubMeshes; i++)
				{


					auto mat = pMorph->morphMesh->subMeshList[i].material;

					if (mat && mat->texture)
					{
						VisualReportTextureInfo info;
						info.FillInfo(mat);

						reportInfo.texturesList.push_back(info);
						reportInfo.materialsList.push_back(mat);
					}
				}
			}

		}

		// ASC/MDS
		if (auto pModel = visual->CastTo<zCModel>())
		{
			for (int i = 0; i < pModel->meshSoftSkinList.GetNum(); i++)
			{
				for (int n = 0; n < pModel->meshSoftSkinList[i]->numSubMeshes; n++)
				{
					if (!pModel->meshSoftSkinList[i]) 
					{
						continue;
					}

					auto mat = pModel->meshSoftSkinList[i]->subMeshList[n].material;

					if (mat && mat->texture)
					{
						VisualReportTextureInfo info;
						info.FillInfo(mat);

						reportInfo.texturesList.push_back(info);
						reportInfo.materialsList.push_back(mat);
					}
					//cmd << pModel->meshSoftSkinList[i]->subMeshList[n].material->GetObjectName() << endl;
				}
			}

			// search in all the nodes
			for (int i = 0; i < pModel->nodeList.GetNum(); i++)
			{
				if (!pModel->nodeList[i])
				{
					continue;
				}

				// если нет визуала узла
				if (!pModel->nodeList[i]->nodeVisual)
					continue;

				ExtractVisualInfo(pModel->nodeList[i]->nodeVisual, reportInfo);
			}
		}
	}

	void CreateHtmlReport(CString path)
	{

		const CString header = "<!DOCTYPE html><html><head><title>Vobs visuals report</title>\
<style type=\"text/css\" media=\"screen\">\
body{font-family:'Segoe UI',Arial,sans-serif;background:#f5f5f5;color:#222222;padding:20px}\
h1{font-size:24px;margin:20px 0;color:#2c3e50}\
p{font-size:18px;margin:15px 0}\
.page-container{max-width:1920px;margin:0 auto;width:100%}\
table{width:100%;border-collapse:collapse;background:white;border:2px solid #444;margin:20px 0}\
th{background:#E1E15D;color:#222;font-weight:bold;padding:10px;border:1px solid #444;text-align:left}\
td{padding:8px;border:1px solid #444}\
#table_report tr:nth-child(even){background-color:#E4E4E4}\
#table_report tr:nth-child(odd){background-color:#ffffff}\
#table_report2 tr:nth-child(even){background-color:#E4E4E4}\
#table_report2 tr:nth-child(odd){background-color:#ffffff}\
tr.warning{background-color:#e17a42!important}\
tr.warning td{background-color:#e17a42;color:#222;border:1px solid #444}\
tr.error{background-color:#ff4444!important}\
tr.error td{background-color:#ff4444;color:#222;border:1px solid #444}\
.texture_word_orange{color:#FF6E00;font-weight:bold}\
.texture_word_red{color:#FF001E;font-weight:bold}\
td.high-poly{color:#FD7228;font-weight:bold}\
.report-time{background:#2897FF;padding:15px;border:2px solid #444444;margin:20px 0;color:white;font-size:22px;font-weight:bold;text-align:center}\
</style></head><body>";

		const CString endFile = "</div></body></html>";

		std::ofstream outfile;
		outfile.open(path, std::ios_base::trunc);


		outfile << header;

		outfile << "<div class=\"page-container\"><div class = \"report-time\">Report generated: " + GetTimeForReport() + "</div>";

		// WARNING VISUALS
		outfile << "<p><b>Warning visuals table</b></p><table id=\"table_report_warn\">\
<tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>_WORK/VDF</th><th>Problem type</th></tr>";


		for (auto& it : pListReport)
		{
			if (!it.isLocationMesh && it.foundType <= VDF_FILE_TYPE_WORK) // not found OR only in WORK
			{
				if (it.foundType == VDF_FILE_TYPE_NOT_FOUND)
				{
					outfile << "<tr class=\"error\">";
				}
				else
				{
					outfile << "<tr class=\"warning\">";
				}
		
				outfile << "<td>" << it.visualName.Upper() << "</td>";
				outfile << "<td>" << it.amount << "</td>";

				if (it.polygons >= 2000) 
				{
					outfile << "<td class='high-poly'>" << it.polygons << "</td>";
				}
				else {
					outfile << "<td>" << it.polygons << "</td>";
				}
				
				outfile << "<td>" << it.GetFileTypeFound() << "</td>";
				outfile << "<td>" << it.GetProblemType() << "</td>";

				outfile << "</tr>";
			}
		}

		outfile << "</table><br>";

		// WARNING MESH LOCATION
		outfile << "<p><b>Warning location mesh</b></p><table id=\"table_report_warn2\">\
<tr><th>Material name</th><th>Texture name TGA</th>><th>Texture name TEX</th><th>Size</th><th>_WORK/VDF</th><th>Problem type</th></tr>";


		for (auto& it : pListReport)
		{
			if (it.isLocationMesh && it.foundType <= VDF_FILE_TYPE_WORK) // not found OR only in WORK
			{
				if (it.foundType == VDF_FILE_TYPE_NOT_FOUND)
				{
					outfile << "<tr class=\"error\">";
				}
				else
				{
					outfile << "<tr class=\"warning\">";
				}

				outfile << "<td>" << it.materialsList[0]->GetObjectName() << "</td>";
				outfile << "<td>" << it.texturesList[0].textureNameTGA << "</td>";
				outfile << "<td>" << it.texturesList[0].textureNameTEX << "</td>";

				if (it.foundType == VDF_FILE_TYPE_NOT_FOUND)
				{
					outfile << "<td>-</td>";
				}
				else
				{
					outfile << "<td>" << it.texturesList[0].textureSizeInfoStr << "</td>";
				}

				outfile << "<td>" << it.GetFileTypeFound() << "</td>";
				outfile << "<td>" << it.GetProblemType() << "</td>";

				outfile << "</tr>";
			}
		}


		outfile << endFile;

		pListReport.clear();
		outfile.close();

	}

#ifdef SMELALALAL

	void CreateHtmlReport(CString path)
	{

		const CString header = "<!DOCTYPE html><html><head><title>Vobs visuals report</title>\
<style type=\"text/css\" media=\"screen\">\
body{font-family:'Segoe UI',Arial,sans-serif;background:#f5f5f5;color:#222222;padding:20px}\
h1{font-size:24px;margin:20px 0;color:#2c3e50}\
p{font-size:18px;margin:15px 0}\
.page-container{max-width:1920px;margin:0 auto;width:100%}\
table{width:100%;border-collapse:collapse;background:white;border:2px solid #444;margin:20px 0}\
th{background:#E1E15D;color:#222;font-weight:bold;padding:10px;border:1px solid #444;text-align:left}\
td{padding:8px;border:1px solid #444}\
#table_report tr:nth-child(even){background-color:#E4E4E4}\
#table_report tr:nth-child(odd){background-color:#ffffff}\
tr.warning{background-color:#e17a42!important}\
tr.warning td{background-color:#e17a42;color:#222;border:1px solid #444}\
tr.error{background-color:#ff4444!important}\
tr.error td{background-color:#ff4444;color:#222;border:1px solid #444}\
.texture_word_orange{color:#FF6E00;font-weight:bold}\
.texture_word_red{color:#FF001E;font-weight:bold}\
td.high-poly{color:#FD7228;font-weight:bold}\
.report-time{background:#2897FF;padding:15px;border:2px solid #444444;margin:20px 0;color:white;font-size:22px;font-weight:bold;text-align:center}\
</style></head><body>";

		const CString endFile = "</div></body></html>";

		std::ofstream outfile;
		outfile.open(path, std::ios_base::trunc);

		
		outfile << header;

		outfile << "<div class=\"page-container\"><div class = \"report-time\">Report generated: " + GetTimeForReport() + "</div>"; 

		outfile << "<body><p><b>There is a list of all vobs' visuals in the location.</b></p>";

		auto arr = searchVisualUniqList.GetArray();

		bool foundAnyBadEntry = false;
		
		


		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);

			if (!pair->IsNull())
			{
				CString originVisualName = pair->GetKey().Upper();
				CString searchName = originVisualName;
				CString checkName = originVisualName;

				if (checkName.Length() == 0 || checkName == ' ' || checkName.Shrink().Length() == 0)
				{
					pair->GetValue()->fileType = "-";
					pair->GetValue()->workOnly = true;
					pair->GetValue()->vdfName = "-";
					continue;
				}



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
				if (/*pair->GetValue()->fileType == "3DS" &&*/ pair->GetValue()->pVob != NULL)
				{
					auto curVob = pair->GetValue()->pVob;

					ExtractVisualInfo(curVob->GetVisual(), pair);
				}

				char* api_name_search = searchName.ToChar();

				//cmd << "api_name_search: '" << api_name_search << "'" << endl;

				auto resultVDF = vdf_fexists(api_name_search, VDF_VIRTUAL);
				auto resultWORK = vdf_fexists(api_name_search, VDF_PHYSICAL);

				bool foundVDF = (resultVDF & VDF_VIRTUAL) == VDF_VIRTUAL;
				bool foundWORK = (resultWORK & VDF_PHYSICAL) == VDF_PHYSICAL;

				

				char* volumeNamePtr = NULL;

				if (foundVDF)
				{
					long length = vdf_getvolumename(api_name_search, volumeNamePtr);

					if (volumeNamePtr && length > 0)
					{
						pair->GetValue()->vdfName = volumeNamePtr;
						delete[] volumeNamePtr;
					}
				}


				if (foundVDF && foundWORK)
				{
					pair->GetValue()->vdfOrWork = "_WORK/VDF";
					pair->GetValue()->vdf = true;
					pair->GetValue()->work = true;
					pair->GetValue()->workOnly = false;
					pair->GetValue()->notFound = false;
				}
				else if (foundVDF && !foundWORK) // VDF ONLY
				{
					pair->GetValue()->vdfOrWork = "VDF";
					pair->GetValue()->vdf = true;
					pair->GetValue()->work = false;
					pair->GetValue()->workOnly = false;
					pair->GetValue()->notFound = false;
				}
				else if (!foundVDF && foundWORK) // WORK ONLY
				{
					pair->GetValue()->vdfOrWork = "_WORK";
					pair->GetValue()->vdf = false;
					pair->GetValue()->work = true;
					pair->GetValue()->workOnly = true;
					pair->GetValue()->notFound = false;
					pair->GetValue()->vdfName = "-"; // 
				}
				else // !foundVDF && !foundWORK
				{
					pair->GetValue()->vdfOrWork = "NOT FOUND";
					pair->GetValue()->vdf = false;
					pair->GetValue()->work = false;
					pair->GetValue()->workOnly = false;
					pair->GetValue()->notFound = true;
					pair->GetValue()->vdfName = "-";
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

						outfile << "<p><b>Warning visuals table</b></p><table id=\"table_report_warn\"><tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";
					}

					outfile << "<tr class=\"error\">";
				}
				else if (pair->GetValue()->workOnly)
				{
					if (!foundAnyBadEntry)
					{
						foundAnyBadEntry = true;

						outfile << "<p><b>Warning visuals table</b></p><table id=\"table_report_warn\"><tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>_WORK/VDF</th><th>VDF name</th><th>File type</th></tr>";
					}

					outfile << "<tr class=\"warning\">";
				}
				else
				{
					continue;
				}


				


				outfile << "<td>'" << pair->GetKey().Upper() << "'</td>";
				outfile << "<td>" << pair->GetValue()->amount << "</td>";

				if (pair->GetValue()->polygons >= 2000) {
					outfile << "<td class='high-poly'>" << pair->GetValue()->polygons << "</td>";
				}
				else {
					outfile << "<td>" << pair->GetValue()->polygons << "</td>";
				}
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

		

		outfile << "<p><b>Not found or _WORK textures</b></p><table id=\"table_bad_tex\"><tr><th>Visual name</th><th>Texture TEX</th><th>Texture TGA</th><th>Type</th></tr>";

		auto meshTexturesProblems = GetLocationMeshTexturesList();

		//print.PrintGreen(Z (int)meshTexturesProblems.size());

		for (auto& entry : meshTexturesProblems)
		{
			auto originalName = entry;

			if (entry.size() >= 4 && entry.substr(entry.size() - 4) == ".TGA") {
				entry = entry.substr(0, entry.size() - 4);
			}

			zSTRING nameTexture = entry.c_str();
			


			//cmd << "Replace: " << nameTexture  << endl;

			nameTexture += "-C";
			nameTexture += ".TEX";

			int fileTypeExist = Union_FileExists(nameTexture);

			//cmd << "File: " << nameTexture << " -> " << fileTypeExist << endl;

			if (fileTypeExist == 2)
			{
				//outfile << originalName << "<br>";
			}
			else if (fileTypeExist == 1)
			{
				outfile << "<tr>";
				outfile << "<td>[LOCATION MESH]</td>";




				outfile << "<td>";
				outfile << "<span class=\"texture_word_orange\">" << nameTexture << "</span>";
				outfile << "</td>";
				outfile << "<td>";
				outfile << "<span class=\"texture_word_orange\">" << originalName << "</span>";
				outfile << "</td>";
				outfile << "<td>File is only in _WORK</td>";
				outfile << "</tr>";

			}
			else
			{
				outfile << "<tr>";
				outfile << "<td>[LOCATION MESH]</td>";


				outfile << "<td>";
				outfile << "<span class=\"texture_word_red\">" << nameTexture << "</span>";
				outfile << "</td>";
				outfile << "<td>";
				outfile << "<span class=\"texture_word_red\">" << originalName << "</span>";
				outfile << "</td>";
				outfile << "<td>NOT FOUND</td>";
				outfile << "</tr>";
			}


			//cmd << entry.c_str() << endl;
		}

		//auto arrBad = badTextures.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			auto pair = arr.GetSafe(i);
			bool foundOk = false;

			if (!pair->IsNull() && pair->GetValue()->pVob)
			{
				for (auto& pInfo : pair->GetValue()->texturesList)
				{
					auto nameTexture = pInfo.textureName;
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
		outfile << "<p><b>Normal visuals table</b></p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>_WORK/VDF</th><th>\
VDF name</th><th>File type</th><th>Texture TEX</th><th>Texture TGA</th><th>Size</th></tr>";

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
				if (pair->GetValue()->polygons >= 2000) {
					outfile << "<td class='high-poly'>" << pair->GetValue()->polygons << "</td>";
				}
				else {
					outfile << "<td>" << pair->GetValue()->polygons << "</td>";
				}
				outfile << "<td>" << pair->GetValue()->vdfOrWork << "</td>";
				outfile << "<td>" << pair->GetValue()->vdfName << "</td>";
				outfile << "<td>" << pair->GetValue()->fileType << "</td>";

				if (pair->GetValue()->pVob)
				{
					outfile << "<td>";

					for (auto& pInfo : pair->GetValue()->texturesList)
					{
						auto nameTexture = pInfo.textureName;
						auto originalName = nameTexture;


						nameTexture = nameTexture.Replace(".TGA", "");
						nameTexture += "-C";
						nameTexture += ".TEX";

						outfile << nameTexture << "<br>";


					}

					outfile  << "</td>";

					outfile << "<td>";

					for (auto& pInfo : pair->GetValue()->texturesList)
					{
						auto nameTexture = pInfo.textureName;
						auto originalName = nameTexture;
						

						outfile << nameTexture << "<br>";


					}

					outfile << "</td>";

					outfile << "<td>";

					for (auto& pInfo : pair->GetValue()->texturesList)
					{
						if (pInfo.isBigTexture)
						{
							outfile << "<span style='color:#E5044F; font-weight:bold;'>" << pInfo.textureSizeStr << "</span><br>";
						}
						else
						{
							outfile << pInfo.textureSizeStr << "<br>";
						}
						
					}

					outfile << "</td>";
				}
				else
				{
					outfile << "<td></td><td></td><td></td>";
				}
				

				outfile << "</tr>";
			}

		}

		outfile << "</table><br>";

		

		
		zCArray<zCVob*> resultArray;


		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, resultArray, 0);

		zCArray<VisualReportEntryItem*> searchItems;

		int num = resultArray.GetNumInList();

		const zSTRING zSTR_SKIP = "\r\t ";

		for (int i = 0; i < num; i++)
		{
			auto pVob = resultArray.GetSafe(i);

			if (pVob)
			{
				if (auto pItem = pVob->CastTo<oCItem>())
				{
					auto pEntry = new VisualReportEntryItem();
					pEntry->count = 1;
					pEntry->name = pItem->GetInstanceName();
					pEntry->inContainer = false;
					pEntry->coords = pItem->GetPositionWorld();
					searchItems.InsertEnd(pEntry);
				}

				if (auto pCont = pVob->CastTo<oCMobContainer>())
				{
					if (pCont->contains.Length() > 0)
					{
						zSTRING info;
						zSTRING s = pCont->contains;

						int wordnr = 1;
						// Get Instance Name
						do {
							info = s.PickWord(wordnr, ",", zSTR_SKIP);
							if (!info.IsEmpty()) {
								// Create these Objects
								zSTRING name = info.PickWord(1, ":", zSTR_SKIP);
								int num = info.PickWord(3, ":", zSTR_SKIP).ToInt();
								if (num <= 0) num = 1;
								
								int index = parser->GetIndex(name);
								if (index >= 0 && parser->MatchClass(index, oCItem::classDef->scriptClassName))
								{
									auto pEntry = new VisualReportEntryItem();
									pEntry->count = 1;
									pEntry->name = name;
									pEntry->inContainer = true;
									pEntry->contName = pCont->GetObjectName();
									pEntry->coords = pCont->GetPositionWorld();
									searchItems.InsertEnd(pEntry);
								}
							}
							wordnr += 2;

						} while (!info.IsEmpty());
					}
				}

				
			}
		}

		outfile << "<p><b>Items table</b></p><table id=\"table_report_items\"><tr><th>Item instance</th><th>Coords</th><th>In container</th></tr>";

		for (int i = 0; i < searchItems.GetNum(); i++)
		{
			auto entry = searchItems.GetSafe(i);

			if (entry)
			{
				outfile << "<tr>";
				outfile << "<td>" << entry->name << "</td>";
				outfile << "<td>" << entry->coords.ToString() << "</td>";

				if (entry->inContainer)
				{
					if (entry->contName.Length() > 0)
					{
						outfile << "<td>" << entry->contName << "</td>";
					}
					else
					{
						outfile << "<td>" << "yes" << "</td>";
					}
					
				}
				else
				{
					outfile << "<td>" << "-" << "</td>";
				}
				

				outfile << "</tr>";
			}
		}

		outfile << "</table><br>";
		

		searchItems.DeleteListDatas();
		

		outfile << endFile;

		DeleteAndClearMap(searchVisualUniqList);
		badTextures.Clear();


		pListReport.clear();
		outfile.close();
	}

#endif

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


		char* volumeNamePtr = NULL;

		long length = vdf_getvolumename(searchName.ToChar(), volumeNamePtr);

		string volumeNameVdf = "";

		if (volumeNamePtr)
		{
			volumeNameVdf = volumeNamePtr;
			delete[] volumeNamePtr;
		}


		return (volumeNameVdf.Upper() == vdfName);
	}

	void SpacerApp::ToggleNoGrass()
	{
		nograss.hideActive = !nograss.hideActive;
		nograss.SetPercentFromSettings(true);
	}
}