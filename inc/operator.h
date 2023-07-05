#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#include "heads.h"
typedef Value* (*Unary)(Value*);

extern std::map<char,Unary> UnaryOperate;

Value* Not(Value* val);

Value* Minus(Value* val);

Value* plusplus(Value* val);

void InitializeUnaryOperate();

#endif