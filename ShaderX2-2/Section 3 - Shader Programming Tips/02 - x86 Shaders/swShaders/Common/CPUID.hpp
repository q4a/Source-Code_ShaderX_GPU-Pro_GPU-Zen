#ifndef CPUID_hpp
#define CPUID_hpp

namespace swShader
{
	class CPUID
	{
	public:
		static bool supportsMMX();
		static bool supportsSSE();

	private:
		static bool pentium;
		static bool MMX;
		static bool SSE;

		static bool detectPentium();
		static bool detectMMX();
		static bool detectSSE();
	};
}

namespace swShader
{
	inline bool CPUID::supportsMMX()
	{
		return MMX;
	}

	inline bool CPUID::supportsSSE()
	{
		return SSE;
	}
}

#endif   // CPUInfo_hpp