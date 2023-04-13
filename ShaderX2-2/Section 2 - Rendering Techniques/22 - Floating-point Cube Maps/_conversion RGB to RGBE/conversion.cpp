typedef union
{
	unsigned int asInteger;
	float        asFloat;
}
INTORFLOAT;
//---------------------------------------------------------------------------

struct CFloatColor
{
	float R;
	float G;
	float B;
};
//---------------------------------------------------------------------------

struct CEncodedColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char E;

	CEncodedColor()
	{
	}

	CEncodedColor(unsigned char _R, unsigned char _G, 
		unsigned char _B, unsigned char _E)
		: R(_R), G(_G), B(_B), E(_E)
	{
	}
};
//---------------------------------------------------------------------------

inline unsigned char getMantisa(float fVal)
{
	INTORFLOAT val;

	val.asFloat = fVal;
	return ((val.asInteger >> 15) & 255);
}
//---------------------------------------------------------------------------

inline char getExponent(float fVal)
{
	INTORFLOAT val;

	val.asFloat = fVal;
	return ((val.asInteger >> 23) & 255) - 127;
}
//---------------------------------------------------------------------------

inline unsigned char fitMantisa(float fVal, char maxExponent)
{
	unsigned char mantisa = (getMantisa(fVal) >> 1) + 128;
	return mantisa >> (maxExponent - getExponent(fVal));
}
//---------------------------------------------------------------------------

CEncodedColor convert(const CFloatColor& floatColor)
{
	const char TEXTURE_ZERO = 128;

	char maxExponent = max(
		max(getExponent(floatColor.R), getExponent(floatColor.G)),
		getExponent(floatColor.B));

	unsigned char mantisaR = fitMantisa(floatColor.R, maxExponent);
	unsigned char mantisaG = fitMantisa(floatColor.G, maxExponent);
	unsigned char mantisaB = fitMantisa(floatColor.B, maxExponent);

	return CEncodedColor(mantisaR, mantisaG, mantisaB, 
		TEXTURE_ZERO + 1 + maxExponent);
}
//---------------------------------------------------------------------------

/*

	SAMPLE USAGE
	Conversion from classic unsigned char RGB values (uRed, uGreen, uBlue):

	CFloatColor floatColor;

	floatColor.R = uRed   / 255.0f;
	floatColor.G = uGreen / 255.0f;
	floatColor.B = uBlue  / 255.0f;

	CEncodedColor encodedColor = convert(floatColor);

*/