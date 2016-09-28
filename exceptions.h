#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "types.h"

class Exceptions{
public:
	Exceptions(string message = "lol cats)", int isFatal = true);

	string message;
	bool isFatal;
};

void ensureExp(bool exp, int isFatal = true, string m1 = "lol cats)", string m2 = "", string m3 = "");

#endif // EXCEPTIONS_H
