#include "../inc/parse.h"
#include "../inc/codegen.h"
#include "../inc/lexical.h"
#include "../inc/operator.h"

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;
std::map<std::string, AllocaInst *> NamedValues;
std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
std::unique_ptr<legacy::FunctionPassManager> TheFPM;

int ReturnType;
Function *CurrentFunc = nullptr;
ExitOnError ExitOnErr;

Value *LogErrorV(const char *Str)
{
  LogError(Str);
  return nullptr;
}

Value *VariableExprAST::codegen()
{
  AllocaInst *A = NamedValues[Name];
  if (!A)
    return LogErrorV("Unknown variable name");
  // Load the value.
  return Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
}

Function *getFunction(std::string Name)
{
  // First, see if the function has already been added to the current module.
  if (auto *F = TheModule->getFunction(Name))
    return F;

  // If not, check whether we can codegen the declaration from some existing
  // prototype.
  auto FI = FunctionProtos.find(Name);
  if (FI != FunctionProtos.end())
    return FI->second->codegen();

  // If no existing prototype exists, return null.
  return nullptr;
}

/*TODO: Finish the codegen() function to implement the Code Generation part.
  We provide some implemented codegen function for reference, like
  NumberDoubleExprAST::codegen(), Value *VariableExprAST::codegen(), and you
  can use these functions directly.
*/
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          StringRef VarName, int type)
{
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  switch (type)
  {
  case type_int:
    return TmpB.CreateAlloca(Type::getInt32Ty(*TheContext), nullptr, VarName);
  case type_char:
    return TmpB.CreateAlloca(Type::getInt8Ty(*TheContext), nullptr, VarName);
  case type_double:
    return TmpB.CreateAlloca(Type::getDoubleTy(*TheContext), nullptr, VarName);
  default:
    return nullptr;
  }
}

// example of codegen()
Value *NumberDoubleExprAST::codegen()
{
  return ConstantFP::get(*TheContext, APFloat(Val));
}

// example of codegen()
Value *NumberIntExprAST::codegen()
{
  return ConstantInt::get(*TheContext, APInt(32, Val));
}

Value *NumberCharExprAST::codegen()
{
  return ConstantInt::get(*TheContext, APInt(8, Val));
}

int getType(Value *Val)
{
  if (Val->getType()->isDoubleTy())
    return type_double;
  if (Val->getType()->isIntegerTy(8))
    return type_char;
  if (Val->getType()->isIntegerTy(32))
    return type_int;
  return -1;
}


Value* lessThan(Value* L, Value* R){
  if (L->getType()->isDoubleTy() || R->getType()->isDoubleTy())
    {
      if (L->getType()->isIntegerTy())
        L = Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "tmpL");
      if (R->getType()->isIntegerTy())
        R = Builder->CreateUIToFP(R, Type::getDoubleTy(*TheContext), "tmpR");
      return Builder->CreateFCmpULT(L, R, "cmptmp");
    }
    else
    {
      if (getType(L) != getType(R))
      {
        if (getType(L) == type_char)
          L = Builder->CreateSExt(L, Type::getInt32Ty(*TheContext), "tmpL");
        else
          R = Builder->CreateSExt(R, Type::getInt32Ty(*TheContext), "tmpR");
      }
      return Builder->CreateICmpULT(L, R, "cmptmp");
    }
}

