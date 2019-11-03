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
#include <astl/core/slot_holder.h>
#include <astl/core/signals.h>

TEST(SlotHolder, SingleSlot)
{
    astl::core::slot_holder sh;

    using MyEvent = astl::core::event<int>;
    MyEvent myEvent;

    ASSERT_FALSE(sh.is_connected(myEvent.sig()));

    // deleting non-existing connection shouldn't fail
    sh.disconnect(myEvent.sig());

    int value1{0}, value2{0};
    bool result = sh.connect(myEvent.sig(), [&value1](MyEvent::value_type const& v){value1 = v;});
    ASSERT_TRUE(result);
    ASSERT_TRUE(sh.is_connected(myEvent.sig()));

    myEvent.invoke(1);
    ASSERT_EQ(value1, 1);

    result = sh.connect(myEvent.sig(), [&value2](MyEvent::value_type const& v){value2 = v;});
    ASSERT_FALSE(result);
    ASSERT_TRUE(sh.is_connected(myEvent.sig()));
    myEvent.invoke(2);
    ASSERT_EQ(value1, 2);
    ASSERT_EQ(value2, 0);

    result = sh.connect(myEvent.sig(), [&value2](MyEvent::value_type const& v){value2 = v;}, true);
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
