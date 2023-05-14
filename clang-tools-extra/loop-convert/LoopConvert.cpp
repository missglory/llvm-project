#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  explicit MyASTVisitor(ASTContext *Context)
      : Context(Context) {}

  bool VisitDecl(Decl *D) {
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

int main(int argc, char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(new MyFrontendAction, argv[1]);
  }
}

