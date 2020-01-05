// (C) Copyright 2020 Alexander Seifarth
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
#include <astl/runloop.h>

#include <chrono>
#include <thread>

TEST(runloop, setup_runloop)
{
    {
        astl::runloop rl{};

        std::chrono::milliseconds timeout{250};
        auto now = std::chrono::system_clock::now();
        rl.run(timeout);
        auto delta = std::chrono::system_clock::now() - now;
        ASSERT_LE(timeout, delta);
    }
}

TEST(runloop, wakeup)
{
    astl::runloop rl{};
    auto now = std::chrono::system_clock::now();

    std::thread thr ([&rl](){
        rl.run(std::chrono::seconds{1000});
    });

    rl.wake_up();
    auto delta = std::chrono::system_clock::now() - now;
    thr.join();
    ASSERT_LE(delta, std::chrono::seconds{1});

    std::chrono::milliseconds timeout{50};
    now = std::chrono::system_clock::now();
    rl.run(timeout);
    delta = std::chrono::system_clock::now() - now;
    ASSERT_LE(timeout, delta);
}