#include <iostream>
#include <random>
#include <unordered_map>

#include "lang.h"
#include "transform.h"

class Cheat : public Transform {
 private:
  std::mt19937 rng;
  std::unordered_map<std::string, std::string> varMap;
  std::unordered_map<std::string, std::string> funcMap;
  int varCounter = 0;
  int funcCounter = 0;
  
  std::string generateVarName(const std::string& original) {
    if (varMap.count(original)) {
      return varMap[original];
    }
    const char* prefixes[] = {"var", "tmp", "val", "x", "y", "z", "data", "item", "elem", "node"};
    int prefixIdx = rng() % 10;
    std::string newName = std::string(prefixes[prefixIdx]) + std::to_string(varCounter++);
    varMap[original] = newName;
    return newName;
  }
  
  std::string generateFuncName(const std::string& original) {
    if (funcMap.count(original)) {
      return funcMap[original];
    }
    std::string newName = "func_" + std::to_string(funcCounter++);
    funcMap[original] = newName;
    return newName;
  }
  
 public:
  Cheat() : rng(std::random_device{}()) {}
  
  Variable *transformVariable(Variable *node) override {
    return new Variable(generateVarName(node->name));
  }
  
  FunctionDeclaration *transformFunctionDeclaration(FunctionDeclaration *node) override {
    std::string newName = node->name;
    if (node->name != "main") {
      newName = generateFuncName(node->name);
    }
    std::vector<Variable *> params;
    for (auto param : node->params) {
      params.push_back(transformVariable(param));
    }
    return new FunctionDeclaration(newName, params, transformStatement(node->body));
  }
  
  Expression *transformIntegerLiteral(IntegerLiteral *node) override {
    // Sometimes transform literals into expressions
    if (node->value > 1 && node->value < 100 && rng() % 4 == 0) {
      int a = rng() % node->value + 1;
      int b = node->value - a;
      return new CallExpression("+", {new IntegerLiteral(a), new IntegerLiteral(b)});
    }
    return new IntegerLiteral(node->value);
  }
};

int main() {
  auto code = scanProgram(std::cin);
  auto cheat = Cheat().transformProgram(code);
  std::cout << cheat->toString();
  return 0;
}
