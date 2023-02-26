project(lab0)

add_executable(lab0 main.cpp huffman.h huffman.cpp runlength.h runlength.cpp)
configure_file(alphabet.txt alphabet.txt COPYONLY)
