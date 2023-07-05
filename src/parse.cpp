#include "../inc/parse.h"
#include "../inc/lexical.h"

void DeclExprAST::addInit(std::unique_ptr<SimpExprAST> init){
  Init.push_back(std::move(init));
}

void WhileExprAST::output(){
  std::cout<<"( (condition:";
  Condition->output();
  std::cout<<") body {";
  for(size_t i = 0 ; i < Body.size(); ++i){
    Body[i]->output();
  }
  std::cout<<"}"<<std::endl;
}

void UnaryExprAST::output(){
    std::cout<<"( UnaryOp "<< Opcode <<" on ";
    Operand->output();
    std::cout<<") "<<std::endl;
}

void IfExprAST::output(){
    std::cout<<"IF condition( ";
    Condition->output();
    std::cout<<")\nbody{"<<std::endl;
    for(int i = 0 ; i < If.size(); ++i){
      std::cout<<"\t";
      If[i]->output();
      std::cout<<std::endl;
    }
    std::cout<<"}\n";
}

void NumberDoubleExprAST::output(){
    std::cout << "(DoubleVal: " << Val << ")";
}

void NumberIntExprAST::output(){
    std::cout << "(IntVal: " << Val << ")";
}

void NumberCharExprAST::output(){
    std::cout << "(CharVal: " << Val << ")";
}

void VariableExprAST::output(){
    std::cout << "(Variable: " << Name << ")";
}

void SimpExprAST::output(){
    std::cout<<"set "<<Identi<<" as ";
    Expr->output();
}

void BinaryExprAST::output(){
    std::cout << "(";
    LHS->output();
    std::cout << Op;
    RHS->output();
    std::cout << ")";
}

void CallExprAST::output(){
    std::cout << "(Call: " << Callee << " (Args: ";
    for(auto &Arg : Args){
      Arg->output();
    }
    std::cout << "))";
}

void ReturnExprAST::output(){
    std::cout<<"return ";
    Ret->output();
}

void DeclExprAST::output(){
    std::cout<<"Define "<<type<<": ";
    for(auto it:Identi) std::cout<<it<<" ";
    std::cout<<std::endl;
}

void PrototypeAST::output(){
    std::cout << "(Function: " << Name << std::endl << "(Args:";
    for(auto &Arg : Args){
      std::cout << " " << Arg;
    }
    std::cout << ")" << std::endl << "(ArgTypes:";
    for(auto ArgType : ArgTypes){
      std::cout << " " << ArgType;
    }
    std::cout << ")" << std::endl << "(FnType: " << FnType << "))" << std::endl;
}

void FunctionAST::output(){
    Proto->output();
    std::cout<<std::endl;
    for(int i = 0 ; i < Body.size(); ++i){
      Body[i]->output();
    }
}

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results.
static std::unique_ptr<ExprAST> OperatorLeft;
static std::unique_ptr<ExprAST> OperatorRight;
// const char Pesu = 100;
int getNextToken() { return CurTok = gettok(); }
/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.

/// LogError* - These are little helper functions for error handling.
/// you can add additional function to help you log error. 
std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

std::unique_ptr<FunctionAST> LogErrorF(const char *Str) {
  LogError(Str);
  return nullptr;
}


/*TODO: Finish the Parse*() function to implement the Parser.
  We provide some implemented Parse* function for reference, like 
  ParseNumberExpr(), ParseExtern(), which are marked with "example", and you can use these functions directly. 
  >>>note: You can add some other Parse*() function to help you achieve this goal,
  >>>e.g. ParseParenExpr() which parenexpr ::= '(' expression ')'.
*/
static std::unique_ptr<ExprAST> ParseExpression();
static std::unique_ptr<ExprAST> ParseExpressionPrefix();
static std::unique_ptr<ExprAST> ParseStatement();


