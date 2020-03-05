cmake_minimum_required(VERSION 3.0.0)
project(RegexEvaluator VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(RegexEvaluator regex_evaluator.cpp nfa_builder.cpp dfa_builder.cpp state_machine.cpp state.cpp utils.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
