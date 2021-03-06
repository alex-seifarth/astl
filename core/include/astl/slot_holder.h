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

#include <astl/event.h>
#include <algorithm>
#include <tuple>
#include <vector>

namespace astl {

    //! Manages creation and lifetime of event slots.
    //! A slot_holder object allows to connect handlers to signals so that slots are not explicitely handled by clients.
    //!
    //! \see \link signal-slot Event Delegation
    class slot_holder
    {
    public:
        //! Connects the signal to the handler functor f.
        //! \param replace  If the signal is already connected by this slot holder and replace = true, then the signal
        //!                 will be connected with the new handler f, otherwise the function does nothing and returns false.
        //! \param signal   The signal to connect to.
        //! \param f        The handler functor that shall receive the events from the signal.
        //! \returns true when the new handler f is connected, otherwise false.
        template<typename F, typename TAG, typename...Ts>
        bool connect(astl::signal<TAG, Ts...>& signal, F f, bool replace = false) noexcept;

        //! Disconnects the signal. Does nothing when the signal is not connected by this slot-holder.
        template<typename TAG, typename...Ts>
        void disconnect(astl::signal<TAG, Ts...>& signal) noexcept;

        //! Returns whether signal is connected or not.
        template<typename TAG, typename...Ts>
        bool is_connected(astl::signal<TAG, Ts...>& signal) const noexcept;

    private:
        struct abstract_item {
            [[nodiscard]] virtual bool is_connected() const noexcept = 0;
            virtual ~abstract_item() = default;
        };

        template<typename TAG, typename...Ts>
        struct item : public abstract_item {

            template<typename F>
            explicit item(F f)
                : slot_{std::forward<F>(f)}
            {}

            [[nodiscard]] bool is_connected() const noexcept override {
                return slot_.is_connected();
            };

            astl::slot<TAG, Ts...>& get() noexcept {
                return slot_;
            }

            astl::slot<TAG, Ts...> slot_{};
        };

        using value_type = std::pair<void*, std::unique_ptr<abstract_item>>;
        std::vector<value_type> slots_{};
    };

} // namespace astl

template<typename F, typename TAG, typename...Ts>
bool astl::slot_holder::connect(astl::signal<TAG, Ts...>& signal, F f, bool replace) noexcept
{
    auto i = std::find_if(slots_.begin(), slots_.end(), [&signal](value_type const& v){return v.first == &signal;});
    if (i != slots_.end() && !replace) {
        return false;
    }
    if (i != slots_.end()) {
        slots_.erase(i);
    }
    auto slotItem = std::make_unique<item<TAG, Ts...>>(std::forward<F>(f));
    signal.connect(slotItem->get());
    slots_.push_back(std::make_pair(&signal, std::move(slotItem)));
    return true;
}

template<typename TAG, typename...Ts>
void astl::slot_holder::disconnect(astl::signal<TAG, Ts...>& signal) noexcept
{
    auto i = std::find_if(slots_.begin(), slots_.end(), [&signal](value_type const& v){return v.first == &signal;});
    if (i != slots_.end()) {
        slots_.erase(i);
    }
}

template<typename TAG, typename...Ts>
bool astl::slot_holder::is_connected(astl::signal<TAG, Ts...>& signal) const noexcept
{
    auto i = std::find_if(slots_.cbegin(), slots_.cend(), [&signal](value_type const& v){return v.first == &signal;});
    if (i == slots_.cend())
        return false;
    return i->second->is_connected();
}
