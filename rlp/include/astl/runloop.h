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
#pragma once

#include <astl/rlp_export.h>

#include <chrono>
#include <memory>

namespace astl {

    namespace _runloop_detail_ {
        class runloop_impl;
    }

    class runloop
    {
    public:
        explicit runloop();
        ~runloop();

        runloop(runloop const&) = delete;
        runloop& operator=(runloop const&) = delete;

        //! \brief Runs one iteration of the event loop.
        //! This method should be called from the owning thread only.
        void run(std::chrono::milliseconds timeout);

        //! \brief Wakes up the runloop iteration.
        //! This method can be called from arbitrary threads.
        void wake_up() noexcept;

    private:
        std::unique_ptr<_runloop_detail_::runloop_impl> _pimpl;
    };

} // namespace astl
