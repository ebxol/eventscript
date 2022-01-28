#include "libraries.h"
#include "XPO.h"
#include "read.h"

#pragma warning( disable : 4244 )

//=====================================================================
//STRINGS
//=====================================================================
bool XEvent::Print(char *str)
{
	if (*str == '(' && *++str == '\"')
	{
		bool bWrite = false;
		char *pFind = str;

		while (*(pFind++ + 1) != '\0')
			if (*pFind == '\"' && *(pFind+1) ==')')
			{
				bWrite = true;
				break;
			}

		if (bWrite && pFind - str - 1 > 0)
		{
			str++;

			for (; str < pFind; str++)
			{
				if (*str == '\\' && *(str + 1) == 'n')
				{
					printf("\n");

					if (str + 2 < pFind)
					str += 2;

					else break;
				}

				printf("%c", *str);
			}
		}

		return bWrite;
	}

	return false;
}

bool XEvent::PrintValue(char *str)
{
	char varId[32] = { 0 };
	short type = 0;

	if (*str == '(')
	{
		bool bWrite = false;
		char *pFind = str++;
		int i = 0;
		char buf[32];

		while (*pFind++ != '\0')
			if (*pFind == ')')
			{
				bWrite = true;
				break;
			}

		if (bWrite && pFind - str - 1 > 0)
		{
			for (i = 0; i < 31; i++)
				if (str < pFind)
				varId[i] = *str++;

				else break;
		}

		varId[i] = '\0';

		//search and determine type
		AutoState *scan = NULL;

		SearchVarName(varId, &scan);

		if (scan == NULL)
			return false;
		using namespace std;
		switch (scan->GetVarType())
		{
		case VT_INT:
			_itoa_s(scan->GetInt(), buf, 10);
			printf("%s", buf);
			break;
		case VT_FLOAT:
			printf("%f", scan->GetFloat());
			break;
		} 

		return bWrite;
	}

	return false;
}

bool XEvent::GetUserInput(char *str)
{
	while (*str == '(' || *str == ' ')
	{
		if (*str == '\0' || *str == ')')
			return false; //unexpected end of expression

		str++;
	}

	if (IsLetter(*str))
	{
		char *end = str;
		while (*++end != ')')
			if (*end == '\0')
				return false; //unexpected end of epxression

		*end = '\0';

		AutoState *var = Lexicon.StringResult(str);

		*end = ')';

		if (var != NULL)
		{	
			char c;
			while (std::cin.get(c) && c != '\n'); //skip garbage input

			std::string input;
			std::getline(std::cin, input);
			std::remove_if(input.begin(), input.end(), isspace); //filter user input before parsing

			if (input[0] == 0)
				return false; //no user input

			for (unsigned int i = 0; i < input.length(); i++)
			{
				if (!IsNumber(input[i]) && input[i] != '.') //final validation of string
					return false;
			}

			if (var->GetVarType() == VT_INT) //assignment
			{
				int newVal = std::stoi(input);
				var->SetValue<int>(VT_INT, newVal);
				using namespace std;
			}

			else if (var->GetVarType() == VT_FLOAT)
			{
				float newVal = std::stof(input);
				var->SetValue<float>(VT_FLOAT, newVal);
			}

			return true;
		}

		return false;
	}

	return false;
}
//=====================================================================
//VALUES
//=====================================================================
template<class T, class C, class R> R XEvent::MathOp(T a, C b, char Operator)
{
	switch (Operator)
	{
	case '+':
		return a + b;
		break;

	case '-':
		return a - b;
		break;

	case '*':
		return a * b;
		break;

	case '/':
		if (b != 0)
		return a / b;
		break;
	}

	return 0;
}

