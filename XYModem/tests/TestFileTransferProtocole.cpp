#include "FileTransferProtocol.h"
#include "gtest/gtest.h"
#include <array>
#include <string_view>

namespace xymodem
{
struct FileTransferProtocoleDemo : public FileTransferProtocol
{
    FileTransferProtocoleDemo (std::shared_ptr<DeviceHandler> device,
                               const unsigned int& currentState,
                               std::shared_ptr<Logger> logger = std::make_shared<Logger>()) :
        FileTransferProtocol (device, currentState, logger){};

    // States
    static constexpr unsigned int state1 = 0;
    static constexpr unsigned int state2 = 1;
    static constexpr unsigned int state3 = 2;
    static constexpr unsigned int state4 = 3;
    static constexpr unsigned int state5 = 4;
    static constexpr unsigned int stateUnknown = 5;
    static constexpr unsigned int state3double = 6;

    // Events
    using eventType = uint8_t;
    static constexpr eventType noEvent = 0;
    static constexpr eventType event1 = 1;

    // Guard conditions
    static constexpr std::string_view guard1 = "guard 1";

    //

    static inline auto noConditions = [] (const GuardConditions&) { return true; };

    static inline std::array<transition<eventType>, 4> transitionsLinear = {
        {{state1, state2, event1, noConditions},
         {state2, state3, noEvent, noConditions},
         {state3, state4, noEvent, noConditions},
         {state4, state5, noEvent, noConditions}}
    };

    static inline std::array<transition<eventType>, 5> transitionsLinear_with_double = {
        {{state1, state2, event1, noConditions},
         {state2, state3double, noEvent, noConditions},
         {state2, state3, noEvent, noConditions},
         {state3, state4, noEvent, noConditions},
         {state4, state5, noEvent, noConditions}}
    };

    static inline std::array<transition<eventType>, 5> transitionsLinear_guards = {
        {{state1, state2, event1, noConditions},
         {state2, state3, noEvent, [] (const GuardConditions& conditions) { return conditions.get (guard1) > 2; }},
         {state3, state4, noEvent, noConditions},
         {state4, state5, noEvent, noConditions}}
    };
};

class TestFileTransferProtocole : public ::testing::Test
{
public:
    TestFileTransferProtocole() : protocole (nullptr, FileTransferProtocoleDemo::state1){};

    auto& getProtocole() { return protocole; };

private:
    FileTransferProtocoleDemo protocole;
};

TEST_F (TestFileTransferProtocole, TestLinearTransitionsWithoutGuards)
{
    auto& protocole = getProtocole();
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::event1,
                                       FileTransferProtocoleDemo::state1,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear),
               FileTransferProtocoleDemo::state2);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state2,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear),
               FileTransferProtocoleDemo::state3);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state3,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear),
               FileTransferProtocoleDemo::state4);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state4,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear),
               FileTransferProtocoleDemo::state5);
}

TEST_F (TestFileTransferProtocole, TestUnkownEvent)
{
    auto& protocole = getProtocole();
    constexpr uint8_t unknown = 23;
    EXPECT_EQ (protocole.getNextState (unknown, FileTransferProtocoleDemo::state1, FileTransferProtocoleDemo::stateUnknown, protocole.transitionsLinear),
               FileTransferProtocoleDemo::stateUnknown);
}

TEST_F (TestFileTransferProtocole, TestLinearTransitionWithDouble)
{
    auto& protocole = getProtocole();
    // If there are multiple solutions, the first one should be chosen
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state2,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_with_double),
               FileTransferProtocoleDemo::state3double);
}

TEST_F (TestFileTransferProtocole, TestLinearTransitionWithGuardsSuccess)
{
    auto& protocole = getProtocole();
    protocole.guards.addGuard (FileTransferProtocoleDemo::guard1, 3);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::event1,
                                       FileTransferProtocoleDemo::state1,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state2);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state2,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state3);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state3,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state4);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state4,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state5);
}

TEST_F (TestFileTransferProtocole, TestLinearTransitionWithGuardsFails)
{
    auto& protocole = getProtocole();
    protocole.guards.addGuard (FileTransferProtocoleDemo::guard1, 1);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::event1,
                                       FileTransferProtocoleDemo::state1,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state2);
    // the next one will give the unknown state because of the guard condition is not fullfilled
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state2,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::stateUnknown);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state3,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state4);
    EXPECT_EQ (protocole.getNextState (FileTransferProtocoleDemo::noEvent,
                                       FileTransferProtocoleDemo::state4,
                                       FileTransferProtocoleDemo::stateUnknown,
                                       protocole.transitionsLinear_guards),
               FileTransferProtocoleDemo::state5);
}
} // namespace xymodem
