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
#include <astl/event.h>

#include <string>

TEST(event, NoSlot)
{
    struct MyEventTag{};
    using MyEvent = astl::event<MyEventTag, int>;
    MyEvent event{};
    event.invoke(2);
}

TEST(event, OneSlot_SlotAutoDeleted)
{
    struct MyEventTag{};
    using MyEvent = astl::event<MyEventTag, int>;
    MyEvent event{};
    int value{0};

    {
        MyEvent::slot_type mySlot{[&value](int const &v) { value = v; }};
        ASSERT_FALSE(mySlot.is_connected());

        event.sig().connect(mySlot);
        ASSERT_TRUE(mySlot.is_connected());

        event.invoke(1);
        ASSERT_EQ(value, 1);
    }
    event.invoke(2);
    ASSERT_EQ(value, 1);
}

TEST(event, OneSlot_SlotExplicitelyDeleted)
{
    struct MyEventTag{};
    using MyEvent = astl::event<MyEventTag, int>;
    MyEvent event{};
    int value{0};

    {
        MyEvent::slot_type mySlot{[&value](int const &v) { value = v; }};
        ASSERT_FALSE(mySlot.is_connected());

        event.sig().connect(mySlot);
        ASSERT_TRUE(mySlot.is_connected());

        event.invoke(1);
        ASSERT_EQ(value, 1);

        mySlot.disconnect();
        ASSERT_FALSE(mySlot.is_connected());
        event.invoke(2);
        ASSERT_EQ(value, 1);
    }
    event.invoke(3);
    ASSERT_EQ(value, 1);
}

TEST(event, OneSlot_SignalDeleted)
{
    struct MyEventTag{};
    using MyEvent = astl::event<MyEventTag, int, std::string>;

    int valueInt{0};
    std::string valueStr{};
    MyEvent::slot_type mySlot{[&valueInt, &valueStr](int const& i, std::string const& s){
        valueInt = i;
        valueStr = s;
    }};

    {
        MyEvent event{};
        event.sig().connect(mySlot);
        ASSERT_TRUE(mySlot.is_connected());

        event.invoke(1, "first");
        ASSERT_EQ(valueInt, 1);
        ASSERT_EQ(valueStr, "first");
    }
    ASSERT_FALSE(mySlot.is_connected());
}

TEST(event, DeleteSameSlotWhileDispatched1)
{
    struct MyEventTag{};
    using MyEvent = ::astl::event<MyEventTag, std::string>;
    std::string value1, value2;

    MyEvent myEvent;
    MyEvent::slot_type slot1([&value1, &slot1](std::string const& v){value1 = v; slot1.disconnect();});
    MyEvent::slot_type slot2([&value2, &slot2](std::string const& v){value2 = v; slot2.disconnect();});

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

TEST(event, DeleteSameSlotWhileDispatched2)
{
    struct MyEventTag{};
    using MyEvent = ::astl::event<MyEventTag, std::string>;
    std::string value1, value2;

    MyEvent myEvent;
    MyEvent::slot_type slot1([&value1, &slot1](std::string const& v){value1 = v; slot1.disconnect();});
    MyEvent::slot_type slot2([&value2](std::string const& v){value2 = v; });

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

TEST(event, ConnectWhileDispatching)
{
    struct MyEventTag{};
    using MyEvent = ::astl::event<MyEventTag, std::string>;
    std::string value1, value2;

    MyEvent myEvent;
    MyEvent::slot_type slot2([&value2](std::string const& v){value2 = v;});
    MyEvent::slot_type slot1([&value1, &slot2, &myEvent](std::string const& v){
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

TEST(event, DoubleConnect)
{
    struct MyEventTag{};
    using MyEvent = ::astl::event<MyEventTag, std::string>;
    int count{0};

    MyEvent myEvent;
    MyEvent::slot_type slot([&count](std::string const&){++count;});

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

TEST(event, VoidData)
{
    struct MyEventTag{};
    using MyEvent = ::astl::event<MyEventTag>;

    int count{0};
    MyEvent myEvent;
    MyEvent::slot_type slot([&count](){++count;});
    myEvent.sig().connect(slot);

    myEvent.invoke();
    ASSERT_EQ(count, 1);

    myEvent.invoke();
    ASSERT_EQ(count, 2);
}