int CurTok;
std::map<char, int> BinopPrecedence;
//====================if is here!!!!=================================
static std::unique_ptr<ExprAST> ParseIfExpr(){
  getNextToken();
  auto condition = ParseExpression();
  std::unique_ptr<IfExprAST> IfExpr = std::make_unique<IfExprAST>(std::move(condition));

  // getNextToken();
  if(CurTok!='{'){
    LogErrorP("Except \"{\" in If");
    exit(-1);
  }
  getNextToken();
  while(CurTok != '}'){
    auto expr = ParseStatement();
    IfExpr->AddIfStatement(std::move(expr));
    // if(CurTok!=';'){ LogErrorP("Except ';
  }
  getNextToken();
  if(CurTok == tok_else){
    getNextToken();
    if(CurTok!='{'){
      LogErrorP("Except \"}\" in if");
      exit(-1);
    }
    getNextToken();
    while(CurTok != '}'){
      auto expr = ParseStatement();
      IfExpr->AddElseStatement(std::move(expr));
      if(CurTok!=';'){ LogErrorP("Except ';'"); exit(-1);}
      getNextToken();
    }
    getNextToken();
  }
  return std::move(IfExpr);
}

//====================while is here !!!!=============================
static std::unique_ptr<ExprAST> ParseWhileExpr(){
  getNextToken();
  auto condition = ParseExpression();
  // getNextToken();
  std::unique_ptr<WhileExprAST> WhileExpr = std::make_unique<WhileExprAST>(std::move(condition));
  if(CurTok!='{'){
    LogErrorP("Except \"{\"");
    exit(-1);
  }
  getNextToken();
  while(CurTok != '}'){
    WhileExpr->AddStatement(ParseStatement());
  }
  getNextToken();

  return std::move(WhileExpr);
}

/// numberexpr ::= number
/// example
static std::unique_ptr<ExprAST> ParseNumberExpr(int NumberType) {
  if (NumberType == type_double){
    auto Result = std::make_unique<NumberDoubleExprAST>(NumValD);
    getNextToken(); // consume the number
    return std::move(Result);
  }
  else{
    auto Result = std::make_unique<NumberIntExprAST>(NumValI);
    getNextToken(); // consume the number
    return std::move(Result);
  }
}


/// identifierexpr
/// <ident> or <callee>
/// TODO
static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  if (CurTok != tok_identifier)
    return LogError("Excepted identifier in Identifier");
  
  getNextToken(); // maybe get '('
  if(CurTok == '('){
    auto FunctionName = IdentifierStr;
    std::vector<std::unique_ptr<ExprAST>> args(0);
    getNextToken();

    if(CurTok != ')'){

      auto arg = ParseExpression();
      args.push_back(std::move(arg));
      // getNextToken();
      while(CurTok == ','){
        getNextToken();
        arg = ParseExpression();
        args.push_back(std::move(arg));
      }
    }
    if(CurTok != ')')
      return LogError("Excepted ')' in calleeExpr");

    getNextToken();
    return std::make_unique<CallExprAST>(std::move(FunctionName), std::move(args));

  }else{
    auto Result = std::make_unique<VariableExprAST>(std::move(IdentifierStr));
    return std::move(Result);
  }
}

static std::unique_ptr<ExprAST> ParseExpressionPrime(std::stack<char> &operators, std::stack<std::unique_ptr<ExprAST>> &exprs) {
  if( isBinOp(CurTok) ){
    char op = CurTok;
    std::unique_ptr<ExprAST> expr = nullptr;
    if(!operators.empty()){
      char popOp = operators.top();
      while(BinopPrecedence[popOp] >= BinopPrecedence[op]){
        operators.pop();
        auto RHS = std::move(exprs.top());
        exprs.pop();
        auto LHS = std::move(exprs.top());
        exprs.pop();

        exprs.push(std::move(std::make_unique<BinaryExprAST>(
          popOp, std::move(LHS), std::move(RHS)
        )));
        if(operators.empty()) break;
        popOp = operators.top();
      }
      operators.push(op);
    }else{
      operators.push(op);
    }
    getNextToken();
    exprs.push(std::move(ParseExpressionPrefix()));
    // getNextToken();
    auto exprPrime = std::move(ParseExpressionPrime(operators, exprs));

    // reach the end, return 
    if(exprPrime == nullptr){
      while(!operators.empty()){
        char popOp = operators.top();
        operators.pop();
        auto RHS = std::move(exprs.top());
        exprs.pop();
        auto LHS = std::move(exprs.top());
        exprs.pop();
        auto result = std::make_unique<BinaryExprAST>(popOp, std::move(LHS), std::move(RHS));
        exprs.push(std::move(result));
      }
      return std::move(exprs.top());
    }else{
      return std::move(exprPrime);
    }
  }
  // return std::move(std::make_unique<VoidExprAST>());
  return nullptr;
}