float XEvent::SumValue(char *str, float &Result)
{
	char p[32] = { 0 };
	char Op;
	bool bAssignment = false;
	int i = 0;
	AutoState *scan = NULL;

	while (*str != '\0' && !IsConditioner(str) && !IsLogical(str))
	{
		if (IsOperator(*str))
		{
			Op = *str++;
			bAssignment = true;

			break;
		}

		else if (IsLetter(*str)) //Result is not yet defined, so define it
		{
			for (int j = 0; IsLetter(*str) && j < 32; j++)	//algorithm can be written into function with fp
				p[j] = *str++;

			SearchVarName(p, &scan);

			if (scan != NULL) //as can this
			{
				if (scan->GetVarType() == VT_FLOAT)
					Result = scan->GetFloat();

				else if (scan->GetVarType() == VT_INT)
					Result = scan->GetInt();
			}

			SumValue(str, Result);
			break;
		}

		else if (IsNumber(*str))
		{
			for (int j = 0; IsNumber(*str) && j < 16; j++)
				p[j] = *str++;

			Result = atof(p);
			SumValue(str, Result);

			break;
		}

		*str++;
	}

	if (bAssignment)
	{
		while (*str != '\0')
		{
			if (IsLetter(*str))
			{
				for (int j = 0; IsLetter(*str) && j < 32; j++)
					p[j] = *str++;

				SearchVarName(p, &scan);

				if (scan != NULL)
				{
					if (scan->GetVarType() == VT_FLOAT)
						Result = MathOp<float, float, float>(Result, scan->GetFloat(), Op);

					else if (scan->GetVarType() == VT_INT)
						Result = MathOp<float, int, float>(Result, scan->GetInt(), Op);

					scan = NULL;
				}

				break;
			}

			if (IsNumber(*str))
			{
				for (int j = 0; IsNumber(*str) && j < 20; j++)
					p[j] = *str++;

				float RValue = atof(p);

				Result = MathOp<float, float, float>(Result, RValue, Op);

				break;
			}

			*str++;
		}

		SumValue(str, Result);
	}

	return Result;
}

template<class T> T XEvent::ScanValue(char *field, unsigned int dist, VType ValueType, bool bOverride)
{
	bool bAssignment = false;
	VType ScanType = VT_INT;	//using VT_INT for constants and VT_FLOAT for vars
	unsigned int fieldspace = (ValueType == VT_INT) ? 0 : 8;

	if (!bOverride)
	{

		for (unsigned int i = 0; i < dist && *field != '\0'; i++)
		{
			if (*field++ == '=')
			{
				bAssignment = true;
				break;
			}
		}
	}

	if (bAssignment || bOverride)
	{
		for (unsigned int i = 0; i < dist; i++)
		{
			if (IsNumber(*field))
			{
				ScanType = VT_INT;
				break;
			}

			if (IsLetter(*field))
			{
				ScanType = VT_FLOAT;
				break;
			}

			*field++;
		}

		if (ScanType == VT_INT) //if constant
		{
			char p[18] = { 0 };

			for (unsigned int i = 0; i < 9 + fieldspace && *field != '\0'; i++)
			{
				p[i] = *field++;
				p[i + 1] = '\0';

				if (!IsNumber(*field) && *field != '.')
					break;
			}

			if (ValueType == VT_INT)
			{
				float Result = (float)atoi(p);

				SumValue(field, Result);

				return (int)Result;
			}

			if (ValueType == VT_FLOAT)
			{
				float Result = atof(p);

				SumValue(field, Result);

				return Result;
			}
		}

		if (ScanType == VT_FLOAT) //variable
		{
			char p[32] = { 0 };
			AutoState *scan = NULL;

			for (int i = 0; i < 31; i++)
			{
				p[i] = *field++;
				p[i + 1] = '\0';

				if (!IsLetter(*field))
					break;
			}

			if (SearchVarName(p, &scan))
			{
				float Result = 0.f;

				if (scan->GetVarType() > VT_FLOAT) //it's an array so look for the index
				{
					int dist = 0;

					if (field == NULL)
						return 0; //weird error

					while (field != '\0' && !Compare(field, "AT", 2))
						field++;

					if (field == '\0')
						return 0; //unexpected end of expression

					field += 2; //skip the "AT"

					int index = ScanBetween<int>(field, '[', ']', VT_INT, &dist);

					field += (2 + dist);

					if (index >= scan->GetInt() || index < 0)
						return 0; //out of bounds index

					//assign the value from the index
					if (scan->GetVarType() == VT_INTARRAY)
					{
						int *a = reinterpret_cast<int*>(scan->GetArrayPtr());
						Result = a[index];
					}

					else if (scan->GetVarType() == VT_FLOATARRAY)
					{
						float *a = reinterpret_cast<float*>(scan->GetArrayPtr());
						Result = a[index];
					}
				}

				else if (scan->GetVarType() == VT_INT)
					Result = scan->GetInt();

				else if (scan->GetVarType() == VT_FLOAT)
					Result = scan->GetFloat();

				SumValue(field, Result);

				return Result;
			}
		}
	}

	return 0;
}

