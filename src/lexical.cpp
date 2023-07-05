#include "../inc/lexical.h"

std::map<std::string, int> TypeValues;  //map typeString to int
FILE *fip;
std::string IdentifierStr; // Filled in if tok_identifier
int NumValI;             // Filled in if tok_number_int
double NumValD;             // Filled in if tok_number_double
char NumValC;
int ValType;               // Filled in if tok_number_double

void InitializeTypeValue(){
  TypeValues["int"] = 1;
  TypeValues["double"] = 2;
  TypeValues["char"] = 3;
}

bool isUnaryOp(int op){
  
  switch(op){
    case '-':
    case doubleMinus:
    case doubleAdd:
    case '!':
      return true;
  }
  return false;
}

bool isBinOp(int op){
  switch(op){
    case '-':
    case '+':
    case '*':
    case '/':
    case '&':
    case '|':
    case '<':
    case '>':
    case eq:
    case ge:
    case le:
    case neq:
      return true;

    default:
      return false;
  }
}

int longerMatch(int &curTok, int tarTok, int retTok){
  int elseTok = curTok;
  curTok = fgetc(fip);
  if(curTok == tarTok) {
    curTok = fgetc(fip);
    return retTok;
  }else
    return elseTok;
}

/// gettok - Return the next token from standard input.
int gettok() {
  static int LastChar = ' ';


  // Skip any whitespace.
  while (isspace(LastChar))
    LastChar = fgetc(fip);

  if(LastChar == '/'){
    LastChar = fgetc(fip);
    if(LastChar!='/') return '/';
    else{
      while(LastChar!='\n' && LastChar!= EOF)
        LastChar = fgetc(fip);
      LastChar = fgetc(fip);
    }
  }

  while (isspace(LastChar))
    LastChar = fgetc(fip);

  if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
    IdentifierStr = LastChar;
    while (isalnum((LastChar = fgetc(fip))))
      IdentifierStr += LastChar;

    if (IdentifierStr == "int" || IdentifierStr == "double" || IdentifierStr == "char"){
      ValType = TypeValues[IdentifierStr];
      return tok_def;
    }
    if (IdentifierStr == "extern")
      return tok_extern;
    
    if (IdentifierStr == "return")
      return tok_return;

    if (IdentifierStr == "if")
      return tok_if;
    
    if (IdentifierStr == "else")
      return tok_else;

    if (IdentifierStr == "while")
      return tok_while;

    if (IdentifierStr == "for")
      return tok_for;
      
    return tok_identifier;
  }

  if(LastChar == '\''){
    std::string NumStr;
    do{
      NumStr += LastChar;
      LastChar = fgetc(fip);
    }while(LastChar!='\'');
    NumStr += LastChar;
    LastChar = fgetc(fip);
    if(NumStr.length() == 3){
      NumValC = NumStr[1];
      return tok_number_char;
    }else{
      std::cout<<"' doesn't match"<<std::endl;
    }
  }
  
  if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = fgetc(fip);
    } while (isdigit(LastChar) || LastChar == '.');
    if(NumStr.find(".") != std::string::npos){
      NumValD = strtod(NumStr.c_str(), nullptr);
      return tok_number_double;
    }else{
      NumValI = atoi(NumStr.c_str());
      return tok_number_int;
    }
  }
  
  if(LastChar == '=')
    return longerMatch(LastChar, '=', eq);
  
  if(LastChar == '!')
    return longerMatch(LastChar, '=', neq);

  if(LastChar == '>')
    return longerMatch(LastChar, '=', ge);

  if(LastChar == '<')
    return longerMatch(LastChar, '=', le);

  // Check for end of file.  Don't eat the EOF.
  if (LastChar == EOF)
    return tok_eof;



  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = fgetc(fip);
  return ThisChar;

}