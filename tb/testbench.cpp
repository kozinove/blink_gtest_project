#include <gtest/gtest.h>
#include <memory>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vblink.h"

// Фикстура для тестов
class BlinkTest : public ::testing::Test {
protected:
    std::unique_ptr<Vblink> dut;
    std::unique_ptr<VerilatedVcdC> trace;
    vluint64_t sim_time;

    void SetUp() override {
        dut = std::make_unique<Vblink>();
        trace = std::make_unique<VerilatedVcdC>();
        Verilated::traceEverOn(true);
        
        // Название VCD-дампа зависит от имени текущего теста
        const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string vcd_name = std::string("dump_") + test_info->name() + ".vcd";
        
        dut->trace(trace.get(), 99);
        trace->open(vcd_name.c_str());
        
        sim_time = 0;
        dut->clk = 0;
        dut->rst = 1;
        dut->led = 0;
    }

    void TearDown() override {
        trace->close();
    }

    // Вспомогательный метод для симуляции одного такта (clock cycle)
    void tick() {
        dut->clk = 0;
        dut->eval();
        trace->dump(sim_time++);
        
        dut->clk = 1;
        dut->eval();
        trace->dump(sim_time++);
    }
};

TEST_F(BlinkTest, ResetBehavior) {
    dut->rst = 1;
    tick();
    tick();
    EXPECT_EQ(dut->led, 0) << "Ошибка: LED должен быть равен 0 при активном rst";
}

TEST_F(BlinkTest, BlinkingBehavior) {
    dut->rst = 1;
    tick();
    dut->rst = 0; // Снятие сигнала сброса

    int last_led = dut->led;
    int toggle_count = 0;

    for (int i = 0; i < 10; i++) {
        tick();
        if (dut->led != last_led) {
            toggle_count++;
            last_led = dut->led;
        }
    }

    EXPECT_GT(toggle_count, 5) << "Ошибка: LED не переключается ожидаемое количество раз";
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
