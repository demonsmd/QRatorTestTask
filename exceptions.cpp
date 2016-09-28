#include "exceptions.h"

Exceptions::Exceptions(string message, int isFatal){
	this->message = message;
	this->isFatal = isFatal;
}

void ensureExp(bool exp, int isFatal, string m1, string m2, string m3){
	string message = m1 + m2 + m3;
	if (!exp)
		throw Exceptions(message, isFatal);
}
