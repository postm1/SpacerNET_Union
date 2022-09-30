// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	class CameraManager
	{
	public:
		zCVob*		cur_vob;
		zCCSCamera* cur_cam;
		float		lastPresetTime;
		zBOOL		lastPresetAutoAddKF;
		zCCSCamera* editPreset;
		CString	m_vobname;
		int		m_kf_pos;
		int		m_splineselect;
		CString	m_ref_cam;
		CString	m_ref_target;
		float	m_cam_play_duration;
		BOOL	m_insert_at_current_pos;

	public:
		void SetCamera(zCVob*);
		void InitAfterWorldLoad();
		void InsertCam(CString name);
	};
}