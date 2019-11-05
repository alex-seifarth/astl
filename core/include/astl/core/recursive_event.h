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
#pragma once

#include "signal.h"
#include <cassert>
#include <queue>
#include <tuple>

namespace astl::core {

    //! Event for signal-slot based event delegation with support for recursive event invocations.
    //!
    //! \tparam Ts      Types of data associated with an event. Maybe empty
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    //!
    //! \see \link signal-slot Event Delegation
    template<typename TAG, typename...Ts>
    class recursive_event
    {
    public:
        using value_type = std::tuple<Ts...>;
        using signal_type = signal<TAG, Ts...>;
        using slot_type = slot<TAG, Ts...>;

        explicit recursive_event() = default;
        ~recursive_event() = default;

        recursive_event(recursive_event const&) = delete;
        recursive_event& operator=(recursive_event const&) = delete;

        //! Returns a reference to the signal associated with the event.
        signal_type& sig() noexcept;

        //! Raises the event and propagates it along with the given data value to all connected slots.
        //! Recursively calling invoke will result in undefined behavior and usually terminate the program.
        template<typename...Args>
        void invoke(Args&&...args) noexcept;

    private:
        signal_type signal_{};
        std::queue<value_type> event_queue_{};
    };

} // namespace astl::core

// ------------------------------------------------------------------------------------------------
// impl recursive_event
// ------------------------------------------------------------------------------------------------
template<typename TAG, typename...Ts>
    typename astl::core::recursive_event<TAG, Ts...>::signal_type&
    astl::core::recursive_event<TAG, Ts...>::sig() noexcept
{
    return signal_;
}

template<typename TAG, typename...Ts>
    template<typename...Args>
    void
    astl::core::recursive_event<TAG, Ts...>::invoke(Args &&... args) noexcept
{
    event_queue_.push(std::make_tuple(std::forward<Args>(args)...));
    if (event_queue_.size() > 1) {
        // we're not the first in a recursive invocation, let the while-loop work by returning
        return;
    }
    while(!event_queue_.empty()) {
        std::apply([this](Ts... args){this->signal_.invoke(args...);},  event_queue_.front());
        event_queue_.pop();
    }
}
