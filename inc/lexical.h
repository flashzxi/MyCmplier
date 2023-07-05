#ifndef _LEXICAL_
#define _LEXICAL_

#include "heads.h"

enum Token {
  tok_eof = -1,

  // commands
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number_int = -5,
  tok_number_double = -6,
  tok_return = -7,
  tok_if = -8,
  tok_else = -9,
  tok_while = -10,
  tok_number_char = -11,
  tok_for = -12,
};

enum Op {
  eq = -100,
  le = -101,
  ge = -102,
  doubleAdd = -103,
  doubleMinus = -104,
  neq = -105,
};

enum Types {
  type_int = 1,
  type_double = 2,
  type_char = 3,
};

extern std::map<std::string, int> TypeValues;  //map typeString to int
extern FILE *fip;
extern std::string IdentifierStr; // Filled in if tok_identifier
extern int NumValI;             // Filled in if tok_number_int
extern double NumValD;             // Filled in if tok_number_double
extern char NumValC;
extern int ValType;               // Filled in if tok_number_double

int gettok();

void InitializeTypeValue();

bool isUnaryOp(int op);

bool isBinOp(int op);

#endif