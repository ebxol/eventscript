// EventScriptBase.cpp : Defines the entry point for the console application.
//

#include "libraries.h"
#include "XPO.h"

#define STRING_LENGTH 1024

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;

	XMaster PublicEvent;
	char tstr[STRING_LENGTH];
	int result = 0;

	std::string input;

	cout << "***EVENT SCRIPT INTERPRETER***" << endl;
	cout << "******************************" << endl;
	cout << "Type 'help' to get started " << endl;

	while (input != "exit")
	{
		cin >> input;

		if (input == "exec")
		{
			std::string fileIn;
			cout << "Type in the name of the file (must be in the same directory): ";
			cin >> fileIn;

			if (PublicEvent.ExecEvent(fileIn, tstr))
				printf("WARNING: Abnormal termination of interpreter \n");

			printf("\n");
			scanf_s("%d \n", &result);
		}

		else if (input == "help")
		{
			cout << endl;
			cout << "Type 'exec' to execute a new script " << endl;
			cout << "Type 'exit' to exit the program " << endl;
		}

		else if (input == "exit")
			cout << "Goodbye!" << endl;

		else cout << "Invalid command " << endl;

	}

	return 0;
}