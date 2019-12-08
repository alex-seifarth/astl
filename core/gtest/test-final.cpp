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
#include <astl/final.h>

TEST(final, Executed)
{
    bool executed{false};
    {
        astl::final f([&executed](){executed = true;});
        (void)f;
        ASSERT_FALSE(executed);
    }
    ASSERT_TRUE(executed);
}

TEST(final, MotExecuted)
{
    bool executed{false};
    {
        astl::final f([&executed](){executed = true;});
        ASSERT_FALSE(executed);
        f.reset();
    }
    ASSERT_FALSE(executed);
}

TEST(final, ChangedFunctor)
{
    bool executed{false}, executed2{false};
    {
        astl::final f([&executed](){executed = true;});
        ASSERT_FALSE(executed);
        f.reset([&executed2](){executed2 = true;});
        ASSERT_FALSE(executed);
        ASSERT_FALSE(executed2);
    }
    ASSERT_FALSE(executed);
    ASSERT_TRUE(executed2);
}
