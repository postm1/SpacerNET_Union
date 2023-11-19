
// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	// Add your code here . . .
	void MatFilter::Init()
	{
		this->filterMatBlocked = 0;

		this->LoadFilterList();

		this->init = true;

		
		this->removeTextureCacheNext = false;

		this->bResizeSmallTextures = false;
		this->bUseAlphaChannels = true;
		this->bUseCenterAligment = true;
		this->worldWasLoadedOnce = false;
	}


	void MatFilter::ToggleWindow(bool value)
	{
		Stack_PushInt(filterMatBlocked);
		Stack_PushInt(value);
		theApp.exports.MatFilter_ToggleWindow();
	}

	void MatFilter::Reset()
	{
		theApp.exports.MatFilter_Clear();
		ToggleWindow(false);
	}

	void MatFilter::RemoveFilterByIndex(int index)
	{
		if (index > 0)
		{
			for (int i = 0; i < matFilterList.GetNum(); i++)
			{
				auto entry = matFilterList.GetSafe(i);

				if (entry)
				{
					if (i == index)
					{
						matFilterList.RemoveOrderIndex(i);
						cmd << "Remove filter: " << index << " name: " << entry->name << endl;
						break;
					}

				}
			}
		}
		else
		{
			cmd << "Remove filter, bad index: " << index << endl;
		}
	}

		
	
	void MatFilter::SearchMaterialByName(CString name)
	{
		zCClassDef* classDef = zCMaterial::classDef;
		zCMaterial*	mat = 0;
		int numOfMats = classDef->objectList.GetNum();
		zSTRING matName = name.Upper();

		zCArray<zCMaterial*> pList;

		for (int i = 0; i < numOfMats; i++)
		{
			mat = dynamic_cast<zCMaterial*>(classDef->objectList[i]);

			if (mat && (mat->GetName().StartWith(matName) || mat->GetName().contains(matName) || mat->GetName() == matName))
			{
				pList.Insert(mat);
			}
		}

		for (int i = 0; i < pList.GetNumInList(); i++)
		{
			auto pMat = pList.GetSafe(i);

			if (pMat)
			{
				Stack_PushInt(pMat->libFlag);
				Stack_PushString(pMat->GetName());
				theApp.exports.MatFilter_AddMatInSearchByName();
			}
			
		}

	}

	void MatFilter::CreateNewMat(CString name, int index)
	{
		zSTRING matName = name.Upper();

		zCClassDef* classDef = zCMaterial::classDef;

		zCMaterial*	mat = 0;
		int numOfMats = classDef->objectList.GetNum();


		if (matName.Length() == 0)
		{
			Message::Box(GetLang("MSG_COMMON_NO_EMPTY_NAME"));
			return;
		}

		for (int i = 0; i < numOfMats; i++)
		{
			mat = dynamic_cast<zCMaterial*>(classDef->objectList[i]);

			if (mat && mat->GetName() == matName)
			{
				Message::Box(GetLang("WIN_MATFILTER_FILTER_MAT_NAME_ALREADY_EXISTS"));
				return;
			}

		}

		auto pMat = new zCMaterial();

		if (pMat)
		{
			pMat->SetName(name);
			//pMat->SetTexture(texture);
			pMat->AddRef();
			pMat->SetUsage(zCMaterial::zMAT_USAGE_LEVEL);

			if (index >= 0)
			{
				pMat->libFlag = matFilterList.GetSafe(index)->id;
			}
			else
			{
				pMat->libFlag = 0;
			}
			

			

			Stack_PushString(pMat->GetName());
			Stack_PushUInt((uint)pMat);
			theApp.exports.AddMatByMatFilterName();

			Stack_PushInt(pMat->libFlag);
			Stack_PushString(pMat->GetName());
			theApp.exports.MatFilter_OnCreateNewMat();
		}
	}


	void MatFilter::FillInterfaceData()
	{

		ToggleWindow(filterMatBlocked != 1);

		if (filterMatBlocked == 1)
		{
			return;
		}



		theApp.exports.Clear_MatFilter_Filters();

		for (int i = 0; i < matFilterList.GetNum(); i++)
		{
			auto entry = matFilterList.GetSafe(i);

			if (entry)
			{
				//cmd << "Push " << entry->name << endl;

				Stack_PushString(entry->name);
				Stack_PushInt(entry->id);
				theApp.exports.Fill_MatFilter_Filters();
			}
		}
	}


	int MatFilter::FindMatFilterIndexByName(CString filterName)
	{
		for (int i = 0; i < matFilterList.GetNum(); i++)
		{
			auto entry = matFilterList.GetSafe(i);

			if (entry)
			{
				CString checkName = entry->name;

				checkName.Lower();

				if (checkName == filterName.Lower())
				{
					return i;
				}

			}
		}

		return -1;
	}





	void MatFilter::OnSelectInMatFilter(zCMaterial* mat)
	{
		int index = mat->libFlag;

		if (index >= 0)
		{
			Stack_PushInt(index);
			theApp.exports.MatFilter_SelectFilterByIndex();

			Stack_PushUInt((uint)mat);
			theApp.exports.MatFilter_SelectMaterialByAddr();



		}
	}

	void MatFilter::FillMatListByFilterName(CString filter)
	{

		//cmd << "Using filter: " << filter << endl;

		zCArray<zCMaterial*>	pList;

		spcCMatFilter* trashItem = 0;
		spcCMatFilter* filterItem = 0;

		int pos = FindMatFilterIndexByName(filter);
		if (pos >= 0)		 filterItem = matFilterList[pos];

		if (!filterItem) return;

		//cmd << "pos: " << pos << endl;

		int undefPos = FindMatFilterIndexByName(FILTER_NAME_TRASH);
		if (undefPos >= 0)
		{
			trashItem = matFilterList[undefPos];
		}
		else
		{
			cmd << "No filter with id: " << undefPos << " Name: " << filter << endl;
			return;
		}

		zCClassDef* classDef = zCMaterial::classDef;

		zCMaterial*	mat = 0;
		unsigned int	matIter = 0;
		unsigned int	numOfMats = classDef->objectList.GetNum();

		if (numOfMats>matIter) mat = dynamic_cast<zCMaterial*>(classDef->objectList[matIter]);

		//cmd << "filterItem: " << filterItem->name << " id: " << filterItem->id << endl;

		zCArray<CString> names;
		//cmd << "numOfMats: " << numOfMats << endl;
		// iterate materials in matlist
		if (pos >= 0)
		{
			while (mat)
			{
				if (mat->GetName().Length() > 0 && (mat->matUsage >= zCMaterial::zMAT_USAGE_LEVEL) && (mat->libFlag == filterItem->id))
				{
					if (!names.IsInList(mat->GetName()))
					{
						pList.Insert(mat);
						names.Insert(mat->GetName());
					}
					else
					{
						cmd << "Material has the same name: " << mat->GetName() << endl;
					}
				}

				matIter++;
				if (numOfMats>matIter) mat = dynamic_cast<zCMaterial*>(classDef->objectList[matIter]);
				else mat = 0;
			}
		}
		//cmd << "pList: " << pList.GetNum() << endl;
		for (int i = 0; i < pList.GetNum(); i++)
		{
			auto entry = pList.GetSafe(i);

			if (entry)
			{
				Stack_PushString(entry->GetName());
				Stack_PushUInt((uint)entry);
				theApp.exports.AddMatByMatFilterName();
			}

		}
		//AddMatByMatFilterName
	}

	void MatFilter::AddNewFilter(CString name)
	{
		spcCMatFilter* item = new spcCMatFilter;
		//item->Init(FILTER_NAME_TRASH, TMatLibFlag::NullLib);
		matFilterList.Insert(item);

		item->name = name;

		zCArray<int> occupiedIndexs;

		for (int i = 0; i < matFilterList.GetNum(); i++)
		{
			occupiedIndexs.InsertEnd(matFilterList.GetSafe(i)->id);
		}
		
		int k = 1;

		while (true)
		{
			if (occupiedIndexs.IsInList(k))
			{
				k++;
				continue;
			}
			else
			{
				break;
			}
		}

		item->id = k;
		cmd << "Add new filter " << name << " with id " << item->id << endl;
		//item->id = matFilterList.GetNum() - 1;




	}
	void MatFilter::OnRenameFilter(int index, CString name)
	{

		if (index >= 0)
		{
			auto entry = matFilterList.GetSafe(index);

			if (entry)
			{
				entry->name = name;
			}
		}

	}

}