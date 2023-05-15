#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  explicit MyASTVisitor(ASTContext *Context)
      : Context(Context) {}

  bool VisitDecl(Decl *D) {
    SourceManager &SM = Context->getSourceManager();
    if (SM.isInMainFile(D->getLocation())) {
      FullSourceLoc FullLocation = Context->getFullLoc(D->getBeginLoc());
      if (FullLocation.isValid())
        llvm::outs() << "Found declaration at "
                     << FullLocation.getSpellingLineNumber() << ":"
                     << FullLocation.getSpellingColumnNumber() << "\n";

      FullSourceLoc FullEndLocation = Context->getFullLoc(D->getEndLoc());
      if (FullEndLocation.isValid())
        llvm::outs() << "Declaration ends at "
                     << FullEndLocation.getSpellingLineNumber() << ":"
                     << FullEndLocation.getSpellingColumnNumber() << "\n";
    }

    return true;
  }

private:
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
  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}

