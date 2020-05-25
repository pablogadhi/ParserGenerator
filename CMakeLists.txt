cmake_minimum_required(VERSION 3.0.0)
project(RegexEvaluator VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

link_libraries(gvc cgraph)
add_executable(ParserGenerator parser_generator.cpp nfa_builder.cpp dfa_builder.cpp state_machine.cpp state.cpp tree_node.cpp simulation.cpp scanner.cpp parser.cpp s_core_generator.cpp error.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)