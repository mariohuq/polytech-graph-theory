project(lab0)

add_executable(lab0 main.cpp huffman.h huffman.cpp)
configure_file(alphabet.txt alphabet.txt COPYONLY)
