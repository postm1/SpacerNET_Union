// Supported with union (c) 2020 Union team

// User API for zCOLOR
// Add your methods here

void			SetRGBA(const zBYTE rgb, const zBYTE a) { r = g = b = rgb; alpha = a; };
void			SetRGBA(const zBYTE rr, const zBYTE gg, const zBYTE bb, const zBYTE a) { r = rr; g = gg; b = bb; alpha = a; };
zREAL			GetIntensityFloat() const { return (0.299F)*float(r) + (0.587F)*float(g) + (0.114F)*float(b); };
zBYTE			GetIntensityByte() const { return ((77)*(r)+(150)*(g)+(29)*(b)) >> 8; };

zBYTE			GetRedByte() const { return r; };
zBYTE			GetGreenByte() const { return g; };
zBYTE			GetBlueByte() const { return b; };
zBYTE			GetAlphaByte() const { return alpha; };

zDWORD			GetARGBDword() const { return (alpha << 24) | (r << 16) | (g << 8) | b; }
static zCOLOR	FromARGB(zDWORD argb) 
{
	zCOLOR result;

	result.alpha = (argb >> 24) & 0xFF;
	result.r = (argb >> 16) & 0xFF;
	result.g = (argb >> 8) & 0xFF;
	result.b = argb & 0xFF;

	return result;
}

inline  zVEC3	GetVEC3() const { return zVEC3(r, g, b); };	/* range 0..255 */