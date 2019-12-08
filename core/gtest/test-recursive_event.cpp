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
#include <astl/recursive_event.h>

TEST(recursive_event, NoSlot)
{
    struct MyEventTag{};
    using MyEvent = ::astl::recursive_event<MyEventTag, int, int>;
    MyEvent myEvent;

    myEvent.invoke(2, 3);
}

TEST(recursive_event, RecursiveInvocation)
{
    struct MyEventTag{};
    using MyEvent = ::astl::recursive_event<MyEventTag, int, int>;
    MyEvent myEvent;

    int value1{0}, value2{0}, count{0};
    MyEvent::slot_type slot([&value1, &myEvent](int const& v1, int const& v2){
        if (value1 < 5) {
            value1 = v1 + 1;
            myEvent.invoke(value1, v2);
        }
    });
    MyEvent::slot_type slot2([&value2, &count](int const& /*v1*/, int const& v2) {
        value2 = v2;
        ++count;
    });
    myEvent.sig().connect(slot);
    myEvent.sig().connect(slot2);

    myEvent.invoke(0, 10);
    ASSERT_EQ(value1, 5);
    ASSERT_EQ(value2, 10);
    ASSERT_EQ(count, 6);
}

TEST(recursive_event, VoidEvents) {
    struct MyEventTag {};
    using MyEvent = ::astl::recursive_event<MyEventTag>;
    MyEvent myEvent;

    int count{0};
    MyEvent::slot_type slot([&count]() { ++count; });
    myEvent.sig().connect(slot);
    myEvent.invoke();

    ASSERT_EQ(count, 1);
}
