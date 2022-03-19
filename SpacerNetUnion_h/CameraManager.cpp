// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void CameraManager::InitAfterWorldLoad()
	{
		if (!ogame->world) return;
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