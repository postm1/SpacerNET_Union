// Supported with union (c) 2020 Union team

// User API for zCCamera
// Add your methods here


void SetVob(zCVob*);
zCVob* GetVob();
zVEC3 Transform(const zVEC3& point);
inline void BackProject(const int xscr, const int yscr, zVEC3& p) const;



// { connectedVob = vob; };
//zCVob* GetVob(); // const;// { return connectedVob; };
//zVEC3 Transform(const zVEC3& point);// const { return camMatrix * point; };	// cam should be 'acitve'

/*
inline void BackProject(const int xscr, const int yscr, zVEC3& p) const {
	// coord-trafo: screen->3d
	// Assumption : p.n[VZ] must have been set before call !
	p.n[VX] = zREAL(xscr - zFloat2Int(vpData.xcenter)) * zREAL(viewDistanceXInv) * p.n[VZ];
	p.n[VY] = zREAL(zFloat2Int(vpData.ycenter) - yscr) * zREAL(viewDistanceYInv) * p.n[VZ];
};
*/
