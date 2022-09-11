// Supported with union (c) 2020 Union team

// User API for zCShaderStage
// Add your methods here

void Clear() {
	memset(this, 0, sizeof(*this));			// [BC] static memory overrun, false alert?
};