template<class T> T XEvent::ScanBetween(char *str, char beg, char end, VType ValueType, int *d) //look for beginning/ending characters, scan in between them
{
	int start = 0;

	if (beg != 0)
	{
		while (str[start] != beg)
		{
			if (str[start] == '\0') //error
				return -1;

			start++;
		}
	}

	char *term = &str[start];
	
	while (*term != end)
	{
		if (*term == '\0')// error
			return -1;

		term++;
	}

	if (d != NULL)
		*d = term - &str[start];

	*term = '\0';
	T result = ScanValue<T>(&str[start], term - &str[start], ValueType, true); //the range in between term and start
	*term = end;
	return result;
}
//=====================================================================
//CONDITIONS
//=====================================================================
int XEvent::Assert(char *str, char *logicfield, unsigned int step)
{
	if (*(str = FindChar(str, '(', 4)) != '(' && step == 0) //<condition set> <appendage> <condition set>
		return false;

	char p[16] = { 0 };
	char q[16] = { 0 };
	char condition[3] = { 0 };
	int flag = 0;
	int flowcontrol = 0;
	float a, b;

	if (logicfield == NULL && ((logicfield = (char *)malloc(33)) == NULL))
	{
		printf("ERROR: Could not allocate memory for condition assertion \n");
		return false;
	}

	SumValue(str, a);

	for (; *str != '\0'; str++)
	{
		if (IsConditioner(str))	//again, can be used in a function pointer function
		{
			condition[0] = *str;
			condition[1] = *(str + 1);
			str += 2;
			break;
		}
	}

	if (*str == '\0') //unary evaluation
	{
		if (a)
			Jump("THEN");
		else Jump("ENDIF");
	}

	if (condition[0] != '\0')
	{
		SumValue(str, b);

		for (; *str != '\0'; str++)
		{
			if (*str == ')' || *str == '\0')
			{
				flowcontrol = 1;
				break;
			}

			else if (IsLogical(str))	//again, can be used in a function pointer function
			{
				flowcontrol = 2;
				str += 1;
				break;
			}
		}

		if (logicfield == NULL)
			return 0;

		logicfield[step] = Verify(a, b, condition) + 48;

		if (flowcontrol == 1) //time to check all values
		{
			for (unsigned int i = 0; i < step + 2; i += 2)
			{
				if (i + 2 <= step)
				{
					if (logicfield[i + 1] == '&')
					{
						flag = (logicfield[i] - 48 && logicfield[i + 2] - 48);
						logicfield[i + 2] = flag + 48;
					}

					else if (logicfield[i + 1] == '|')
					{
						flag = (logicfield[i] - 48 || logicfield[i + 2] - 48);
						logicfield[i + 2] = flag + 48;
					}
				}

				else flag = logicfield[i] - 48;
			}
		}

		else if (flowcontrol == 2)
		{
				logicfield[step + 1] = *str;
				Assert(str, logicfield, step + 2);
				return 1;
		}

		logicfield[step + 1] = '\0';

		if (flag) //all conditions are true, add to stack //relinquish control of script, let interpreter handle the rest of the condition
		Jump("THEN");

		else Jump("ENDIF");
	}

	if (logicfield != NULL)
		free(logicfield);

	return flag;
}

int XEvent::Verify(const float p, const float q, const char *conditioner)
{
	if (Compare(conditioner, "==") && p == q)
		return 1;

	if (Compare(conditioner, "!=") && p != q)
		return 1;

	if (Compare(conditioner, ">=") && p >= q)
		return 1;

	if (Compare(conditioner, "<=") && p <= q)
		return 1;

	if (*conditioner == '>' && p > q)
		return 1;

	if (*conditioner == '<' && p < q)
		return 1;

	return 0;
}

void XEvent::Jump(const char *cmd)
{
	if (strlen(cmd) > sizeof(commandSlot))
		return;

	EMPTY(commandSlot);
	strcpy_s(commandSlot, cmd);
	bSkip = true;
}

