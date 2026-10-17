cmake_minimum_required(VERSION 3.20)
project(riscv_core_verification)

# 1. Находим установленный в системе Verilator
find_package(verilator REQUIRED)

# 2. Находим установленный GTest
find_package(GTest REQUIRED)

# 3. Транслируем SystemVerilog в библиотеку C++ (включая поддержку VCD-трейсинга)
verilate(Vblink
    SOURCES src/blink.sv
    TRACE
)

# 4. Создаем исполняемый файл нашего тестбенча
add_executable(testbench tb/testbench.cpp)

# 5. Линкуем транслированную модель аппаратуры и GTest к нашему тестбенчу
target_link_libraries(testbench
    PRIVATE
    Vblink
    GTest::gtest_main
)
