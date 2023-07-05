#ifndef _CODE_GEN_
#define _CODE_GEN_

#include "heads.h"
#include "parse.h"
extern std::unique_ptr<LLVMContext> TheContext;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<IRBuilder<>> Builder;
extern std::map<std::string, AllocaInst *> NamedValues;
extern std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;

extern int ReturnType;

extern Function *CurrentFunc;
// std::unique_ptr<KaleidoscopeJIT> TheJIT;
extern ExitOnError ExitOnErr;

Value *LogErrorV(const char *Str);


#endif