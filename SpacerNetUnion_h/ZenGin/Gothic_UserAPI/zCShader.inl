// Supported with union (c) 2020 Union team

// User API for zCShader
// Add your methods here

zCShaderStage*			GetStage(int stageIndex) const { return stageList[stageIndex]; };
void					AddStage(zCShaderStage *stage) { zERR_ASSERT(numStages<MAX_STAGES);	 stageList[numStages++] = stage; };