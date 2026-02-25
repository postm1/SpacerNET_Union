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
		int polygons;

		VisualReportEntry::VisualReportEntry()
		{
			amount = 0;
			polygons = 0;
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


	struct VisualReportEntryItem
	{
		zSTRING name;
		int count;
		bool inContainer = false;
		zVEC3 coords;
		zSTRING contName;
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

	void ExtractVisualInfo(zCVisual* visual, Common::MapPair<Common::CString, VisualReportEntry*>* pair)
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
				pair->GetValue()->texturesNames.InsertEnd(mat->texture->GetObjectName());
			}
		}

		// 3DS
		if (auto pProgMesh = visual->CastTo<zCProgMeshProto>())
		{
			//cmd << "mesh! " << curVob->GetVobName() << " visual: " << visual->GetVisualName() << endl;

			pair->GetValue()->polygons += pProgMesh->GetNumTri();

			for (int i = 0; i < pProgMesh->numSubMeshes; i++)
			{

				auto mat = pProgMesh->subMeshList[i].material;

				if (mat && mat->texture)
				{
					//cmd << mat->texture->GetObjectName() << endl;
					pair->GetValue()->texturesNames.InsertEnd(mat->texture->GetObjectName());
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
						pair->GetValue()->texturesNames.InsertEnd(mat->texture->GetObjectName());
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
						pair->GetValue()->texturesNames.InsertEnd(mat->texture->GetObjectName());
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

				ExtractVisualInfo(pModel->nodeList[i]->nodeVisual, pair);
			}
		}
	}

	std::vector<std::string> GetLocationMeshTexturesList()
	{
		zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
		int numPolys = ogame->GetWorld()->bspTree.numPolys;

		std::unordered_set<zCMaterial*> checkedMaterials;
		std::unordered_set<std::string> uniqueTextures;
		std::vector<std::string> result;

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
			std::string texName = mat->texture->GetObjectName().ToChar();

			// Проверяем есть ли текстура вообще в uniqueTextures
			if (uniqueTextures.find(texName) != uniqueTextures.end())
				continue;

			// Формируем имя для проверки физического файла
			std::string originalName = texName.substr(0, texName.length() - 4) + "-C.TEX";
			const char* checkName = originalName.c_str();

			// Проверяем наличие текстуры
			bool hasVirtual = (vdf_fexists(const_cast<char*>(checkName), VDF_VIRTUAL) & VDF_VIRTUAL) == VDF_VIRTUAL;
			bool hasPhysical = (vdf_fexists(const_cast<char*>(checkName), VDF_PHYSICAL) & VDF_PHYSICAL) == VDF_PHYSICAL;

			// Добавляем в результат если:
			// 1. Текстура есть только физически (hasPhysical && !hasVirtual)
			// 2. Текстура не найдена вообще (!hasPhysical && !hasVirtual)
			if ((hasPhysical && !hasVirtual) || (!hasPhysical && !hasVirtual))
			{
				uniqueTextures.insert(texName);
				result.push_back(texName);

				// Логирование для отладки
				if (hasPhysical && !hasVirtual)
				{
					cmd << "PHYS only: " << texName.c_str() << endl;
				}
				else if (!hasPhysical && !hasVirtual)
				{
					cmd << "NOT FOUND: " << texName.c_str() << endl;
				}
			}
		}

		return result;
	}

	void CreateHtmlReport(CString path)
	{

		const CString header = "<!DOCTYPE html><html><head><title>Vobs visuals report</title><style type=\"text/css\" media=\"screen\">html,body{font-family:serif;,sans-serif;\
color:#222222;}p{font-size:20px}table{border-collapse:collapse;border:1px solid grey;font-size:14px;}th{font-size:bold;background-color:#E1E15D;}td,th{border:1px solid grey;padding:5px}\
tr.warning{background-color:#e17a42}tr.error{background-color:red}.texture_word_orange{color:#FF6E00;} .texture_word_red{color:#FF001E;} #table_report tr:nth-child(odd) { background-color: #E4E4E4; color: #222222;} td.high-poly {color: #FD7228; font-weight: bold; }</style></head><body><p>\
<b>There is a list of all vobs' visuals in the location.</b></p>";

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
				if (/*pair->GetValue()->fileType == "3DS" &&*/ pair->GetValue()->pVob != NULL)
				{
					auto curVob = pair->GetValue()->pVob;

					ExtractVisualInfo(curVob->GetVisual(), pair);
				}

				// VDF NAME
				char* volumeNamePtr = NULL;
				long length = vdf_getvolumename(searchName.ToChar(), volumeNamePtr);

				if (volumeNamePtr)
				{
					string volumeNameVdf = volumeNamePtr;

					pair->GetValue()->vdfName = volumeNameVdf;

					delete[] volumeNamePtr;
				}
				else
				{
					//cmd << "NoName: " << searchName << endl;
					continue;
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

		outfile << "<p><b>Not found or _WORK textures</b></p><table id=\"table_bad_tex\"><tr><th>Visual name</th><th>Texture TEX</th><th>Texture TGA</th></tr>";

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
				outfile << "<td>Location mesh</td>";




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
				outfile << "<tr>";
				outfile << "<td>Location mesh</td>";


				outfile << "<td>";
				outfile << "<span class=\"texture_word_red\">" << nameTexture << "</span>";
				outfile << "</td>";
				outfile << "<td>";
				outfile << "<span class=\"texture_word_red\">" << originalName << "</span>";
				outfile << "</td>";
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
		outfile << "<p><b>Normal visuals table</b></p><table id=\"table_report\"><tr><th>Visual name</th><th>Amount</th><th>Polygons</th><th>_WORK/VDF</th><th>\
VDF name</th><th>File type</th><th>Texture TEX</th><th>Texture TGA</th></tr>";

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

					for (int k = 0; k < pair->GetValue()->texturesNames.GetNumInList(); k++)
					{
						auto nameTexture = pair->GetValue()->texturesNames.GetSafe(k);
						auto originalName = nameTexture;

						nameTexture = nameTexture.Replace(".TGA", "");
						nameTexture += "-C";
						nameTexture += ".TEX";

						outfile << nameTexture << "<br>";


					}

					outfile  << "</td>";

					outfile << "<td>";

					for (int k = 0; k < pair->GetValue()->texturesNames.GetNumInList(); k++)
					{
						auto nameTexture = pair->GetValue()->texturesNames.GetSafe(k);
						auto originalName = nameTexture;

						outfile << nameTexture << "<br>";


					}

					outfile << "</td>";
				}
				else
				{
					outfile << "<td></td><td></td>";
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
								/*
								zCPar_Symbol* sym		= parser.GetSymbol(name);
								zCPar_Symbol* classSym	= sym?parser.GetSymbol(parser.GetBaseClass(sym)):0;
								if (classSym && (classSym->name == oCItem::GetStaticClassDef()->GetScriptClassName()) )
								*/
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

	void SpacerApp::ToggleNoGrass()
	{
		nograss.hideActive = !nograss.hideActive;
		nograss.SetPercentFromSettings(true);
	}
}