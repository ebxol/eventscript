#ifndef _AUTOSTATE_H
#define _AUTOSTATE_H
#include <list>
#include <stdint.h>
typedef uint64_t longlong;
enum VType { VT_INT, VT_FLOAT, VT_STRING, VT_INTARRAY, VT_FLOATARRAY, VT_EMPTY };
class AutoState
{
private:
	char label;
	unsigned int name;
	longlong alpha;
	std::list<AutoState*> transit;

	union UVar
	{
		int IntVar;
		float FloatVar;
	} UValue;

	VType VarType;
	void *arrayValue;
	
public:
	inline bool HasTransition(char label) { return (	(alpha & (1ULL << (label - '0'))	) != 0); };
	inline void MakeStart() { label = 0; VarType = VT_EMPTY; };
	inline unsigned int GetName() { return name; };
	inline float GetFloat() { return UValue.FloatVar; };
	inline int GetInt() { return UValue.IntVar; };
	inline VType GetVarType() { return VarType; };
	inline char GetLabel() { return label; };
	inline int *GetPrivateInt() { return &UValue.IntVar; };
	inline void *GetArrayPtr() { return arrayValue; };
	inline void SetArrayPtr(void *addr) { arrayValue = addr; };
	inline void SetType(VType NewType) { VarType = NewType; };

	template<class T> void SetValue(VType ValueType, T value, int index = 0)
	{
		float *fa;
		int *ia;

		switch (ValueType)
		{
		case VT_FLOAT:
			UValue.FloatVar = static_cast<float>(value); //cast to avoid compiler warnings
			break;

		case VT_INT:
			UValue.IntVar = static_cast<int>(value); // cast to avoid compiler warnings
			break;

		case VT_INTARRAY:
			ia = reinterpret_cast<int*>(arrayValue);
			ia[index] = static_cast<int>(value);
			break;

		case VT_FLOATARRAY:
			fa = reinterpret_cast<float*>(arrayValue);
			fa[index] = static_cast<float>(value);
			break;

		default: //error
			break;
		}

		VarType = ValueType;
	}

	bool AddTransition(char label, AutoState **result);
	bool DeleteString(const char *str);
	AutoState *AddString(const char *str);
	AutoState *StringResult(const char *str);
	AutoState *MakeTransitionExt(const char *str);
	AutoState *MakeTransition(char label);

	AutoState(char l, unsigned int n);
	AutoState();
	~AutoState();
};

#endif