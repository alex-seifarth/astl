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
#include <chrono>
#include <vector>

#include <sys/epoll.h>

namespace astl::_runloop_detail_ {

    class runloop_impl
    {
    public:
        explicit runloop_impl(std::size_t events_per_run = 100);
        ~runloop_impl();

        runloop_impl(runloop_impl const&) = delete;
        runloop_impl& operator=(runloop_impl const&) = delete;

        void run(std::chrono::milliseconds timeout);
        void wake_up() noexcept;

    private:
        void process_file_descriptors(int count);
        void process_wake_up_fd();

    private:
        int _epoll_fd{-1};
        int _event_fd{-1};
        std::vector<epoll_event> _epoll_events{};
        std::size_t _epoll_events_size;
    };

} // namespace astl::_runloop_detail_
