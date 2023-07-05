#include "../inc/operator.h"
#include "../inc/codegen.h"
#include "../inc/lexical.h"


std::map<char,Unary> UnaryOperate;

Value* Not(Value* val){
    return Builder->CreateNot(val,"nottmp");
}

Value* Sub(Value* val){
    if(val->getType()->isDoubleTy()){
        return Builder->CreateFMul(val,ConstantFP::get(*TheContext, APFloat(-1.0)),"minustmp");
    }else if(val->getType()->isIntegerTy(8)){
        return Builder->CreateMul(val,ConstantInt::get(*TheContext, APInt(8,-1)),"minustmp");
    }else if(val->getType()->isIntegerTy(32)){
        return Builder->CreateMul(val,ConstantInt::get(*TheContext, APInt(32,-1)),"minustmp");
    }else{
        return nullptr;
    }
}

Value* PPlus(Value* val){
    if(val->getType()->isDoubleTy()){
        return Builder->CreateFAdd(val,ConstantFP::get(*TheContext, APFloat(1.0)),"minustmp");
    }else if(val->getType()->isIntegerTy(8)){
        return Builder->CreateAdd(val,ConstantInt::get(*TheContext, APInt(1,8)),"minustmp");
    }else if(val->getType()->isIntegerTy(32)){
        return Builder->CreateFAdd(val,ConstantInt::get(*TheContext, APInt(1,32)),"minustmp");
    }else{
        return nullptr;
    }
}

Value* SSub(Value* val){
    if(val->getType()->isDoubleTy()){
        return Builder->CreateFSub(val,ConstantFP::get(*TheContext, APFloat(1.0)),"minustmp");
    }else if(val->getType()->isIntegerTy(8)){
        return Builder->CreateSub(val,ConstantInt::get(*TheContext, APInt(1,8)),"minustmp");
    }else if(val->getType()->isIntegerTy(32)){
        return Builder->CreateFSub(val,ConstantInt::get(*TheContext, APInt(1,32)),"minustmp");
    }else{
        return nullptr;
    }
}

void InitializeUnaryOperate(){
    UnaryOperate['-'] = Sub;
    UnaryOperate[doubleAdd] = PPlus;
    UnaryOperate[doubleMinus] = SSub;
    UnaryOperate['!'] = Not;
}