static std::unique_ptr<ExprAST> ParseExpressionPrefix(){
  std::unique_ptr<ExprAST> exp = nullptr;
  auto zero = std::make_unique<NumberIntExprAST>(0);


  if( isUnaryOp(CurTok) ){
    int op = CurTok;
    getNextToken();
    return std::make_unique<UnaryExprAST>(op,ParseExpressionPrefix());
  }

  switch(CurTok){
    case (int)'(':
      getNextToken();
      exp = ParseExpression();
      if(CurTok != ')')
        LogError("excepted ')' in expression with '('");
      getNextToken();
      return std::move(exp);

    case tok_number_int:
      getNextToken();
      return std::move(std::make_unique<NumberIntExprAST>(NumValI));

    case tok_number_double:
      getNextToken();
      return std::move(std::make_unique<NumberDoubleExprAST>(NumValD));

    case tok_number_char:
      getNextToken();
      return std::move(std::make_unique<NumberCharExprAST>(NumValC));

    case tok_identifier:
      return std::move(ParseIdentifierExpr());
    
    default:
      return nullptr;
  }
}

/// expression
/// <exp>
/// TODO
static std::unique_ptr<ExprAST> ParseExpression() {
  auto expr = ParseExpressionPrefix();
  std::stack<char> operators;
  std::stack<std::unique_ptr<ExprAST>> exprs;
  exprs.push(std::move(expr));
  auto exprPrime = std::move(ParseExpressionPrime(operators, exprs));
  if(exprPrime == nullptr){
    return std::move(exprs.top());
  }
  else{
    // exprs.pop();
    return std::move(exprPrime);
  }
}

static std::unique_ptr<SimpExprAST> ParseSimp(){
  std::string variable = IdentifierStr;
  getNextToken();
  if(CurTok == ',' || CurTok == ';')
    return nullptr;
  if(CurTok != '='){
    LogErrorP("Except '=' in SimpExpr");
    exit(-1);
  }
  getNextToken();
  auto expr = ParseExpression();
  return std::make_unique<SimpExprAST>(variable, std::move(expr));
}

static std::unique_ptr<DeclExprAST> ParseDecl(){
  // int type = ValType;
  std::vector<std::string> Identi;
  std::vector<std::unique_ptr<SimpExprAST>> Init(0);
  do{
    getNextToken();
    Identi.push_back(IdentifierStr);
    auto init = ParseSimp();
    if(init != nullptr)
      Init.push_back(std::move(init));
    // getNextToken();
  }while(CurTok == ',');
  return std::make_unique<DeclExprAST>(ValType, Identi, std::move(Init));
}

static std::unique_ptr<ReturnExprAST> ParseReturn(){
  getNextToken();
  auto E = ParseExpression();
  return std::make_unique<ReturnExprAST>(std::move(E));
}


static std::unique_ptr<ExprAST> ParseForExpr(){
  getNextToken();
  if(CurTok != '('){
    LogErrorP("Expect '(' in for");
    exit(-1);
  }
  getNextToken();
  auto Per = ParseStatement();

  auto cond = ParseExpression();

  if(CurTok != ';'){
    LogErrorP("Expect ';' after condition in for");
    exit(-1);
  }
  getNextToken();
  auto afterEachLoop = ParseSimp();

  if(CurTok != ')'){
    LogErrorP("Expect ')' in for");
    exit(-1);
  }
  getNextToken();
  if(CurTok != '{'){
    LogErrorP("Expect '{' in for");
    exit(-1);
  }

  auto whilePart = std::make_unique<WhileExprAST>(std::move(cond));
  getNextToken();
  while(CurTok != '}'){
    auto stst = ParseStatement();
    whilePart->AddStatement(std::move(stst));
  }
  whilePart->AddStatement(std::move(afterEachLoop));

  getNextToken();

  return std::make_unique<ForExprAST>(std::move(Per),std::move(whilePart));
}