void XEvent::RecordLabel(const char *str, unsigned int value)
{
	if (value == 0)
		return;

	for (int i = 0; str[i] != '\0'; i++)
	{
		if (IsLetter(str[i]))
		{
			AutoState *anchor = Lexicon.AddString(&str[i]);

			if (anchor == NULL)
				return;

			anchor->SetValue<int>(VT_INT, static_cast<int>(value));
		}
	}
}
//=====================================================================
//LOOPS
//=====================================================================
int XEvent::BeginLoop(char *str)
{ 
	if (Loop != NULL) //for now, no nested loops
		return 1;

	while (*str++ != '('); //find starting point (
	while (*str == ' ') //and spaces
		str++;

	int *iterator;
	bool bNew = false;
	int xpos = 0;

	//find iterator
	if (Compare("INT", str, 3)) //"local" variable
	{
		str += 4;
		bNew = true;
		iterator = new int;
	}

	else //try to look up name...
	{
		char *varname = str;
		char old;

		while (*varname != ' ' && *varname != '=') //find the end of the name
		{
			if (*varname == '\0')
				return 1; //erroneous expression

			varname++;
		}

		old = *varname;
		*varname = '\0';

		AutoState *var = Lexicon.StringResult(str);

		if (var == NULL)
			return 1; //invalid variable name

		else iterator = var->GetPrivateInt();

		*varname = old; //replace the termination with the previous character
	}
	
	char *newpos;

	if (SetLoopExpr(str, iterator, bNew, newpos) == 1)
	{
		if (bNew)
			delete iterator;

		return 1; //error
	}

	str = newpos;

	//evaluation expression
	std::string evaluator;
	char op[2];
	
	if (SetLoopEval(str, op, evaluator, newpos) == 1)
	{
		if (bNew)
			delete iterator;

		return 1; //error
	}

	str = newpos;

	while (*str == ' ')
		str++;

	//stride expression
	int strideResult;

	if (IsNumber(*str) || IsLetter(*str))
	{
		unsigned int end;
		for (end = 0; str[end] != ')'; end++)
			if (str[end] == '\0')
			{
				if (bNew)
					delete iterator;

				return 1; //unexpected end of expression
			}

		strideResult = ScanValue<int>(str, end - 1, VT_INT, true);
	}

	else return 1; //invalid expression

	Loop = new LoopQueue(evaluator.c_str(), strideResult, iterator, op, bNew);
	Jump("ENDLOOP");
	return 0;
}

int XEvent::SetLoopEval(char *str, char *op, std::string &eval, char *&startstr)
{
	while (true)
	{
		if (*(str + 1) == '=')
		{
			op[0] = *str;
			op[1] = *(str + 1);
			str += 2;
			break;
		}

		else if (*str == '<' || *str == '>') //look for conditional operator
		{
			op[0] = *str;
			op[1] = 0;
			str++;
			break;
		}

		else if (*str == ';' || *str == '\0' || *str == ')')
			return 1; //unexpected termination of expression

		str++;
	}

	int next;
	for (next = 0; str[next] != ';'; next++)
		if (str[next] == '\0' || str[next] == ')')
			return 1; //unexpected termination of expression

	str[next] = '\0'; //temporarily trim string
	eval = str;
	str[next] = ';';

	next++;
	startstr = &str[next];
	return 0;
}

int XEvent::SetLoopExpr(char *str, int *iterator, bool bDef, char *&startstr)
{
	unsigned int next;

	while (true)
	{
		if (*str == '=')
		{
			for (next = 0; str[next] != ';'; next++) //find range of expression
				if (str[next] == '\0')
					return 1; //unexpected termination of expression

			*iterator = ScanValue<int>(str, next - 1, VT_INT); //assign result of expression to iterator
			next++;

			startstr = &str[next];
			return 0;
		}

		else if (*str == ';') //no assignment
		{
			if (bDef) //if its a new iterator assign default value
				*iterator = 0;

			startstr = str + 1;
			return 0;
		}

		else if (*str = '\0' || *str == ')')
			return 1; //unexpected termination of expression

		str++;
	}

	return 1;
}

