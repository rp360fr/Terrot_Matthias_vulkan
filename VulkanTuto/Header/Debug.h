#pragma once
class Debug
{
public:

#ifdef _DEBUG
	static constexpr bool enabaleDebug = true;
#else				  
	static constexpr bool enabaleDebug = false;
#endif // _DEBUG
};

