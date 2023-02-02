cmake_minimum_required(VERSION 3.24)
project(polytech_graph_theory
        VERSION 1.0
        DESCRIPTION "Graph Theory course' at Polytechnic University Labs"
        LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
  set(CMAKE_CXX_STANDARD 17)

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

  # Docs only available if this is the main app
  #find_package(Doxygen)
  #if(Doxygen_FOUND)
  #  add_subdirectory(docs)
  #else()
  #  message(STATUS "Doxygen not found, not building docs")
  #endif()
endif()

add_subdirectory(L0)