int XEvent::ExecLoop()
{
	AutoState *vscan = NULL;
	std::string exprbuf = Loop->GetMaxExpr();
	
	int max = ScanValue<int>(const_cast<char*>(exprbuf.c_str()), exprbuf.length() - 1, VT_INT, true);

	while (!Loop->Terminated(max))
	{
		std::string execstr = Loop->Fetch();
	
		if (bSkip && Compare(execstr.c_str(), commandSlot, strlen(commandSlot)))
		{
			bSkip = false;
			EMPTY(commandSlot);
		}

		if (bSkip)
			continue;
		
		ExecStatement(const_cast<char*>(execstr.c_str()), vscan);
		max = ScanValue<int>(const_cast<char*>(exprbuf.c_str()), exprbuf.length() - 1, VT_INT, true); //keep evaluating the maximum expression as it might change
	} 

	delete Loop;
	Loop = NULL;

	return 0;
}
//=====================================================================
//VARIABLES
//=====================================================================
template<class T> bool XEvent::InsertVar(VType ValueType, T newval, const char *VName) //add new variable
{
	AutoState *Var = Lexicon.AddString(VName);

	if (Var == &Lexicon)
		return false;

	Var->SetValue<T>(ValueType, newval);
	return true;
}

template<class T> bool XEvent::ModifyVar(VType ValueType, T newval, const char *VName)
{
	AutoState *Result = Lexicon.StringResult(VName);

	if (Result == NULL)
		return false;

	Result->SetValue<T>(ValueType, newval);
	return true;
}

bool XEvent::SearchVarName(char *VName, AutoState **VState) //look for a variable by name return if exists
{
	if (VName == NULL) //no point searching for nothing
		return false;

	char *start = VName;

	while (*start == ' ' && start != '\0')
		start++;

	char *end = start;

	while (*end != '\0' && *end != ' ')
		end++;

	char old = *end;
	*end = '\0';

	AutoState *Result = Lexicon.StringResult(start);

	*end = old;

	if (Result == NULL)
		return false;

	*VState = Result;
	return true;
}

bool XEvent::VarAssign(AutoState *node, char *str) //assign new value to variable
{
	if (node == NULL)
		return false;

	str += node->GetName();

	char *idx = str;
	int index = -1; //for array index
	int dist = 0;
	int limit = 0;
	
	while (*idx != '\0' && limit < 4) //for array indexes
	{
		if (Compare(idx, "AT", 2))
		{
			index = ScanBetween<int>(idx + 2, '[', ']', VT_INT, &dist);
			break;
		}

	
		limit++;
		idx++;
	}

	if (index >= node->GetInt())
		return false; //array index out of bounds

	if (index >= 0)
	str = idx + 4 + dist; //skip the [] expresion if it was there

	if (SearchChar(str, 8, '=')) //make sure it is an assignment
	{
		if (node->GetVarType() == VT_INT)
			node->SetValue<int>(VT_INT, ScanValue<int>(str, 8, VT_INT) ); //scans the value of the expression and assigns it to the variable

		else if (node->GetVarType() == VT_FLOAT)
			node->SetValue<float>(VT_FLOAT, ScanValue<float>(str, 8, VT_FLOAT));

		else if (node->GetVarType() == VT_INTARRAY)
			node->SetValue<int>(VT_INTARRAY, ScanValue<int>(str, 8, VT_INT), index);

		else if (node->GetVarType() == VT_FLOATARRAY)
			node->SetValue<float>(VT_FLOATARRAY, ScanValue<float>(str, 8, VT_FLOAT), index);

		return true;
	}

	return false;
}

bool XEvent::VarRemove(const char *VName)
{
	return Lexicon.DeleteString(VName);
}

bool XEvent::InsertVar(char *vid, short type, int &fId, int &strLen)
{
	char VName[32] = { 0 };

	if (IsLetter(*vid))
	{
		strLen = CopyTo(VName, vid);

		switch (type)
		{
		case TYPE_INT:
			return InsertVar<int>(VT_INT, ScanValue<int>(vid, 32, VT_INT), VName);
		case TYPE_FLOAT:
			return InsertVar<float>(VT_FLOAT, ScanValue<float>(vid, 32, VT_FLOAT), VName);
		}
	}

	return false;
}

bool XEvent::ProcessVar(char *str)
{
	int myId = -1;
	int len = 0;

	if (Compare(str, "INT", 3))
	{
		str += 3;

		if (!InsertVar(++str, TYPE_INT, myId, len))
		{
			//HANDLE ERROR
		}
	}

	else if (Compare(str, "FLOAT", 5))
	{
		str += 5;

		if (!InsertVar(++str, TYPE_FLOAT, myId, len))
		{
			//HANDLE ERROR
		}
	}

	return 0;
}

