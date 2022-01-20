// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	MultiSelect::MultiSelect() {
		BoundsView = Null;
	}



	void MultiSelect::DrawBounds() {
		if (!BoundsView) {
			BoundsView = new zCView();
			BoundsView->InsertBack("Select");
			BoundsView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_ADD);
			BoundsView->SetTransparency(50);
		}

		zVEC2 v1 = PickBegin;
		zVEC2 v2 = theApp.GetMousePosVirt();

		Bounds[VA] = zVEC2(min(v1[VX], v2[VX]), min(v1[VY], v2[VY]));
		Bounds[VB] = zVEC2(max(v1[VX], v2[VX]), max(v1[VY], v2[VY]));
		zVEC2 size = Bounds[VB] - Bounds[VA];

		BoundsView->SetPos(Bounds[VA][VX], Bounds[VA][VY]);
		BoundsView->SetSize(size[VX], size[VY]);
	}



	void MultiSelect::DoPick() {

		if (keys.KeyPressed("VOB_DISABLE_SELECT", true))
		{
			ClearSelection();
			return;

		}

		if (keys.KeyPressed("VOB_DELETE", true))
		{
			RemoveSelected();
			return;

		}

		if (!Locked && zMouseKeyPressed(Left)) {
			PickBegin = theApp.GetMousePosVirt();
			Locked = True;
			screen->InsertItem(BoundsView);

		}
		else if (Locked && !zMouseKeyPressed(Left)) {
			TraceRayEnabled = zKeyPressed(KEY_LSHIFT);
			PickEnd = theApp.GetMousePosVirt();

			Locked = False;
			screen->RemoveItem(BoundsView);

			PickBounds();
				
		}

		if (Locked)
			DrawBounds();
	}

	void MultiSelect::PickSingle(zCVob* vob) {
		if (zKeyPressed(KEY_LSHIFT))
			return AddToSelection(vob);

		if (zKeyPressed(KEY_LALT))
			return RemoveFromSelection(vob);

		ClearSelection();
		AddToSelection(vob);
	}

	void MultiSelect::PickBounds() {
		bool add = zKeyPressed(KEY_LSHIFT);
		bool sub = zKeyPressed(KEY_LALT);
		bool ignoreCollisions = zKeyPressed(KEY_LCONTROL);

		if (!add && !sub)
			ClearSelection();

		oCWorld* world = ogame->GetGameWorld();
		zCVob*   camera = ogame->GetCameraVob();

		auto& activeVobList = world->bspTree.renderVobList;
		for (int i = 0; i < activeVobList.GetNum(); i++) {
			zCVob* vob = activeVobList[i];
			if (vob->showVisual && vob->visual /*&& camera->GetDistanceToVob( *vob ) <= 5000.0f*/) {


				zVEC3 cameraPosition = camera->GetPositionWorld();
				zVEC3 vobPosition = vob->GetPositionWorld();
				zVEC3 direction = vobPosition - cameraPosition;

				if (!ignoreCollisions) {
					int flags = zTRACERAY_STAT_POLY | zTRACERAY_VOB_IGNORE | zTRACERAY_VOB_IGNORE_PROJECTILES;
					if (world->TraceRayFirstHit(cameraPosition, direction, (zCVob*)Null, flags))
						continue;
				}

				zVEC3 normal = direction.Normalize();
				float angle = Alg_AngleUnitRad(camera->GetAtVectorWorld(), direction);

				if (angle < RAD90) {
					int x, y;
					GetProjection(x, y, vobPosition);

					if (x >= Bounds[VA][VX] && x < Bounds[VB][VX] && y >= Bounds[VA][VY] && y < Bounds[VB][VY]) {
						if (sub)
							RemoveFromSelection(vob);
						else
							AddToSelection(vob);
					}
				}
			}
		}
	}

	void MultiSelect::AddToSelection(zCVob* vob) {
		if (!vob)
			return;

		vob->SetDrawBBox3D(True);
		theApp.SelectedVobs |= vob;
	}

	void MultiSelect::RemoveFromSelection(zCVob* vob) {
		if (!vob)
			return;

		vob->SetDrawBBox3D(False);
		theApp.SelectedVobs ^= vob;
	}

	void MultiSelect::RemoveSelected()
	{
		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
		{
			theApp.SelectedVobs[i]->SetDrawBBox3D(False);
			theApp.RemoveVob(theApp.SelectedVobs[i]);
		}
			
		theApp.SelectedVobs.Clear();
	}

	void MultiSelect::ClearSelection() 
	{
		for (uint i = 0; i < theApp.SelectedVobs.GetNum(); i++)
			theApp.SelectedVobs[i]->SetDrawBBox3D(False);

		theApp.SelectedVobs.Clear();
	}
}