// example of codegen()
// To Do
Value *BinaryExprAST::codegen()
{
  Value *L = LHS->codegen();
  Value *R = RHS->codegen();

  if (!L || !R)
    return nullptr;

  switch (Op)
  {
  case '+':
  {
    if (L->getType()->isDoubleTy() || R->getType()->isDoubleTy())
    {
      if (L->getType()->isIntegerTy())
        L = Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "tmpL");
      if (R->getType()->isIntegerTy())
        R = Builder->CreateUIToFP(R, Type::getDoubleTy(*TheContext), "tmpR");
      return Builder->CreateFAdd(L, R, "addtmp");
    }
    else
    {
      if (getType(L) != getType(R))
      {
        if (getType(L) == type_char)
          L = Builder->CreateSExt(L, Type::getInt32Ty(*TheContext), "tmpL");
        else
          R = Builder->CreateSExt(R, Type::getInt32Ty(*TheContext), "tmpR");
      }
      return Builder->CreateAdd(L, R, "addtmp");
    }
  }
  case '-':
  {
    if (L->getType()->isDoubleTy() || R->getType()->isDoubleTy())
    {
      if (L->getType()->isIntegerTy())
        L = Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "tmpL");
      if (R->getType()->isIntegerTy())
        R = Builder->CreateUIToFP(R, Type::getDoubleTy(*TheContext), "tmpR");
      return Builder->CreateFSub(L, R, "subtmp");
    }
    else
    {
      if (getType(L) != getType(R))
      {
        if (getType(L) == type_char)
          L = Builder->CreateSExt(L, Type::getInt32Ty(*TheContext), "tmpL");
        else
          R = Builder->CreateSExt(R, Type::getInt32Ty(*TheContext), "tmpR");
      }
      return Builder->CreateSub(L, R, "subtmp");
    }
  }
  case '*':
  {
    if (L->getType()->isDoubleTy() || R->getType()->isDoubleTy())
    {
      if (L->getType()->isIntegerTy())
        L = Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "tmpL");
      if (R->getType()->isIntegerTy())
        R = Builder->CreateUIToFP(R, Type::getDoubleTy(*TheContext), "tmpR");
      return Builder->CreateFMul(L, R, "multmp");
    }
    else
    {
      if (getType(L) != getType(R))
      {
        if (getType(L) == type_char)
          L = Builder->CreateSExt(L, Type::getInt32Ty(*TheContext), "tmpL");
        else
          R = Builder->CreateSExt(R, Type::getInt32Ty(*TheContext), "tmpR");
      }
      return Builder->CreateMul(L, R, "multmp");
    }
  }
  case eq:
    return Builder->CreateAnd(Builder->CreateNot(lessThan(L,R),"tempGE"),Builder->CreateNot(lessThan(R,L),"tempGE"),"tempEQ");
  case ge:
    return Builder->CreateNot(lessThan(L,R),"tempGE");
  case le:
    return Builder->CreateNot(lessThan(R,L),"tempLE");
  case neq:
    return Builder->CreateNot(Builder->CreateAnd(Builder->CreateNot(lessThan(L,R),"tempGE"),Builder->CreateNot(lessThan(R,L),"tempGE"),"tempEQ"),"tempNot");
  case '<':
    return lessThan(L,R);
  case '>':
    return lessThan(R,L);
  case '&' :
    return Builder->CreateAnd(L,R,"tmpAnd");
  case '|' :
    return Builder->CreateOr(L,R, "tmpOr");
  }
  return nullptr;
}

// To Do
Value *CallExprAST::codegen()
{
  Function *CalleeF = TheModule->getFunction(Callee);
  if (!CalleeF)
    return LogErrorV("Unknown function referenced");

  if (CalleeF->arg_size() != Args.size())
    return LogErrorV("Incorrect # arguments passed");

  std::vector<Value *> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i)
  {

    ArgsV.push_back(Args[i]->codegen());
    if (!ArgsV.back())
      return nullptr;
  }
  return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

// an imcomplete codegen function. It can generate IR for prototype whose types of args
// and return value are all double.
Function *PrototypeAST::codegen()
{
  std::vector<Type *> Paras;
  for (size_t i = 0; i < Args.size(); ++i)
  {
    if (ArgTypes[i] == 1)
      Paras.push_back(Type::getInt32Ty(*TheContext));
    else if (ArgTypes[i] == 2)
      Paras.push_back(Type::getDoubleTy(*TheContext));
    else if(ArgTypes[i] == 3)
      Paras.push_back(Type::getInt8Ty(*TheContext));
    else
      LogErrorV("unkonwn value type");
  }

  FunctionType *FT;
  if (FnType == 1)
    FT = FunctionType::get(Type::getInt32Ty(*TheContext), Paras, false);
  else if(FnType == 2)
    FT = FunctionType::get(Type::getDoubleTy(*TheContext), Paras, false);
  else if(FnType == 3)
    FT = FunctionType::get(Type::getInt8Ty(*TheContext), Paras, false);

  Function *F =
      Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}

