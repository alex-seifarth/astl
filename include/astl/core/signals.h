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

#include <algorithm>
#include <functional>
#include <list>
#include <vector>

namespace astl::core {

    template<typename T, typename TAG> class signal;
    template<typename T, typename TAG> class slot;
    template<typename T, typename TAG> class event;

    //! \ingroup astl/core/signal
    //! Signal transmitting event to connected slots.
    //! Signals are the connection points for slots that are interested in event invocations. Signals are owned by
    //! events and cannot be created outside of them.
    //!
    //! \tparam T       Type of data associated with an event.
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    template<typename T, typename TAG = T>
    class signal
    {
    public:
        using slot_type = slot<T, TAG>;

        signal(signal const&) = delete;
        signal& operator=(signal const&) = delete;

        //! Connects a slot to the signal so that it will receive future events.
        void connect(slot_type& slot);

    private:
        template<typename U, typename V> friend class event;
        template<typename U, typename V> friend class slot;

        explicit signal() = default;
        ~signal();

        void invoke(T const&);
        void slot_disconnected(slot_type& slot);
    private:
        std::list<slot_type*> slots_{};
        slot_type* dispatching_slot_{nullptr};
    };

    //! \ingroup astl/core/signal
    //! A slot contains a (possible indefinite) handler functor that will be called when an event arrives from the
    //! connected signal. The handler functor has signature void(T const&) noexcept.
    //! Slots automatically unlink themselves from connected signals and signals automatically disconnect from all
    //! connected slots when they're destructed.
    //!
    //! \tparam T       Type of data associated with an event.
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    template<typename T, typename TAG = T>
    class slot
    {
    public:
        using signal_type = signal<T, TAG>;

        //! Constructs a new slot disconnected slot object with indefinite handler functor.
        explicit slot() noexcept = default;

        //! Destructor.
        //! Automatically disconnects from possibly connected slot.
        ~slot() noexcept;

        //! Constructs a new disconnected slot object and immediately sets the functor f.
        //! \tparam F   Functor of signature void(T const&) noexcept
        template<typename F>
        explicit slot(F&& f) noexcept;

        slot(slot const&) = delete;
        slot(slot&&) = delete;
        slot& operator=(slot const&) = delete;

        //! Disconnects the slot from the connected signal. If not connected, it does nothing.
        void disconnect() noexcept;

        //! Returns true when the slot is actually connected to a signal.
        [[nodiscard]]
        bool is_connected() const noexcept;

        //! Sets the functor of the slot, a previously set functor will be deleted.
        //! \tparam F   Functor of signature void(T const&) noexcept
        template<typename F>
        void set_handler(F&& f) noexcept;

    private:
        template<typename U, typename V> friend class signal;

        void connectedTo(signal_type& s) noexcept;
        void signal_deleted() noexcept;
        void invoke(T const& value) noexcept;

    private:
        signal_type* signal_{nullptr};
        std::function<void(T const&)> event_handler_{};
    };

    //! \ingroup astl/core/signal
    //! The event class represent an event source that can broadcast events associated with data of type T.
    //! Event producers should own or have access to an instance of an event object. It allows to raise events using the
    //! invoke(T const&) method (event invocation) and to access the signal associated with the event that allows to
    //! connect slots to it.
    //!
    //! \tparam T       Type of data associated with an event.
    //! \tparam TAG     Tagging type to distinguish events using the same data type T. Defaults to T.
    template<typename T, typename TAG = T>
    class event
    {
    public:
        using value_type = T;
        using signal_type = signal<T, TAG>;
        using slot_type = slot<T, TAG>;

        explicit event() = default;
        ~event() = default;

        event(event const&) = delete;
        event& operator=(event const&) = delete;

        //! Returns a reference to the signal associated with the event.
        signal_type& sig();

        //! Raises the event and propagates it along with the given data value to all connected slots.
        void invoke(T const&);

    private:
        signal_type signal_{};
    };

} // namespace astl::core

// ------------------------------------------------------------------------------------------------
// impl signal
// ------------------------------------------------------------------------------------------------
template<typename T, typename TAG>
astl::core::signal<T, TAG>::~signal()
{
    for (auto const& slot : slots_) {
        slot->signal_deleted();
    }
}

template<typename T, typename TAG>
void astl::core::signal<T, TAG>::invoke(T const& value) {
    for (auto i = slots_.begin(); i != slots_.end();) {
        dispatching_slot_ = *i;
        (*i)->invoke(value);
        if (!dispatching_slot_) {
            // actual slot was deleted during ->invoke(..)
            i = slots_.erase(i);
        }
        else {
            ++i;
        }
    }
    dispatching_slot_ = nullptr;
}

template<typename T, typename TAG>
void astl::core::signal<T, TAG>::connect(slot_type& slot)
{
    auto i = std::find_if(slots_.begin(), slots_.end(), [&slot](slot_type* s){return s == &slot;});
    if (i == slots_.end()) {
        // pushing to front guarantees that a new slot will not be dispatched while an event invocation is ongoing
        slots_.push_front(&slot);
        slot.connectedTo(*this);
    }
}

template<typename T, typename TAG>
void astl::core::signal<T, TAG>::slot_disconnected(slot_type& slot)
{
    if (&slot != dispatching_slot_) {
        slots_.erase(std::find_if(slots_.begin(), slots_.end(), [&slot](slot_type *s) { return s == &slot; }));
    }
    else {
        dispatching_slot_ = nullptr;
    }
}

// ------------------------------------------------------------------------------------------------
// impl slot
// ------------------------------------------------------------------------------------------------
template<typename T, typename TAG>
astl::core::slot<T, TAG>::~slot() noexcept
{
    if (signal_) {
        signal_->slot_disconnected(*this);
    }
}

template<typename T, typename TAG>
template<typename F>
astl::core::slot<T, TAG>::slot(F&& f) noexcept
    : event_handler_{f}
{}

template<typename T, typename TAG>
void astl::core::slot<T, TAG>::invoke(T const& value) noexcept
{
    if (event_handler_) {
        event_handler_(value);
    }
}

template<typename T, typename TAG>
void astl::core::slot<T, TAG>::connectedTo(signal_type& s) noexcept
{
    if (signal_ == &s)
        return;
    if (signal_)
        signal_->slot_disconnected(*this);
    signal_ = &s;
}

template<typename T, typename TAG>
void astl::core::slot<T, TAG>::signal_deleted() noexcept
{
    signal_ = nullptr;
}

template<typename T, typename TAG>
void astl::core::slot<T, TAG>::disconnect() noexcept
{
    if (signal_) {
        signal_->slot_disconnected(*this);
        signal_ = nullptr;
    }
}

template<typename T, typename TAG>
bool astl::core::slot<T, TAG>::is_connected() const noexcept
{
    return nullptr != signal_;
}

template<typename T, typename TAG>
template<typename F>
void astl::core::slot<T, TAG>::set_handler(F&& f) noexcept
{
    event_handler_ = f;
}

// ------------------------------------------------------------------------------------------------
// impl event
// ------------------------------------------------------------------------------------------------
template<typename T, typename TAG>
typename astl::core::event<T, TAG>::signal_type& astl::core::event<T, TAG>::sig()
{
    return signal_;
}

template<typename T, typename TAG>
void astl::core::event<T, TAG>::invoke(T const& value)
{
    signal_.invoke(value);
}
