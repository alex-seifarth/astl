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

#include <cassert>
#include <algorithm>
#include <functional>
#include <list>

namespace astl::core {

    template<typename TAG, typename...Ts> class recursive_event;
    template<typename TAG, typename...Ts> class event;
    template<typename TAG, typename...Ts> class signal;
    template<typename TAG, typename...Ts> class slot;

    //! Signal transmitting event to connected slots.
    //! Signals are the connection points for slots that are interested in event invocations. Signals are owned by
    //! events and cannot be created outside of them.
    //!
    //! \tparam Ts      Types of data associated with an event. Maybe empty.
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    template<typename TAG, typename...Ts>
    class signal
    {
    public:
        using slot_type = slot<TAG, Ts...>;

        void connect(slot_type& slot) noexcept;

    private:
        template<typename TAG1, typename...Ts1> friend class event;
        template<typename TAG1, typename...Ts1> friend class recursive_event;
        template<typename TAG1, typename...Ts1> friend class slot;

        explicit signal() = default;
        ~signal();

        template<typename...Args>
        void invoke(Args&& ... args) noexcept;

        void slot_detached(slot_type& slot) noexcept;

    private:
        std::list<slot_type*> slots_{};
        slot_type* actual_slot_{nullptr};
    };

    //! A slot contains a (possible indefinite) handler functor that will be called when an event arrives from the
    //! connected signal. The handler functor has signature void(T const&) noexcept.
    //! Slots automatically unlink themselves from connected signals and signals automatically disconnect from all
    //! connected slots when they're destructed.
    //!
    //! \tparam Ts      Types of data associated with an event. Maybe empty
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    template<typename TAG, typename...Ts>
    class slot
    {
    public:
        using functor_type = std::function<void(Ts const&...)>;
        using signal_type = signal<TAG, Ts...>;

        explicit slot() noexcept = default;
        ~slot() noexcept;

        template<typename F>
        explicit slot(F f) noexcept;

        template<typename F>
        void set_functor(F f) noexcept;

        [[nodiscard]] bool is_connected() const noexcept;

        void disconnect() noexcept;

    private:
        template<typename TAG1, typename...Ts1> friend class signal;

        template<typename...Args>
        void invoke(Args&& ... args) noexcept;

        void connected_to(signal_type& signal) noexcept;
        void disconnected() noexcept;

    private:
        functor_type functor_{};
        signal_type* signal_{nullptr};
    };

} // namespace astl::core

// ------------------------------------------------------------------------------------------------
// impl signal
// ------------------------------------------------------------------------------------------------
template<typename TAG, typename...Ts>
    astl::core::signal<TAG, Ts...>::~signal()
{
    for (auto& slot : slots_) {
        slot->disconnected();
    }
}

template<typename TAG, typename...Ts>
    template<typename...Args>
    void
    astl::core::signal<TAG, Ts...>::invoke(Args &&... args) noexcept
{
    assert(actual_slot_ == nullptr); // check recursive invocation
    for(auto i = slots_.begin(); i != slots_.end();) {
        actual_slot_ = *i;
        (*i)->invoke(std::forward<Args>(args)...);
        if (actual_slot_) {
            ++i;
        }
        else {
            i = slots_.erase(i);
        }
    }
    actual_slot_ = nullptr;
}

template<typename TAG, typename...Ts>
    void
    astl::core::signal<TAG, Ts...>::connect(slot_type& slot) noexcept
{
    auto i = std::find_if(slots_.begin(), slots_.end(), [&slot](slot_type* s){return s == &slot;});
    if (i != slots_.end())
        return;
    // pushing to front guarantees that a new slot will not be dispatched while an event invocation is ongoing
    slots_.push_front(&slot);
    slot.connected_to(*this);
}

template<typename TAG, typename...Ts>
    void
    astl::core::signal<TAG, Ts...>::slot_detached(slot_type& slot) noexcept
{
    if (actual_slot_ != &slot) {
        slots_.erase(std::find_if(slots_.begin(), slots_.end(), [&slot](slot_type* s){return s == &slot;}));
    }
    else {
        actual_slot_ = nullptr;
    }
}

// ------------------------------------------------------------------------------------------------
// impl slot
// ------------------------------------------------------------------------------------------------
template<typename TAG, typename...Ts>
    template<typename F>
    astl::core::slot<TAG, Ts...>::slot(F f) noexcept
        : functor_{f}
{}

template<typename TAG, typename...Ts>
    astl::core::slot<TAG, Ts...>::~slot() noexcept
{
    if (signal_) {
        signal_->slot_detached(*this);
    }
}

template<typename TAG, typename...Ts>
    template<typename F>
    void
    astl::core::slot<TAG, Ts...>::set_functor(F f) noexcept
{
    functor_ = f;
}

template<typename TAG, typename...Ts>
    void
    astl::core::slot<TAG, Ts...>::disconnect() noexcept
{
    if (signal_) {
        signal_->slot_detached(*this);
        signal_ = nullptr;
    }
}

template<typename TAG, typename...Ts>
    template<typename...Args>
    void
    astl::core::slot<TAG, Ts...>::invoke(Args &&... args) noexcept
{
    if (functor_) {
        functor_(std::forward<Args>(args)...);
    }
}

template<typename TAG, typename...Ts>
    void
    astl::core::slot<TAG, Ts...>::connected_to(signal_type& signal) noexcept
{
    if (signal_) {
        signal_->slot_detached(*this);
    }
    signal_ = &signal;
}

template<typename TAG, typename...Ts>
    void
    astl::core::slot<TAG, Ts...>::disconnected() noexcept
{
    signal_ = nullptr;
}

template<typename TAG, typename...Ts>
    bool
    astl::core::slot<TAG, Ts...>::is_connected() const noexcept
{
    return signal_;
}

