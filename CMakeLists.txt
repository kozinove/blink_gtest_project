cmake_minimum_required(VERSION 3.20)
project(hardware_verification)

# 1. Поиск системных зависимостей
find_package(verilator REQUIRED)
find_package(GTest REQUIRED)

# 2. Создаем таргет (исполняемый файл тестбенча) ПЕРЕД вызовом verilate
add_executable(testbench tb/testbench.cpp)

# 3. Трансляция SystemVerilog в C++ и автоматическая привязка к таргету testbench
verilate(testbench
    SOURCES src/blink.sv
    TRACE
)

# 4. Линковка функции main() из фреймворка GTest
target_link_libraries(testbench
    PRIVATE
    GTest::gtest_main
)