#include <gtest/gtest.h>

#include "RHI/CommandList.hpp"

TEST(CommandHeaderTests, DrawCommandReturnsCorrectString)
{
    Nexus::Graphics::CommandHeader header{};

    header.Type = Nexus::Graphics::CommandType::Draw;

    EXPECT_EQ(header.GetCommandTypeAsString(), "Draw");
}

TEST(CommandHeaderTests, BarrierGroupReturnsCorrectString)
{
    Nexus::Graphics::CommandHeader header{};

    header.Type = Nexus::Graphics::CommandType::BarrierGroup;

    EXPECT_EQ(header.GetCommandTypeAsString(), "BarrierGroup");
}

TEST(CommandHeaderTests, UnknownCommandReturnsUnknownCommand)
{
    Nexus::Graphics::CommandHeader header{};

    header.Type = static_cast<Nexus::Graphics::CommandType>(999);

    EXPECT_EQ(header.GetCommandTypeAsString(), "Unknown command");
}

TEST(CommandHeaderTests, AllCommandTypesHaveNames)
{
    for (uint16_t i = 0; i <= static_cast<uint16_t>(Nexus::Graphics::CommandType::BarrierGroup); i++)
    {
        Nexus::Graphics::CommandHeader header{};

        header.Type = static_cast<Nexus::Graphics::CommandType>(i);

        EXPECT_NE(header.GetCommandTypeAsString(), "Unknown command");
    }
}