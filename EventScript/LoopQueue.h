#ifndef _LOOPQUEUE_H
#define _LOOPQUEUE_H
#include "libraries.h"
#include <queue>
class LoopQueue
{
private:
	std::queue<std::string> Q;
	std::string maxExpr;
	int *iterator;
	int stride;
	bool bManagedIterator;
	bool bLocked;
	std::string varI;

	bool(*condition)(int, int);

public:	
	inline std::string &GetMaxExpr() { return maxExpr; };

	void Insert(const char *);
	std::string Fetch();
	bool Terminated(int max);

	LoopQueue(const char *imax, int istride, int *i, const char *iop, bool itype);
	~LoopQueue();
};
#endif