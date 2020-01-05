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
#include <astl/runloop.h>

#include "runloop_impl.h"

using namespace astl;

runloop::runloop()
    : _pimpl{std::make_unique<_runloop_detail_::runloop_impl>()}
{

}

runloop::~runloop() = default;

void runloop::run(std::chrono::milliseconds timeout)
{
    _pimpl->run(timeout);
}

void runloop::wake_up() noexcept
{
    _pimpl->wake_up();
}
