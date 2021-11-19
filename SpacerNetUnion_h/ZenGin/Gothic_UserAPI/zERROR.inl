// Supported with union (c) 2020 Union team

// User API for zERROR
// Add your methods here

void zERROR::Init_Union(zSTRING cmd);
int zERROR::Report_Union(zERROR_TYPE type, int id, zSTRING const& str_text, signed char levelPrio, unsigned int flag, int line, char* file, char* function);