Value *DeclExprAST::codegen()
{
  for (auto VarName : Identi)
  {
    if (NamedValues.find(VarName) != NamedValues.end())
    {
      LogErrorP("redeclaretion of variable");
      exit(-1);
    }
    AllocaInst *Alloca = CreateEntryBlockAlloca(CurrentFunc, VarName, type);
    NamedValues[std::string(VarName)] = Alloca;
  }
  for(size_t i = 0 ; i < Init.size(); ++i )
    Init[i]->codegen();
  
  return nullptr;
}

Value *SimpExprAST::codegen()
{
  if (NamedValues.find(Identi) == NamedValues.end())
  {
    LogErrorP("undefined variable");
    exit(-1);
  }
  Value *temp = Expr->codegen();
  auto thisVal = NamedValues[Identi];
  int ValType = thisVal->getAllocatedType()->getTypeID();
  if (thisVal->getAllocatedType()->isIntegerTy() && temp->getType()->isDoubleTy())
  {
    if (thisVal->getAllocatedType()->isIntegerTy(8))
      temp = Builder->CreateFPToUI(temp, Type::getInt8Ty(*TheContext), "tmpV");
    else
      temp = Builder->CreateFPToUI(temp, Type::getInt32Ty(*TheContext), "tmpV");
  }
  else if (thisVal->getAllocatedType()->isDoubleTy() && temp->getType()->isIntegerTy())
  {
    temp = Builder->CreateUIToFP(temp, Type::getDoubleTy(*TheContext), "tmpV");
  }
  else if (thisVal->getAllocatedType()->isIntegerTy(8) && temp->getType()->isIntegerTy(32))
  {
    temp = Builder->CreateTrunc(temp, Type::getInt8Ty(*TheContext), "tmpV");
  }
  else if (thisVal->getAllocatedType()->isIntegerTy(32) && temp->getType()->isIntegerTy(8))
  {
    temp = Builder->CreateSExt(temp, Type::getInt32Ty(*TheContext), "tmpV");
  }
  Builder->CreateStore(temp, NamedValues[Identi]);
  return nullptr;
}

Value *WhileExprAST::codegen()
{
  Function *TheFunction = Builder->GetInsertBlock()->getParent();
  BasicBlock *LoopBB = BasicBlock::Create(*TheContext, "loop", TheFunction);

  Value *CondV = Condition->codegen();
  // if (!CondV)
  //   return nullptr;
  // if (CondV->getType()->isIntegerTy())
  //   CondV = Builder->CreateUIToFP(CondV, Type::getDoubleTy(*TheContext), "tmpCondV");
  // CondV = Builder->CreateFCmpONE(CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "whilecond");

  BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "whilecont");

  Builder->CreateCondBr(CondV, LoopBB, MergeBB);

  Builder->SetInsertPoint(LoopBB);
  for (size_t i = 0; i < Body.size(); ++i)
  {
    Body[i]->codegen();
  }
  CondV = Condition->codegen();
  if (!CondV)
    return nullptr;
  if (CondV->getType()->isIntegerTy())
    CondV = Builder->CreateUIToFP(CondV, Type::getDoubleTy(*TheContext), "tmpCondV");
  CondV = Builder->CreateFCmpONE(CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "whilecond");
  Builder->CreateCondBr(CondV, LoopBB, MergeBB);

  TheFunction->getBasicBlockList().push_back(MergeBB);
  Builder->SetInsertPoint(MergeBB);

  return nullptr;
}

