// Supported with union (c) 2020 Union team

// User API for zCCamera
// Add your methods here


void SetVob(zCVob*);
zCVob* GetVob();
zVEC3 Transform(const zVEC3& point);

#if ENGINE != Engine_G1A
inline void BackProject(const int xscr, const int yscr, zVEC3& p) const;
#endif

#if ENGINE <= Engine_G1A
inline void ProjectClamp(zCVertexTransform* vertex, const float inv) const;
#endif