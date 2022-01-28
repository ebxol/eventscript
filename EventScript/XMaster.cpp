#include "libraries.h"
#include "read.h"
#include "XPO.h"

#define LINE_SIZE 1024

bool XMaster::ExecEvent(std::string &fn, char *eventline)
{
	if (Status & StatusCode(SC_BUSY))
		return false;

	if (read_main(fn, eventline) == 1) //error caught
		return true;

	return false;
}

int XMaster::read_main(std::string &fn, char *eventline)
{
	using namespace std;

	SetStatus(SC_BUSY);

	try
	{
		EventSource.open(fn, std::ios::in);

		if (!EventSource.is_open())
			throw std::invalid_argument("Invalid file reference");
	}

	catch (const std::ifstream::failure &f)
	{
		cout << "Exception: " << f.what() << endl;
		return 1;
	}

	catch (const std::invalid_argument &e)
	{
		cout << "Exception: " << e.what() << endl;
		return 1;
	}
	
	cout << "Executing " << fn.c_str() << endl;
	cout << "******************************" << endl;
	cout << endl;
	cout << endl;

	//Initialize XPO
	XEvent RunTimeXPO(LINE_SIZE, *this);
	CurrentEvent = &RunTimeXPO;

	unsigned int cLine = 1;

	while (GetLine(EventSource, eventline))
	{
		stoupper(eventline);

		//Send to XPO
		if (RunTimeXPO.Interpret(eventline, cLine))
		{
			CurrentEvent = NULL;
			return 1;
		}

		cLine++;
	}

	CurrentEvent = NULL;
	EventSource.close();
	StripStatus(SC_BUSY);

	return 0;
}

inline int XMaster::StatusCode(SCode N)
{
	return (1 << N);
}

inline void XMaster::SetStatus(SCode N)
{
	Status |= (1 << N);
}

inline void XMaster::StripStatus(SCode N)
{
	Status &= ~(1 << N);
}

void XMaster::RequestFileStream(std::ifstream *&F)
{
	F = &EventSource;
}

bool XMaster::GetLine(std::ifstream &fp, char *scriptline) //get line from filestream and filter it
{
	using namespace std;

	while (!fp.eof()) //this will clean the string and remove all spaces before and after a valid character but NOT in between characters
	{
		bool bKill = false;
		string line;
		std::getline(fp, line);

		//remove unwanted escape characters
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

		if (line.length() <= 1)
			continue;

		int strstart = line.find_first_not_of(' '); //trim exterior whitespace
		int strend = line.find_last_not_of(' ');

		line = line.substr(strstart, strend - strstart + 1);

		if ((line[0] == 0) || ((line[0] == '/') && (line[0 + 1] == '/'))) //if comment or invalid character, not useful
			continue;

		for (unsigned int i = 0; i < line.length(); i++)
			if (!iswascii(line[i]))
				bKill = true;

		if (bKill) //bad characters detected
			continue;

		strcpy_s(scriptline, LINE_SIZE, line.c_str()); //line is good to go
		return true;
	}

	return false; //blank line maybe
}