Value *IfExprAST::codegen()
{
  Value *CondV = Condition->codegen();
  if (!CondV)
    return nullptr;

  // if (CondV->getType()->isIntegerTy())
  //   CondV = Builder->CreateUIToFP(CondV, Type::getDoubleTy(*TheContext), "tmpCondV");
  // CondV = Builder->CreateFCmpONE(CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "ifcond");
  Function *TheFunction = Builder->GetInsertBlock()->getParent();

  BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
  BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
  BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "ifcont");

  Builder->CreateCondBr(CondV, ThenBB, ElseBB);

  Builder->SetInsertPoint(ThenBB);
  for (size_t i = 0; i < If.size(); ++i)
    If[i]->codegen();

  Builder->CreateBr(MergeBB);
  ThenBB = Builder->GetInsertBlock();

  TheFunction->getBasicBlockList().push_back(ElseBB);

  Builder->SetInsertPoint(ElseBB);
  for (size_t i = 0; i < Else.size(); ++i)
    Else[i]->codegen();

  Builder->CreateBr(MergeBB);
  ElseBB = Builder->GetInsertBlock();

  TheFunction->getBasicBlockList().push_back(MergeBB);
  Builder->SetInsertPoint(MergeBB);
  return ConstantInt::get(*TheContext, APInt());
}

Value *ReturnExprAST::codegen()
{
  Value *RetVal = Ret->codegen();
  if (ReturnType == type_int && RetVal->getType()->isDoubleTy())
  {
    RetVal = Builder->CreateFPToUI(RetVal, Type::getInt32Ty(*TheContext), "tmpRet");
  }
  else if (ReturnType == type_char && RetVal->getType()->isDoubleTy())
  {
    RetVal = Builder->CreateFPToUI(RetVal, Type::getInt8Ty(*TheContext), "tmpRet");
  }
  else if (ReturnType == type_double && RetVal->getType()->isIntegerTy())
  {
    RetVal = Builder->CreateUIToFP(RetVal, Type::getDoubleTy(*TheContext), "tmpRet");
  }
  else if (ReturnType == type_int && RetVal->getType()->isIntegerTy(8))
  {
    RetVal = Builder->CreateSExt(RetVal, Type::getInt32Ty(*TheContext), "tmpRet");
  }
  else if (ReturnType == type_char && RetVal->getType()->isIntegerTy(32))
  {
    RetVal = Builder->CreateTrunc(RetVal, Type::getInt8Ty(*TheContext), "tmpRet");
  }
  Builder->CreateRet(RetVal);
  // Validate the generated code, checking for consistency.
  verifyFunction(*CurrentFunc);
  return RetVal;
}
// an imcomplete codegen function.
// You should finish the ToDo part in this function
Function *FunctionAST::codegen()
{
  // Transfer ownership of the prototype to the FunctionProtos map, but keep a
  // reference to it for use below.
  auto &P = *Proto;
  ReturnType = P.getReturnType();
  FunctionProtos[Proto->getName()] = std::move(Proto);
  Function *TheFunction = getFunction(P.getName());
  CurrentFunc = TheFunction;
  if (!TheFunction)
    return nullptr;

  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  NamedValues.clear();
  int index = 0;
  for (auto &Arg : TheFunction->args())
  {
    // Create an alloca for this variable.
    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName(), P.getArgTypes()[index++]);

    // Store initial value
    Builder->CreateStore(&Arg, Alloca);

    // Add argurements to variable symbol table.
    NamedValues[std::string(Arg.getName())] = Alloca;
  }

  for (size_t i = 0; i < Body.size(); ++i)
  {
    Body[i]->codegen();
  }
  return TheFunction;

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;
}

Value *UnaryExprAST::codegen()
{
  Value *val = Operand->codegen();
  return UnaryOperate[Opcode](val);
}

Value *ForExprAST::codegen(){
  PerTreatement->codegen();
  ForWhile->codegen();
  return nullptr;
}