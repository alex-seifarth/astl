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

namespace astl {

    //! The event class represent an event source that can broadcast events associated with data of type T.
    //! Event producers should own or have access to an instance of an event object. It allows to raise events using the
    //! invoke(T const&) method (event invocation) and to access the signal associated with the event that allows to
    //! connect slots to it.
    //! Note that the event class is not able to handle recursive event invocations. These will result in undefined
    //! behavior (DEBUG builds will terminate).
    //!
    //! \tparam Ts      Types of data associated with an event. Maybe empty.
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    //!
    //! \see \link signal-slot Event Delegation
    template<typename TAG, typename...Ts>
    class event
    {
    public:
        using value_type = std::tuple<Ts...>;
        using signal_type = signal<TAG, Ts...>;
        using slot_type = slot<TAG, Ts...>;

        explicit event() = default;
        ~event() = default;

        event(event const&) = delete;
        event& operator=(event const&) = delete;

        //! Returns a reference to the signal associated with the event.
        signal_type& sig() noexcept;

        //! Raises the event and propagates it along with the given data value to all connected slots.
        //! Recursively calling invoke will result in undefined behavior and usually terminate the program.
        template<typename...Args>
        void invoke(Args&&...args) noexcept;

    private:
        signal_type signal_{};
#ifndef NDEBUG
        bool dispatching_{false};
#endif
    };

} // namespace astl


// ------------------------------------------------------------------------------------------------
// impl event
// ------------------------------------------------------------------------------------------------
template<typename TAG, typename...Ts>
    typename astl::event<TAG, Ts...>::signal_type&
    astl::event<TAG, Ts...>::sig() noexcept
{
    return signal_;
}

template<typename TAG, typename...Ts>
    template<typename...Args>
    void
    astl::event<TAG, Ts...>::invoke(Args &&... args) noexcept
{
#ifndef NDEBUG
    assert(!dispatching_);
    dispatching_ = true;
#endif
    signal_.invoke(std::forward<Args>(args)...);
#ifndef NDEBUG
    dispatching_ = false;
#endif
}
