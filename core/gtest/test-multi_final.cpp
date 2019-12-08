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
#include <astl/multi_final.h>

TEST(multi_final, Empty)
{
    astl::multi_final mf{};
    (void)mf;
}

TEST(multi_final, OneFunctor)
{
    bool exec1{false};
    {
        astl::multi_final mf{};
        mf.append([&exec1](){exec1 = true;});
        ASSERT_FALSE(exec1);
    }
    ASSERT_TRUE(exec1);
}

TEST(multi_final, TwoFunctors)
{
    bool exec1{false};
    bool exec2{false};
    {
        astl::multi_final mf{};
        mf.append([&exec1](){exec1 = true;});
        mf.append([&exec2](){exec2 = true;});
        ASSERT_FALSE(exec1);
        ASSERT_FALSE(exec2);
    }
    ASSERT_TRUE(exec1);
    ASSERT_TRUE(exec2);
}

TEST(multi_final, Resetted)
{
    bool exec1{false};
    bool exec2{false};
    {
        astl::multi_final mf{};
        mf.append([&exec1](){exec1 = true;});
        mf.append([&exec2](){exec2 = true;});
        ASSERT_FALSE(exec1);
        ASSERT_FALSE(exec2);
        mf.reset();
    }
    ASSERT_FALSE(exec1);
    ASSERT_FALSE(exec2);
}
