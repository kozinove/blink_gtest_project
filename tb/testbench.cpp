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
        
        // Название дампа зависит от текущего теста
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
    EXPECT_EQ(dut->led, 0) << "LED must be 0 when reset is active";
}

TEST_F(BlinkTest, BlinkingBehavior) {
    dut->rst = 1;
    tick();
    dut->rst = 0; // Отпускаем сброс

    int last_led = dut->led;
    int toggle_count = 0;

    for (int i = 0; i < 10; i++) {
        tick();
        if (dut->led != last_led) {
            toggle_count++;
            last_led = dut->led;
        }
    }

    EXPECT_GT(toggle_count, 5) << "LED should toggle multiple times";
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
