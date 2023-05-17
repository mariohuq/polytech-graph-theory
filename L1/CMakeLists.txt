project(lab1)
add_executable(lab1 main.cpp polya_dist.h polya_dist.cpp graphs.h graphs.cpp graphs.euler.cpp)
add_library(graphsLib STATIC polya_dist.h polya_dist.cpp graphs.h graphs.cpp graphs.euler.cpp)