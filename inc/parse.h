#ifndef _PARSE_
#define _PARSE_

#include "heads.h"

extern int CurTok;

extern std::map<char, int> BinopPrecedence;


/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  virtual ~ExprAST() = default;

  virtual Value *codegen() = 0;
  virtual void output() { return; }
};

class UnaryExprAST : public ExprAST {
  char Opcode;
  std::unique_ptr<ExprAST> Operand;

public:
  UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand)
      : Opcode(Opcode), Operand(std::move(Operand)) {}

  Value *codegen() override;
  void output() override;
};

class IfExprAST: public ExprAST{
private:
  std::unique_ptr<ExprAST> Condition;
  std::vector<std::unique_ptr<ExprAST>> If;
  std::vector<std::unique_ptr<ExprAST>> Else;

public:
  IfExprAST(std::unique_ptr<ExprAST> Condition):Condition(std::move(Condition)){}
  void AddIfStatement(std::unique_ptr<ExprAST> row){ If.push_back(std::move(row) );}
  void AddElseStatement(std::unique_ptr<ExprAST> row){ Else.push_back(std::move(row) );}
  Value* codegen() override;
  void output() override;
};

class WhileExprAST: public ExprAST{
private:
  std::unique_ptr<ExprAST> Condition;
  std::vector<std::unique_ptr<ExprAST>> Body;

public:
  WhileExprAST(std::unique_ptr<ExprAST> Condition):Condition(std::move(Condition)){}
  void output() override;
  void AddStatement(std::unique_ptr<ExprAST> row){ Body.push_back(std::move(row) );}
  Value* codegen() override;
};

class ForExprAST: public ExprAST{
private:
  std::unique_ptr<WhileExprAST> ForWhile;
  std::unique_ptr<ExprAST> PerTreatement;

public:
  ForExprAST(std::unique_ptr<ExprAST> PerTreatement, std::unique_ptr<WhileExprAST> ForWhile)
    :PerTreatement(std::move(PerTreatement)), ForWhile(std::move(ForWhile))
  {};
  Value* codegen() override;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberDoubleExprAST : public ExprAST {
  double Val;

public:
  NumberDoubleExprAST(double Val) : Val(Val) {}
  void output() override;
  Value *codegen() override;
};

class NumberIntExprAST : public ExprAST {
  int Val;

public:
  NumberIntExprAST(int Val) : Val(Val) {}
  void output() override;
  Value *codegen() override;
};

class NumberCharExprAST : public ExprAST {
  char Val;

public :
  NumberCharExprAST(char Val) : Val(Val) {}
  void output() override;
  Value *codegen() override;
};
/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(const std::string &Name) : Name(Name) {}
  void output() override;
  Value *codegen() override;
};

class SimpExprAST : public ExprAST {
  std::string Identi;
  std::unique_ptr<ExprAST> Expr;
public:
  SimpExprAST(std::string Identi, std::unique_ptr<ExprAST> Expr):Identi(Identi),Expr(std::move(Expr)){}
  void output() override;
  Value *codegen() override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  Value *codegen() override;
  void output() override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}

  Value *codegen() override;
  void output() override;
};

class ReturnExprAST : public ExprAST {
  // int Type;
  std::unique_ptr<ExprAST> Ret;

public:
  ReturnExprAST(std::unique_ptr<ExprAST> Ret):Ret(std::move(Ret)){ }
  void output() override;
  Value *codegen() override;
};

class DeclExprAST: public ExprAST {
  int type;
  std::vector<std::string> Identi;
  std::vector<std::unique_ptr<SimpExprAST>> Init;

public:
  DeclExprAST(int type, std::vector<std::string> &Identi,std::vector<std::unique_ptr<SimpExprAST>> Init__)
    :type(type), Identi(Identi), Init(std::move(Init__)){ 
      // for(size_t i = 0 ; i < Init__.size(); ++i)
      //   Init.push_back(std::move(Init__[i]));
    }

  void addInit(std::unique_ptr<SimpExprAST> init);
  void output() override;
  Value *codegen() override;
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;
  std::vector<int> ArgTypes;
  int FnType;

public:
  PrototypeAST(const std::string &Name, std::vector<std::string> Args, std::vector<int> ArgTypes, int FnType)
      : Name(Name), Args(std::move(Args)), ArgTypes(std::move(ArgTypes)), FnType(FnType) {}

  Function *codegen();
  const std::string &getName() const { return Name; }
  const int getReturnType() {return FnType;}
  const std::vector<int> &getArgTypes() {return ArgTypes;}
  void output();
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::vector<std::unique_ptr<ExprAST>> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto):Proto(std::move(Proto)){}
  void AddStatement(std::unique_ptr<ExprAST> row){ Body.push_back(std::move(row) );}
  void output();
  const std::vector<int> &getArgsTypes(){ return Proto->getArgTypes(); }
  Function *codegen() ;
};

class VoidExprAST: public ExprAST{
public:
  VoidExprAST(){};
  Value* codegen() override{ return nullptr;}
  void output() override{ return ;}
};


int getNextToken();

std::unique_ptr<ExprAST> LogError(const char *Str) ;

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

std::unique_ptr<FunctionAST> LogErrorF(const char *Str);

std::unique_ptr<FunctionAST> ParseDefinition();

std::unique_ptr<PrototypeAST> ParseExtern();

#endif