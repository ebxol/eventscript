#include "AutoState.h"
#include "LoopQueue.h"

#define EMPTY(s) memset(&s[0], 0, sizeof(s))

#ifndef __XPO__
#define __XPO__
class XMaster;
//XPO.H
class XEvent
{
private:
	unsigned int STRSZ;
	char commandSlot[12];
	bool bSkip;
	const static int IdSize = 32;

	XMaster &Master;
	AutoState Lexicon;
	LoopQueue *Loop;

	//AUTOMATA
	template<class T> bool InsertVar(VType ValueType, T newval, const char *VName);
	template<class T> bool ModifyVar(VType ValueType, T newval, const char *VName);
	bool SearchVarName(char *VName, AutoState **VState);
	bool VarAssign(AutoState *node, char *str);
	bool VarRemove(const char *VName);

	//TASKS
	bool Print(char *str);
	bool PrintValue(char *str);
	bool ProcessVar(char *str);
	bool ProcessArray(char *str);
	bool InsertVar(char *str, short type, int &fId, int &strLen);
	bool ExecStatement(char *str, AutoState *, unsigned int l = 0);
	bool StreamJump(const char *str);
	bool GetUserInput(char *str);

	//VALUES
	float SumValue(char *str, float &Result);
	template<class T, class C, class R> R MathOp(T a, C b, char Op);
	template<class T> T ScanValue(char *str, unsigned int dist, VType ValueType, bool bOverride = false);
	template<class T> T ScanBetween(char *str, char beg, char end, VType ValueType, int *dist = NULL);

	//CONDITIONS
	int Assert(char *str, char *logicfield = NULL, unsigned int steps = 0);
	int Verify(const float p, const float q, const char *conditioner);
	void Jump(const char *cmd);
	void RecordLabel(const char *, unsigned int value);

	//LOOPS
	int BeginLoop(char *str);
	int SetLoopExpr(char *str, int *, bool bDef, char *&next);
	int SetLoopEval(char *str, char *op,  std::string &eval, char *&next);
	int ExecLoop();

	//GENERAL
	void ErrorPrompt(const char *error, bool bWarn);

public:
	bool Interpret(char *str, unsigned int line);

	XEvent(int MaxStringSize, XMaster &NewMaster) : Master(NewMaster), Loop(NULL), bSkip(false) { STRSZ = (MaxStringSize > 0) ? MaxStringSize : 1024; EMPTY(commandSlot); };
	~XEvent() { if (Loop != NULL) delete Loop; };
};
#endif

#ifndef __XMASTER__
#define __XMASTER__
class XMaster
{
private:
	std::ifstream EventSource; //script-wide file stream
	XEvent *CurrentEvent; //current stream of execution
	int Status;
	unsigned int LevelCode;
	enum SCode { SC_IDLING, SC_BUSY };

	bool GetLine(std::ifstream &fp, char *scriptline); //get a line from filestream
	int read_main(std::string &fn, char *tstr); //read in starting point

	inline void SetStatus(SCode);
	inline void StripStatus(SCode);
	inline int StatusCode(SCode);

public:
	bool ExecEvent(std::string &fn, char *tstr);
	void RequestFileStream(std::ifstream *&F); //executor requests script file
	inline int GetStatus() { return Status; }

	XMaster() { };
	XMaster(std::string &fn, char *tstr) { ExecEvent(fn, tstr);  };
	~XMaster() { };
};

#endif

#define TYPE_INT 3
#define TYPE_FLOAT 5

enum VBOOL { VB_FALSE, VB_TRUE };