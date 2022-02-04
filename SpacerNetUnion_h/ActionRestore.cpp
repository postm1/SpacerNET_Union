// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void ActionRestore::Init()
	{

	}

	ActionRestoreEntry* ActionRestore::Add(zCVob* pVob, ActionRestoreType type)
	{
		auto entry = new ActionRestoreEntry();

		if (entry)
		{
			entry->pVob = pVob;
			entry->type = type;

			pList.Insert(entry);
		}

		return entry;
	}

	void ActionRestore::RestoreVobPos(zCVob* pVob)
	{
		if (!pVob) return;

		cmd << "RestoreVobPos: try" << endl;

		auto list = pList.next;

		while (list)
		{
			auto entry = list->data;

			if (entry && entry->pVob == pVob)
			{
				
				HandleVobTranslation(pVob, entry->pos);
				

				pVob->BeginMovement();
				pVob->SetPositionWorld(pVob->GetPositionWorld());
				
				pVob->EndMovement(FALSE);


				HandleVobRotationMatrix(pVob, entry->rot);

				break;
			}

			list = list->next;
		}

		cmd << "RestoreVobPos: pList size: " << pList.GetNum() << endl;
	}

	ActionRestoreEntry* ActionRestore::AddNewVobPos(zCVob* pVob)
	{
		if (pVob)
		{
			auto entry = this->Add(pVob, ART_VOB_POS);

			entry->pos = pVob->GetPositionWorld();
			entry->rot = pVob->trafoObjToWorld;
			
			cmd << "Restorator add " << endl;
			return entry;
		}

		return NULL;
	}
	
	void ActionRestore::GenerateVobsPos()
	{
		/*
		oCWorld* pGameWorld = dynamic_cast<oCWorld*> (ogame->GetWorld());

		if (!pGameWorld) return;


		zCListSort<zCVob>* voblist_items = pGameWorld->voblist;

		int foundCount = 0;

		while (voblist_items) {
			auto pVob = voblist_items->data;

			voblist_items = voblist_items->next;

			if (pVob
				) {

				auto entry = this->Add(pVob, ART_VOB_POS);

				if (entry)
				{
					entry->pos = pVob->GetPositionWorld();
					entry->rot = pVob->trafoObjToWorld;
				}
			}
		}
		*/

		
		zCArray<zCVob*> activeVobList;

		ogame->GetWorld()->SearchVobListByBaseClass(zCVob::classDef, activeVobList, 0);

		int count = activeVobList.GetNumInList();


		for (int i = 0; i < count; i++) {
			zCVob* vob = activeVobList.GetSafe(i);

			if (vob)
			{
				auto entry = this->Add(vob, ART_VOB_POS);

				if (entry)
				{
					entry->pos = vob->GetPositionWorld();
					entry->rot = vob->trafoObjToWorld;
				}

				
			}
		}
		
		cmd << "GenerateVobsPos: pList size: " << pList.GetNum() << endl;
	}

	void ActionRestore::RemoveByVob(zCVob* pVob)
	{
		if (!pVob) return;


		auto list = pList.next;

		while (list)
		{
			auto entry = list->data;
			if (entry && entry->pVob == pVob)
			{
				pList.Remove(entry);
				break;
			}

			list = list->next;
		}


		cmd << "Remove item pList, Size: " << pList.GetNum() << endl;
	}

	void ActionRestore::Reset()
	{
		cmd << "All remove pList, Size: " << pList.GetNum() << endl;
		
		if (pList.GetNum())
		{
			pList.DeleteListDatas();
		}
		
		cmd << "All remove pList after, Size: " << pList.GetNum() << endl;
	}
}