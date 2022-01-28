#include "LoopQueue.h"
#include "read.h"

bool gt(int a, int b) //a > b
{
	return (a > b);
}

bool lt(int a, int b) // a < b
{
	return (a < b);
}

bool gteq(int a, int b)
{
	return (a >= b);
}

bool lteq(int a, int b)
{
	return (a <= b);
}

bool ne(int a, int b)
{
	return (a != b);
}

bool eq(int a, int b)
{
	return (a == b);
}

bool taut(int, int)
{
	return true;
}

LoopQueue::LoopQueue(const char *imax, int istride, int *i, const char *iop, bool itype) : stride(istride), iterator(i), bManagedIterator(itype), bLocked(false)
{
	maxExpr = imax;

	if (iop[1] == '=')
	{
		if (iop[0] == '>')
			condition = gteq;

		else if (iop[0] == '<')
			condition = lteq;

		else if (iop[0] == '!')
			condition = ne;

		else if (iop[0] == '=')
			condition = eq;

		else condition = taut; //error, terminate loop abnormally
	}

	else
	{
		if (iop[0] == '>')
			condition = gt;

		else if (iop[0] == '<')
			condition = lt;

		else condition = taut; //error, terminate loop abnormally
	}
}

LoopQueue::~LoopQueue()
{
	if (bManagedIterator)
		delete iterator;
}

void LoopQueue::Insert(const char *item)
{
	if (bLocked)
		return;

	std::string kstr;

	if (Compare(item, "LOOP", 4)) //skip loop expressions
		return;

	if (Compare(item, "ENDLOOP", 7))
	{
		kstr = "LOOPBACK"; //replace keyword with term only known to loop queue
		bLocked = true;
	}

	else kstr = item;

	Q.push(kstr);
}

std::string LoopQueue::Fetch()
{
	std::string kstr = Q.front();

	if (kstr == "LOOPBACK") //end of loop, so iterate by stride
		*iterator += stride;

	Q.pop();
	Q.push(kstr);

	return kstr;
}

bool LoopQueue::Terminated(int max)
{
	return !condition(*iterator, max);
}