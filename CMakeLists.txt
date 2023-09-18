# CMakeList.txt: проект CMake для microprocessorsCmake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("microprocessorsCmake")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (microprocessorsCmake "microprocessorsCmake.cpp" "JsonParametresReader.cpp" "JsonParametresReader.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
