// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void CameraManager::InitAfterWorldLoad()
	{
		if (!ogame->world) return;
	}
	

	void CameraManager::InsertCam(CString name)
	{
		cmd << "try to create a new camera: " << name << endl;


		auto parent = theApp.GetSelectedVob();

		if (theApp.globalParent) parent = theApp.globalParent;

		if (parent && parent->IsPFX())
		{
			print.PrintRed(GetLang("CANT_APPLY_PARENT_VOB"));

			return;
		};


		zCCSCamera* newcam = new zCCSCamera();
		newcam->SetVobName(name);

		newcam->SetPhysicsEnabled(FALSE);
		newcam->SetSleepingMode(zVOB_AWAKE_DOAI_ONLY);
		newcam->SetDrawEnabled(true);
		newcam->SetCollDetStat(FALSE);
		newcam->SetCollDetDyn(FALSE);


		zCCSCamera_EventMsgActivate* msg_c = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETCAMREFERENCE);
		zCCSCamera_EventMsgActivate* msg_t = new zCCSCamera_EventMsgActivate(zCCSCamera_EventMsgActivate::EV_SETTARGETREFERENCE);

		msg_c->referenceVob = newcam;
		msg_t->referenceVob = newcam;

		newcam->OnMessage(msg_c, NULL);
		newcam->OnMessage(msg_t, NULL);

		zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();


		newcam->SetPositionWorld(pos + dir * 200);
		InsertIntoWorld(newcam, parent);

		newcam->Refresh();

		newcam->SetDrawEnabled(TRUE);
		newcam->Release();


		theApp.SetSelectedVob(newcam, "InsertCam");
		cur_cam = newcam;
	}
	void CameraManager::SetCamera(zCVob* pVob)
	{
		zCCSCamera*			isCamera = dynamic_cast<zCCSCamera*>(pVob);
		zCCamTrj_KeyFrame*	keyframe = dynamic_cast<zCCamTrj_KeyFrame*>(pVob);

		if (!isCamera && !keyframe) return;


		if (keyframe)	// ist ein Keyframe
		{
			// Kameraobject suchen
			isCamera = keyframe->cscam;
			if (!isCamera) return;

			// Splinetyp und Position ermitteln
			int posIndex = isCamera->SearchCamKey(keyframe);
			int targetIndex = isCamera->SearchTargetKey(keyframe);

			m_kf_pos = (posIndex >= 0) ? posIndex : targetIndex;
			m_splineselect = (posIndex >= 0) ? 0 : 1;
		}

		cur_cam = isCamera;
		if (cur_cam)
		{
			cur_cam->Refresh();
			m_vobname = cur_cam->GetVobName().ToChar();
		}

	}
}