bool XEvent::ProcessArray(char *str)
{
	while (*str == ' ')
	{
		if (*str == '\0')
			return false; //unexpected end of expression
		str++;
	}

	//type declaration
	VType VarType;

	if (Compare(str, "INT", 3))
	{
		str += 3;
		VarType = VT_INTARRAY;
	}

	else if (Compare(str, "FLOAT", 5))
	{
		str += 5;
		VarType = VT_FLOATARRAY;
	}

	else return false; //invalid type
	//end type 

	//get var name
	while (*str == ' ')
	{
		if (*str == '\0')
			return false; //unexpected end of expression
		str++;
	}

	AutoState *newvar = NULL;

	if (IsLetter(*str))
	{
		char *end = str;
		char old;

		while (IsLetter(*end))
			end++;

		old = *end; //replace character to terminate string so we can read the variable name
		*end = '\0';

		AutoState *result = Lexicon.StringResult(str);

		if (result == NULL)
		{
			newvar = Lexicon.AddString(str);
			*end = old;
			str += newvar->GetName();
		}

		else
		{
			*end = old;
			return false; //attempt to replace a var name
		}
	}

	//look for size argument now
	char *numptr = str;
	int dist = 0;

	int asz = ScanBetween<int>(numptr, '[', ']', VT_INT, &dist);

	str += dist;

	if (asz <= 1)
		return false; //invalid argument for array

	void *vArray;

	//initialization
	if (VarType == VT_INTARRAY)
	{
		int *iArray = new int[asz];

		for (int i = 0; i < asz; i++)
			iArray[i] = 0;
	
		vArray = iArray;
	}

	else if (VarType == VT_FLOATARRAY)
	{
		float *fArray = new float[asz];

		for (int i = 0; i < asz; i++)
			fArray[i] = 0;

		vArray = fArray;
	}

	else return false; //couldnt find array type??
	
	newvar->SetArrayPtr(vArray);
	newvar->SetValue<int>(VT_INT, asz); //set pointer and size
	newvar->SetType(VT_INTARRAY);
	return true;
}

bool XEvent::StreamJump(const char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (IsLetter(str[i]))
		{
			AutoState *result = Lexicon.StringResult(&str[i]);

			if (result != NULL)
			{
				std::ifstream *fp;
				Master.RequestFileStream(fp);

				fp->seekg(std::ios::beg); //move back to beginning of file

				for (int i = 0; i < result->GetInt() - 1; i++) //skip n lines
					fp->ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				return true;
			}

			else break;
		}
	}

	return false; //invalid name
}
//=====================================================================
//PUBLIC ENTRY POINT
//=====================================================================
bool XEvent::Interpret(char *str, unsigned int line)
{
	AutoState *vscan = NULL;
	char *scan = str;
	
	if (Loop != NULL)
		Loop->Insert(str);

		if (bSkip && Compare(scan, commandSlot, strlen(commandSlot)))
		{
			bSkip = false;
			EMPTY(commandSlot);
		}

		if (bSkip)
		{
			scan += 1;
			return false;
		}

		if (!ExecStatement(scan, vscan))
			return true;

	return false;
}

bool XEvent::ExecStatement(char *scan, AutoState *vscan, unsigned int l)
{
		if (Compare(scan, "ENDLOOP", 7) && Loop != NULL)
		ExecLoop();

		else if (Compare(scan, "PRINTVAL", 8))
			PrintValue(scan + 8);

		else if (Compare(scan, "PRINT", 5))
			Print(scan + 5);

		else if (Compare(scan, "VAR", 3))
			ProcessVar(scan + 4);

		else if (Compare(scan, "ARRAY", 5))
			ProcessArray(scan + 5);

		else if (Compare(scan, "IF", 2))
			Assert(scan + 2);

		else if (Compare(scan, "LOOP", 4))
			BeginLoop(scan + 4);

		else if (Compare(scan, "ANCHOR", 6))
			RecordLabel(scan + 6, l);

		else if (Compare(scan, "JUMPTO", 6))
			return StreamJump(scan + 6);

		else if (Compare(scan, "USERINPUT", 9))
			GetUserInput(scan + 9);

		else if (SearchVarName(scan, &vscan))
			VarAssign(vscan, scan);

		else if (Compare(scan, "ENDEVENT", 8))
			return true;

		return true;
}