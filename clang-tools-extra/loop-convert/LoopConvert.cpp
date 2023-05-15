#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "nlohmann/json.hpp"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using json = nlohmann::json;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  explicit MyASTVisitor(ASTContext *Context)
      : Context(Context) {}

  bool VisitDecl(Decl *D) {
    SourceManager &SM = Context->getSourceManager();
    if (SM.isInMainFile(D->getLocation())) {
      json j;
      j["kind"] = D->getDeclKindName();
      j["start"]["line"] = SM.getSpellingLineNumber(D->getBeginLoc());
      j["start"]["column"] = SM.getSpellingColumnNumber(D->getBeginLoc());
      j["start"]["offset"] = SM.getFileOffset(D->getBeginLoc());
      j["end"]["line"] = SM.getSpellingLineNumber(D->getEndLoc());
      j["end"]["column"] = SM.getSpellingColumnNumber(D->getEndLoc());
      j["end"]["offset"] = SM.getFileOffset(D->getEndLoc());

      // for (const auto &child : D->children()) {
      //   j["children"].push_back(processDecl(child));
      // }
      // D->dump();
      llvm::outs() << j.dump(4) << "\n";
    }
    return true;
  }

private:
  json processDecl(const Decl *D) {
    json j;
    SourceManager &SM = Context->getSourceManager();
    j["kind"] = D->getDeclKindName();
    j["start"]["line"] = SM.getSpellingLineNumber(D->getBeginLoc());
    j["start"]["column"] = SM.getSpellingColumnNumber(D->getBeginLoc());
    j["start"]["offset"] = SM.getFileOffset(D->getBeginLoc());
    j["end"]["line"] = SM.getSpellingLineNumber(D->getEndLoc());
    j["end"]["column"] = SM.getSpellingColumnNumber(D->getEndLoc());
    j["end"]["offset"] = SM.getFileOffset(D->getEndLoc());
    j["name"] = D->getQualifiedNameAsString();

    // for (const auto &child : D->children()) {
    //   j["children"].push_back(processDecl(child));
    // }
    return j;
  }

  ASTContext *Context;
};

class MyASTConsumer : public ASTConsumer {
public:
  explicit MyASTConsumer(ASTContext *Context)
      : Visitor(Context) {}

  virtual void HandleTranslationUnit(ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  MyASTVisitor Visitor;
};

class MyFrontendAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                         StringRef file) {
    return std::unique_ptr<ASTConsumer>(new MyASTConsumer(&CI.getASTContext()));
  }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
  auto optParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!optParser) {
    llvm::errs() << optParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = optParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
