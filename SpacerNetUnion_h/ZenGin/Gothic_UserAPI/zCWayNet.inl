// Supported with union (c) 2020 Union team

// User API for zCWayNet
// Add your methods here


inline zBOOL	HasWay(zCWaypoint *w1, zCWaypoint *w2) const { return (w1->HasWay(w2) != NULL); };