#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "lang.h"
#include "visitor.h"

// Count various structural features
class StructureCounter : public Visitor<int> {
 public:
  int numFunctions = 0;
  int numIfs = 0;
  int numFors = 0;
  int numReturns = 0;
  int numCalls = 0;
  int numSets = 0;
  int totalDepth = 0;
  int maxDepth = 0;
  int currentDepth = 0;
  
  int visitProgram(Program *node) override {
    numFunctions = node->body.size();
    int total = 0;
    for (auto func : node->body) {
      total += visitFunctionDeclaration(func);
    }
    return total;
  }
  
  int visitFunctionDeclaration(FunctionDeclaration *node) override {
    return visitStatement(node->body);
  }

  int visitExpressionStatement(ExpressionStatement *node) override {
    return visitExpression(node->expr);
  }
  
  int visitSetStatement(SetStatement *node) override {
    numSets++;
    return visitExpression(node->value) + 1;
  }
  
  int visitIfStatement(IfStatement *node) override {
    numIfs++;
    currentDepth++;
    maxDepth = std::max(maxDepth, currentDepth);
    totalDepth += currentDepth;
    int result = visitExpression(node->condition) + visitStatement(node->body) + 1;
    currentDepth--;
    return result;
  }
  
  int visitForStatement(ForStatement *node) override {
    numFors++;
    currentDepth++;
    maxDepth = std::max(maxDepth, currentDepth);
    totalDepth += currentDepth;
    int result = visitStatement(node->init) + visitExpression(node->test) + 
                 visitStatement(node->update) + visitStatement(node->body) + 1;
    currentDepth--;
    return result;
  }
  
  int visitBlockStatement(BlockStatement *node) override {
    int l = 0;
    for (auto stmt : node->body) {
      l += visitStatement(stmt);
    }
    return l;
  }
  
  int visitReturnStatement(ReturnStatement *node) override { 
    numReturns++;
    return 1; 
  }

  int visitIntegerLiteral(IntegerLiteral *node) override { return 1; }
  int visitVariable(Variable *node) override { return 1; }
  
  int visitCallExpression(CallExpression *node) override {
    numCalls++;
    int l = 1;
    for (auto expr : node->args) {
      l += visitExpression(expr);
    }
    return l;
  }
};

struct Features {
  int length;
  int numFunctions;
  int numIfs;
  int numFors;
  int numReturns;
  int numCalls;
  int numSets;
  int maxDepth;
  double avgDepth;
};

Features extractFeatures(Program *p) {
  StructureCounter counter;
  int len = counter.visitProgram(p);
  Features f;
  f.length = len;
  f.numFunctions = counter.numFunctions;
  f.numIfs = counter.numIfs;
  f.numFors = counter.numFors;
  f.numReturns = counter.numReturns;
  f.numCalls = counter.numCalls;
  f.numSets = counter.numSets;
  f.maxDepth = counter.maxDepth;
  f.avgDepth = (f.numIfs + f.numFors > 0) ? 
               (double)counter.totalDepth / (f.numIfs + f.numFors) : 0.0;
  return f;
}

double computeSimilarity(const Features& f1, const Features& f2) {
  double score = 0.5;
  
  // Length difference
  int lenDiff = abs(f1.length - f2.length);
  double lenSim = 1.0 - std::min(1.0, lenDiff / 50.0);
  
  // Structure similarity
  int structDiff = abs(f1.numFunctions - f2.numFunctions) * 5 +
                   abs(f1.numIfs - f2.numIfs) +
                   abs(f1.numFors - f2.numFors) +
                   abs(f1.numReturns - f2.numReturns);
  double structSim = 1.0 - std::min(1.0, structDiff / 30.0);
  
  // Call/Set pattern similarity
  int patternDiff = abs(f1.numCalls - f2.numCalls) + abs(f1.numSets - f2.numSets);
  double patternSim = 1.0 - std::min(1.0, patternDiff / 40.0);
  
  // Depth similarity
  int depthDiff = abs(f1.maxDepth - f2.maxDepth);
  double depthSim = 1.0 - std::min(1.0, depthDiff / 5.0);
  
  // Weighted combination
  double similarity = 0.3 * lenSim + 0.35 * structSim + 0.2 * patternSim + 0.15 * depthSim;
  
  // Map to [0, 1] with 0.5 as baseline
  score = 0.5 + 0.5 * similarity;
  
  return std::max(0.0, std::min(1.0, score));
}

int main() {
  // Read two programs
  Program *prog1 = scanProgram(std::cin);
  Program *prog2 = scanProgram(std::cin);
  // Read sample input
  std::string input;
  int c;
  while ((c = std::cin.get()) != EOF) {
    input += c;
  }

  // Extract features
  Features f1 = extractFeatures(prog1);
  Features f2 = extractFeatures(prog2);
  
  // Compute similarity
  double similarity = computeSimilarity(f1, f2);
  
  std::cout << similarity << std::endl;
  return 0;
}