/// statement 
/// <stmt>
/// example
static std::unique_ptr<ExprAST> ParseStatement() {
  if(CurTok == tok_return){
    auto ret = ParseReturn();
    if(CurTok!=';'){
      LogErrorF("Expect ';' in statement");
      exit(-1);
    }
    getNextToken();
    return std::move(ret);

  }else if(CurTok == tok_def){
    auto ret =  ParseDecl(); 
    if(CurTok!=';'){
      LogErrorF("Expect ';' in statement");
      exit(-1);
    }
    getNextToken();
    return std::move(ret);

  }else if(CurTok == tok_identifier){
    auto ret =  ParseSimp();
    if(CurTok!=';'){
      LogErrorF("Expect ';' in statement");
      exit(-1);
    }
    getNextToken();
    return std::move(ret);

  }else if(CurTok == tok_if){
    return ParseIfExpr();
  }else if(CurTok == tok_while){
    return ParseWhileExpr();
  }else if(CurTok == tok_for){
    return ParseForExpr();
  }else if(CurTok == ';'){
    getNextToken();
    return make_unique<VoidExprAST>();
  }else{
    exit(-1);
  }
}

/// prototype
/// <prototype>
/// an imcomplete parse function. It can parse <prototype> without args.
/// TODO
static std::unique_ptr<PrototypeAST> ParsePrototype() {
  int FnType = ValType;
  getNextToken(); // eat ValType

  if (CurTok != tok_identifier)
    return LogErrorP("Expected function name in prototype");
  
  std::string FnName = IdentifierStr;
  getNextToken(); // eat FnName

  if (CurTok != '(')
    return LogErrorP("Expected '(' in prototype");
  getNextToken(); // eat '('.

  std::vector<std::string> ArgNames;
  std::vector<int> ArgTypes;

  if (CurTok != ')'){
    
    if(CurTok != tok_def)
      return LogErrorP("except type of para in paramlist");
    ArgTypes.push_back(ValType);
    getNextToken();
    if( CurTok != tok_identifier)
      return LogErrorP("except identifier of para in paramlist");
    ArgNames.push_back(IdentifierStr);
    getNextToken();
    
    while( CurTok==','){
      getNextToken();
      if(CurTok != tok_def)
        return LogErrorP("except type of para in paramlist");
      ArgTypes.push_back(ValType);
      getNextToken();
      if( CurTok != tok_identifier)
        return LogErrorP("except identifier of para in paramlist");
      ArgNames.push_back(IdentifierStr);
      getNextToken();
    }
  }
    // return LogErrorP("Expected ')' in prototype");

  // success.
  getNextToken(); // eat ')'.

  return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames), std::move(ArgTypes), FnType);
}

/// definition ::= 'def' prototype expression
/// <function>
/// TODO
std::unique_ptr<FunctionAST> ParseDefinition() {

  auto Proto = ParsePrototype();
  auto result = std::make_unique<FunctionAST>(std::move(Proto));
  if(CurTok != '{')
    return LogErrorF("Excepted '{' in function's body");
  
  getNextToken();

  std::unique_ptr<ExprAST> BodyItem = nullptr;
  // std::vector<std::unique_ptr<ExprAST>> Body;
  while(CurTok!='}'){
    BodyItem = ParseStatement();
    // getNextToken();
    result->AddStatement(std::move(BodyItem));
  }
  getNextToken();
  return std::move(result);
}

/// external ::= 'extern' prototype
/// <gdecl>
/// example
std::unique_ptr<PrototypeAST> ParseExtern() {
  int isdef = getNextToken(); // eat extern.s
  if (isdef != tok_def)
    return LogErrorP("Expected type declaration");
  auto Proto = ParsePrototype();
  if (CurTok != ';')
    return LogErrorP("Expected ';' in global declaration");
  getNextToken(); // eat ';'
  return Proto;
}
