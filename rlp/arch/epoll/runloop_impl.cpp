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
#include "runloop_impl.h"

#include <astl/multi_final.h>

#include <cstdint>
#include <system_error>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/unistd.h>

using namespace astl::_runloop_detail_;

namespace {

    void safe_close(int& fd)
    {
        close(fd);
        fd = -1;
    }

} // namespace

runloop_impl::runloop_impl(std::size_t events_per_run)
    : _epoll_events_size{events_per_run}
{
    astl::multi_final mf{};

    _epoll_events.resize(_epoll_events_size);

    _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (0 > _epoll_fd) {
        throw std::system_error(errno, std::system_category(), "epoll file descriptor setup failed");
    }
    mf.append([this](){close(_epoll_fd);});

    _event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (0 > _event_fd) {
        throw std::system_error(errno, std::system_category(), "event file descriptor setup failed");
    }
    mf.append([this](){close(_event_fd);});

    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = nullptr;
    if (0 > epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _event_fd, &ev)) {
        throw std::system_error(errno, std::system_category(), "event file descriptor registration failed");
    }

    mf.reset();
}

runloop_impl::~runloop_impl()
{
    safe_close(_event_fd);
    safe_close(_epoll_fd);
}

void runloop_impl::run(std::chrono::milliseconds timeout)
{
    auto res = epoll_wait(_epoll_fd, _epoll_events.data(), _epoll_events.size(), timeout.count());
    if (0 > res) {
        throw std::system_error(errno, std::system_category(), "error while waiting on epoll_wait");
    }
    process_file_descriptors(res);
}

void runloop_impl::wake_up() noexcept
{
    uint64_t v = {1U};
    write(_event_fd, &v, sizeof(v));
}

void runloop_impl::process_file_descriptors(int count)
{
    for (int i = 0; i < count; ++i) {
        auto const& ed = _epoll_events[i];
        if (ed.data.ptr == nullptr) {
            process_wake_up_fd();
        }
        else {

        }
    }
}

void runloop_impl::process_wake_up_fd()
{
    uint64_t buffer{0};
    read(_event_fd, &buffer, sizeof(buffer));
}
