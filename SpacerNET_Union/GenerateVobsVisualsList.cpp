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

				if (IsSpacerVob(pVob))
				{
					continue;
				}

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

					if (entry.vdfName == "SpacerNET.mod")
					{
						continue;
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

				if (entry.vdfName == "SpacerNET.mod")
				{
					continue;
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


		/*
		for (auto& it : pListReport)
		{
			it.PrintData();
			
		}
		*/
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
body{font-family:'Segoe UI',Arial,sans-serif;background:#f5f5f5;color:#222222;padding:10px;overflow-y:scroll;}\
html{scrollbar-gutter: stable;}\
h1{font-size:24px;margin:20px 0;color:#2c3e50}\
p{font-size:18px;margin:15px 0}\
.page-container{max-width:1920px;margin:0 auto;width:100%}\
table{width:100%;border-collapse:collapse;background:white;border:2px solid #444;margin:14px 0}\
th{background:#E1E15D;color:#222;font-weight:bold;padding:6px;border:1px solid #444;text-align:left;cursor:pointer;user-select:none;white-space:nowrap}\
td{padding:4px;border:1px solid #444}\
.table_report tr:nth-child(even){background-color:#E4E4E4}\
.table_report tr:nth-child(odd){background-color:#ffffff}\
tr.warning{background-color:#e17a42!important}\
tr.warning td{background-color:#e17a42;color:#222;border:1px solid #444}\
tr.error{background-color:#ff4444!important}\
tr.error td{background-color:#ff4444;color:#222;border:1px solid #444}\
.texture_word_orange{color:#FF6E00;font-weight:bold}\
.texture_word_red{color:#FF001E;font-weight:bold}\
td.high-poly{color:#E5044F;font-weight:bold}\
.report-time{background:#2897FF;padding:15px;border:2px solid #444444;margin:20px 0;color:white;font-size:22px;font-weight:bold;text-align:center}\
.tabs{width:100%;margin-top:10px}.tabs input{display:none}.tabs-labels{display:flex;flex-wrap:wrap;border-bottom:2px solid #444;margin-bottom:10px}.tabs-labels label{padding:10px 15px;cursor:pointer;background:#ddd;border:1px solid #444;border-bottom:none;margin-right:5px;font-weight:700}.tabs-labels label:hover{background:#ccc}.tab{display:none;background:#fff;padding:10px;border:2px solid #444}#tab1:checked~.tabs-labels label[for=tab1],#tab2:checked~.tabs-labels label[for=tab2],#tab3:checked~.tabs-labels label[for=tab3],#tab4:checked~.tabs-labels label[for=tab4],#tab5:checked~.tabs-labels label[for=tab5]{background:#fff;border-bottom:2px solid #fff;text-decoration:underline}#tab1:checked~#content1,#tab2:checked~#content2,#tab3:checked~#content3,#tab4:checked~#content4,#tab5:checked~#content5{display:block}\
</style></head><body>";

		const CString endFile = R"(</div><script>document.querySelectorAll('.table_report').forEach(t=>t.querySelectorAll('th').forEach((h,i)=>{h.dataset.o=h.innerHTML;h.innerHTML+=' &#8597;';h.onclick=()=>{let b=t.tBodies[0],d=h.d=!h.d,r=[...b.rows].filter(x=>!x.querySelector('th'));t.querySelectorAll('th').forEach(x=>x.innerHTML=x.dataset.o+' &#8597;');h.innerHTML=h.dataset.o+(d?' &#9650;':' &#9660;');r.sort((a,c)=>{let x=a.cells[i].innerText.trim(),y=c.cells[i].innerText.trim(),nx=Number(x),ny=Number(y),v=x!==''&&y!==''&&!isNaN(nx)&&!isNaN(ny)?nx-ny:x.localeCompare(y,undefined,{numeric:true,sensitivity:'base'});return d?v:-v});r.forEach(x=>b.appendChild(x))}}))</script></body></html>)";

		std::ofstream outfile;
		outfile.open(path, std::ios_base::trunc);


		outfile << header;


		int pos = path.SearchReverse("\\");

		CString result;

		if (pos != -1)
		{
			result = path.Copy(pos + 1, path.Length() - (pos + 1));
		}
		else
		{
			result = path; // если слеша нет
		}

		outfile << "<div class=\"page-container\"><div class = \"report-time\">Report generated: " + GetTimeForReport() + " | " + result + "</div>";

		outfile << R"(
<div class="tabs">

<input type="radio" name="tabs" id="tab1" checked>
<input type="radio" name="tabs" id="tab2">
<input type="radio" name="tabs" id="tab3">
<input type="radio" name="tabs" id="tab4">
<input type="radio" name="tabs" id="tab5">

<div class="tabs-labels">
<label for="tab1">Warnings (Visuals)</label>
<label for="tab2">Warnings (Textures)</label>
<label for="tab3">Visuals</label>
<label for="tab4">Location mesh</label>
<label for="tab5">Items</label>
</div>
)";


		outfile << "<div class='tab' id='content1'>";

		// WARNING VISUALS
		outfile << "<p><b>Warning visuals table</b></p><table id=\"table_report_warn\" class=\"table_report\">\
<tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>Problem type</th></tr>";


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

				outfile << "<td>" << it.GetProblemType() << "</td>";

				outfile << "</tr>";
			}
		}

		outfile << "</table>";
		outfile << "</div>";

		outfile << "<div class='tab' id='content2'>";
		// WARNING MESH LOCATION
		outfile << "<p><b>Warning location (mesh) textures table</b></p><table id=\"table_report_mesh_warn\" class=\"table_report\"><tr><th>Material name</th><th>Texture name TGA</th><th>Texture name TEX</th><th>Size</th><th>Problem type</th></tr>";


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

				outfile << "<td>" << it.GetProblemType() << "</td>";

				outfile << "</tr>";
			}
		}

		outfile << "</table>";
		outfile << "</div>";
		// USUAL VOBS

		outfile << "<div class='tab' id='content3'>";
		outfile << "<p><b>Usual visuals table</b></p><table id=\"table_report_visuals\" class=\"table_report\">\
<tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>_WORK/VDF</th><th>VDF name</th><th>Material | Texture | Size</th>";


		std::vector<VisualReportEntry*> usualVisuals;
		for (auto& it : pListReport)
		{
			if (!it.isLocationMesh && it.foundType >= VDF_FILE_TYPE_VDF)
			{
				usualVisuals.push_back(&it);
			}
		}
		std::sort(usualVisuals.begin(), usualVisuals.end(),
			[](const VisualReportEntry* a, const VisualReportEntry* b) {

				std::string sa = a->visualName;
				std::string sb = b->visualName;

				std::transform(sa.begin(), sa.end(), sa.begin(), ::tolower);
				std::transform(sb.begin(), sb.end(), sb.begin(), ::tolower);
				return sa < sb;
			});

		for (auto& it : usualVisuals)
		{
			if (!it->isLocationMesh && it->foundType >= VDF_FILE_TYPE_VDF) // only good visuals
			{
				outfile << "<tr>";
				outfile << "<td>" << it->visualName << "</td>";
				outfile << "<td>" << it->amount << "</td>";

				if (it->polygons >= 3000)
				{
					outfile << "<td class='high-poly'>" << it->polygons << "</td>";
				}
				else {
					outfile << "<td>" << it->polygons << "</td>";
				}

				outfile << "<td>" << it->GetFileTypeFound() << "</td>";
				outfile << "<td>" << it->vdfName << "</td>";

				/*
				outfile << "<td>" << it.materialsList[0]->GetObjectName() << "</td>";
				outfile << "<td>" << it.texturesList[0].textureNameTGA << "</td>";
				outfile << "<td>" << it.texturesList[0].textureNameTEX << "</td>";

				outfile << "<td>" << it.texturesList[0].textureSizeInfoStr << "</td>";
				*/

				int count = it->texturesList.size();

				outfile << "<td>";

				for (int i = 0; i < count; i++)
				{
					auto& entry = it->texturesList[i];

					if (entry.material)
					{
						if (entry.isBigTexture)
						{
							outfile << entry.material->GetObjectName()
								<< " | "
								<< entry.textureNameTGA
								<< " | <span style='color:#E5044F; font-weight:bold;'>"
								<< entry.textureSizeInfoStr
								<< "</span>"
								;
						}
						else
						{
							outfile << entry.material->GetObjectName()
								<< " | "
								<< entry.textureNameTGA
								<< " | "
								<< entry.textureSizeInfoStr
								;
						}

					}

					if (count > 1)
					{
						outfile << "<br>";
					}
				}



				outfile << "</td></tr>";



			}
		}


		outfile << "</table>";
		outfile << "</div>";


		outfile << "<div class='tab' id='content4'>";
		// USUAL MESH LOCATION
		outfile << "<p><b>Normal location (mesh) textures table</b></p><table id=\"table_report_mesh\" class=\"table_report\"><tr><th>Material name</th><th>Texture name TGA</th>\
<th>Texture name TEX</th>\
<th>Size</th><th>_WORK/VDF</th><th>VDF name</th>\
</tr>";


		std::vector<VisualReportEntry*> usualMeshes;
		for (auto& it : pListReport)
		{
			if (it.isLocationMesh && it.foundType >= VDF_FILE_TYPE_VDF)
			{
				usualMeshes.push_back(&it);
			}
		}
		std::sort(usualMeshes.begin(), usualMeshes.end(),
			[](const VisualReportEntry* a, const VisualReportEntry* b) {


				std::string sa = a->materialsList[0]->GetObjectName();
				std::string sb = b->materialsList[0]->GetObjectName();

				std::transform(sa.begin(), sa.end(), sa.begin(), ::tolower);
				std::transform(sb.begin(), sb.end(), sb.begin(), ::tolower);
				return sa < sb;
			});

		for (auto& it : usualMeshes)
		{
			if (it->isLocationMesh && it->foundType >= VDF_FILE_TYPE_VDF) // only good textures
			{

				outfile << "<td>" << it->materialsList[0]->GetObjectName() << "</td>";
				outfile << "<td>" << it->texturesList[0].textureNameTGA << "</td>";
				outfile << "<td>" << it->texturesList[0].textureNameTEX << "</td>";


				if (it->texturesList[0].isBigTexture)
				{
					outfile
						<< "<td><span style='color:#E5044F; font-weight:bold;'>"
						<< it->texturesList[0].textureSizeInfoStr
						<< "</span>"
						;
				}
				else
				{
					outfile << "<td>" << it->texturesList[0].textureSizeInfoStr << "</td>";
				}


				outfile << "<td>" << it->GetFileTypeFound() << "</td>";

				outfile << "<td>" << it->vdfName << "</td>";

				outfile << "</tr>";
			}
		}

		outfile << "</table>";
		outfile << "</div>";

		//=============================================================================================
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

		outfile << "<div class='tab' id='content5'>";
		outfile << "<p><b>Items table</b></p><table id=\"table_report_items\" class=\"table_report\"><tr><th>Item instance</th><th>Coords</th><th>In container</th></tr>";

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

		outfile << "</table>";
		outfile << "</div>";

		searchItems.DeleteListDatas();

		//====================================================================================================
		outfile << endFile;

		pListReport.clear();
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

	
}