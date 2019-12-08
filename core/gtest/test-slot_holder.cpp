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
#include <astl/slot_holder.h>
#include <astl/event.h>

TEST(slot_holder, SingleSlot)
{
    astl::slot_holder sh;

    struct MyEventTag{};
    using MyEvent = astl::event<MyEventTag, int>;
    MyEvent myEvent;

    ASSERT_FALSE(sh.is_connected(myEvent.sig()));

    // deleting non-existing connection shouldn't fail
    sh.disconnect(myEvent.sig());

    int value1{0}, value2{0};
    bool result = sh.connect(myEvent.sig(), [&value1](int const& v){value1 = v;});
    ASSERT_TRUE(result);
    ASSERT_TRUE(sh.is_connected(myEvent.sig()));

    myEvent.invoke(1);
    ASSERT_EQ(value1, 1);

    result = sh.connect(myEvent.sig(), [&value2](int const& v){value2 = v;});
    ASSERT_FALSE(result);
    ASSERT_TRUE(sh.is_connected(myEvent.sig()));
    myEvent.invoke(2);
    ASSERT_EQ(value1, 2);
    ASSERT_EQ(value2, 0);

    result = sh.connect(myEvent.sig(), [&value2](int const& v){value2 = v;}, true);
    ASSERT_TRUE(sh.is_connected(myEvent.sig()));
    ASSERT_TRUE(result);
    myEvent.invoke(3);
    ASSERT_EQ(value1, 2);
    ASSERT_EQ(value2, 3);

    sh.disconnect(myEvent.sig());
    ASSERT_FALSE(sh.is_connected(myEvent.sig()));
    myEvent.invoke(4);
    ASSERT_EQ(value1, 2);
    ASSERT_EQ(value2, 3);
}

TEST(slot_holder, ThreeSlots)
{
    astl::slot_holder sh;

    struct MyEvent1Tag{};
    using MyEvent1 = ::astl::event<MyEvent1Tag, int, std::string>;
    MyEvent1 myEvent1a, myEvent1b;

    struct MyEvent2Tag{};
    using MyEvent2 = ::astl::event<MyEvent2Tag, float>;
    MyEvent2 myEvent2;

    int valueInta{0}, valueIntb{0};
    std::string valueStra{}, valueStrb{};
    float valueFloat{0.};

    sh.connect(myEvent1a.sig(), [&valueInta, &valueStra](int const& i, std::string const& s){valueInta = i; valueStra =s;});
    sh.connect(myEvent1b.sig(), [&valueIntb, &valueStrb](int const& i, std::string const& s){valueIntb = i; valueStrb =s;});
    sh.connect(myEvent2.sig(), [&valueFloat](float const& f){valueFloat = f;});

    myEvent1a.invoke(1, "a");
    ASSERT_EQ(valueInta, 1);
    ASSERT_EQ(valueStra, "a");
    ASSERT_EQ(valueIntb, 0);
    ASSERT_EQ(valueStrb, "");
    ASSERT_FLOAT_EQ(valueFloat, 0.);

    myEvent1b.invoke(2, "b");
    ASSERT_EQ(valueInta, 1);
    ASSERT_EQ(valueStra, "a");
    ASSERT_EQ(valueIntb, 2);
    ASSERT_EQ(valueStrb, "b");
    ASSERT_FLOAT_EQ(valueFloat, 0.);

    myEvent2.invoke(1.4);
    ASSERT_EQ(valueInta, 1);
    ASSERT_EQ(valueStra, "a");
    ASSERT_EQ(valueIntb, 2);
    ASSERT_EQ(valueStrb, "b");
    ASSERT_FLOAT_EQ(valueFloat, 1.4);
}
