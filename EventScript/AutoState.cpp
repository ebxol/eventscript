#include "AutoState.h"
#include "read.h"
#include <stack>
#include <iostream>


AutoState::AutoState() : name(0)
{
	MakeStart();
}

AutoState::AutoState(char ilabel, unsigned int iname) : alpha(0), name(iname), label(ilabel), arrayValue(NULL)
{
	VarType = VT_EMPTY;
}

AutoState::~AutoState()
{
	if (label = 0) //this means it's a start state, so do DFS deletion starting here
	{
		std::stack<AutoState*> S;

		//first set of pushes
		for (std::list<AutoState*>::const_iterator i = transit.begin(); i != transit.end(); i++)
		{
			AutoState *next = *i;

			if (next->name != 0xDEADBEEF)
			{
				next->name = 0xDEADBEEF;
				S.push(next);
			}
		}

		//DFS
		while (S.size() > 0)
		{
			AutoState *next = S.top();
			S.pop();

			for (std::list<AutoState*>::const_iterator i = next->transit.begin(); i != next->transit.end(); i++)
			{
				AutoState *nextnext = *i;

				if (nextnext->name != 0xDEADBEEF)
				{
					nextnext->name = 0xDEADBEEF;
					S.push(nextnext);
				}
			}

			delete next; //since there are no cycles we can just delete here
		}
	}
}

bool AutoState::DeleteString(const char *str) //deletes the end of a string that starts from this unless it is not a leaf
{
	if (str == NULL)
		return false;

	AutoState *currentState = this;
	int i;

	for (i = 0; str[i + 1] != '\0'; i++)
	{
		AutoState *next = currentState->MakeTransition(str[i]); //see if a transition from P to Q labeled str[i] exists

		if (next == NULL) //if not...
			return false;

		currentState = next;
	}

	AutoState *target = currentState->MakeTransition(str[i]);

	if (target == NULL) //final transition does not exist, so string doesn either
		return false;

	else if (target->alpha != 0) //can't delete because it's not a leaf
		return false;

	else //ok to delete
	{
		currentState->transit.remove(target);
		currentState->alpha &= ~(1ULL << (str[i] - '0'));
		delete target;
	}

	return true;
}

bool AutoState::AddTransition(char newlabel, AutoState **result) //add transition with symbol 'newlabel' form P to new state Q
{
	using namespace std;

	if (IsValidName(newlabel) || IsNumber(newlabel))
		newlabel -= '0'; //offset the bits;

	else return false;

	if ((alpha & (1ULL << newlabel)) != 0) //the transition already exists
		return false;

	//add a new trasntion d(S, q)
	AutoState *nextState = new AutoState(newlabel + '0', name + 1);
	transit.push_back(nextState);

	alpha |= (1ULL << newlabel); //mark the flag for the label

	if (result != NULL)
	*result = nextState;

	return true;
}

AutoState *AutoState::MakeTransitionExt(const char *str) //make n transitions
{
	AutoState *currentState = this;

	for (int j = 0; str[j] != '\0'; j++)
	{
		if (currentState == NULL)
			return NULL;

		if (currentState->HasTransition(str[j]))
		{
			for (std::list<AutoState*>::const_iterator i = currentState->transit.begin(); i != currentState->transit.end(); i++)
			{
				if ((*i)->GetLabel() == str[j])
				{
					currentState = *i;
					break;
				}
			}
		}
	}

	unsigned int last = strlen(str);

	if (currentState != NULL && currentState->GetLabel() != str[last - 1])
		return NULL;

	return currentState;
}

AutoState *AutoState::MakeTransition(char tlabel) //makes a single transition from P to Q with symbol tlabel
{
	if (!HasTransition(tlabel))
		return NULL;

	//stl iterator to go through whole list
	for (std::list<AutoState*>::const_iterator i = transit.begin(); i != transit.end(); i++)
	{
		if ((*i)->label == tlabel)
			return *i;
	}

	return NULL; //error
}

AutoState *AutoState::StringResult(const char *str)
{
	if (str == NULL)
		return NULL;

	AutoState *currentState = this;

	for (int i = 0; str[i] != '\0'; i++)
	{
		AutoState *next = currentState->MakeTransition(str[i]); //see if a transition from P to Q labeled str[i] exists

		if (next == NULL) //if not...
			return NULL;

		currentState = next;
	}
	
	if (currentState->label == 0)
		return NULL;

	return currentState;
}

AutoState *AutoState::AddString(const char *str)
{
	AutoState *currentState = this;

	for (int i = 0; str[i] != '\0'; i++)
	{
		if (currentState == NULL)
			return NULL;

		AutoState *next = currentState->MakeTransition(str[i]); //see if a transition from P to Q labeled str[i] exists

		if (next == NULL) //if not...
			currentState->AddTransition(str[i], &next); //add transition with symbol str[i] and move to new state

		currentState = next;
	}

	return currentState;
}