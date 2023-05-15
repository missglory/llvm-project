set(LLVM_LINK_COMPONENTS support)

find_package(nlohmann_json REQUIRED)

include_directories(${nlohmann_json_INCLUDE_DIRS})

add_clang_executable(loop-convert
  LoopConvert.cpp
  )
target_link_libraries(loop-convert
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  nlohmann_json::nlohmann_json
  )

