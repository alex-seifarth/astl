// (C) Copyright 2019 Alexander Seifarth
//
// This file is part of ASTL.
// ASTL is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option)
// any later version.
// ASTL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
// You should have received a copy of the GNU General Public License along with Foobar.
// If not, see <http://www.gnu.org/licenses/>.
#include <gtest/gtest.h>
#include <astl/core/signals.h>

#include <string>

TEST(SignalSlot, OneSlot_SlotDeleted) {
    using MyEvent = ::astl::core::event<int>;

    MyEvent myEvent;

    int receivedInt{0};
    {
        MyEvent::slot_type mySlot{[&receivedInt](MyEvent::value_type const& v){receivedInt = v;}};
        ASSERT_FALSE(mySlot.is_connected());

        myEvent.sig().connect(mySlot);
        ASSERT_TRUE(mySlot.is_connected());

        myEvent.invoke(3);
        ASSERT_EQ(3, receivedInt);
    }
    myEvent.invoke(5);
    ASSERT_EQ(3, receivedInt);
}

TEST(SignalSlot, OneSlot_SignalDeleted)
{
    using MyEvent = ::astl::core::event<std::string>;

    std::string value{};
    MyEvent::slot_type mySlot([&value](MyEvent::value_type const& v){value = v;});
    {
        MyEvent ev;
        ASSERT_FALSE(mySlot.is_connected());
        ev.invoke("abc");
        ASSERT_EQ(value, std::string{});

        ev.sig().connect(mySlot);
        ASSERT_TRUE(mySlot.is_connected());
        ASSERT_EQ(value, std::string{});

        ev.invoke("defg");
        ASSERT_EQ(value, std::string{"defg"});
    }
    ASSERT_FALSE(mySlot.is_connected());
}

TEST(SignalSlot, DeleteSameSlotWhileDispatched1)
{
    using MyEvent = ::astl::core::event<std::string>;
    std::string value1, value2;

    MyEvent myEvent;
    MyEvent::slot_type slot1([&value1, &slot1](MyEvent::value_type const& v){value1 = v; slot1.disconnect();});
    MyEvent::slot_type slot2([&value2, &slot2](MyEvent::value_type const& v){value2 = v; slot2.disconnect();});

    myEvent.sig().connect(slot1);
    myEvent.sig().connect(slot2);
    ASSERT_TRUE(slot1.is_connected());
    ASSERT_TRUE(slot2.is_connected());

    myEvent.invoke("first");
    ASSERT_EQ(value1, "first");
    ASSERT_EQ(value2, "first");
    ASSERT_FALSE(slot1.is_connected());
    ASSERT_FALSE(slot2.is_connected());

    myEvent.invoke("second");
    ASSERT_EQ(value1, "first");
    ASSERT_EQ(value2, "first");
    ASSERT_FALSE(slot1.is_connected());
    ASSERT_FALSE(slot2.is_connected());
}

TEST(SignalSlot, DeleteSameSlotWhileDispatched2)
{
    using MyEvent = ::astl::core::event<std::string>;
    std::string value1, value2;

    MyEvent myEvent;
    MyEvent::slot_type slot1([&value1, &slot1](MyEvent::value_type const& v){value1 = v; slot1.disconnect();});
    MyEvent::slot_type slot2([&value2](MyEvent::value_type const& v){value2 = v; });

    myEvent.sig().connect(slot1);
    myEvent.sig().connect(slot2);
    ASSERT_TRUE(slot1.is_connected());
    ASSERT_TRUE(slot2.is_connected());

    myEvent.invoke("first");
    ASSERT_EQ(value1, "first");
    ASSERT_EQ(value2, "first");
    ASSERT_FALSE(slot1.is_connected());
    ASSERT_TRUE(slot2.is_connected());

    myEvent.invoke("second");
    ASSERT_EQ(value1, "first");
    ASSERT_EQ(value2, "second");
    ASSERT_FALSE(slot1.is_connected());
    ASSERT_TRUE(slot2.is_connected());
}

TEST(SignalSlot, ConnectWhileDispatching)
{
    using MyEvent = ::astl::core::event<std::string>;
    std::string value1, value2;

    MyEvent myEvent;
    MyEvent::slot_type slot2([&value2](MyEvent::value_type const& v){value2 = v;});
    MyEvent::slot_type slot1([&value1, &slot2, &myEvent](MyEvent::value_type const& v){
        value1 = v;
        myEvent.sig().connect(slot2);
    });

    myEvent.sig().connect(slot1);
    ASSERT_TRUE(slot1.is_connected());
    ASSERT_FALSE(slot2.is_connected());

    myEvent.invoke("first");
    ASSERT_TRUE(slot1.is_connected());
    ASSERT_TRUE(slot2.is_connected());
    ASSERT_EQ(value1, "first");
    ASSERT_EQ(value2, std::string{});

    myEvent.invoke("second");
    ASSERT_TRUE(slot1.is_connected());
    ASSERT_TRUE(slot2.is_connected());
    ASSERT_EQ(value1, "second");
    ASSERT_EQ(value2, "second");
}

TEST(SignalSlot, DoubleConnect)
{
    using MyEvent = ::astl::core::event<std::string>;
    int count{0};

    MyEvent myEvent;
    MyEvent::slot_type slot([&count](MyEvent::value_type const&){++count;});

    myEvent.sig().connect(slot);
    myEvent.sig().connect(slot);

    myEvent.invoke("a");
    ASSERT_EQ(count, 1);

    myEvent.invoke("a");
    ASSERT_EQ(count, 2);

    slot.disconnect();
    ASSERT_FALSE(slot.is_connected());
    myEvent.invoke("b");
    ASSERT_EQ(count, 2);
}
