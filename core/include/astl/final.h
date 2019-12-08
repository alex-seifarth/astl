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

#include <functional>

namespace astl {

    //! A final object holds a functor void() that will be executed when the functor is being destroyed.
    //!
    //! final objects are typically used in exception safe programming when a certain task - the functor - has to be
    //! executed when an exception occurs. If instead the method reaches a point where the exception risk is over the
    //! final object can be reset to avoid execution of the functor:
    //! \code
    //! #include <astl/final.h>
    //!
    //! astl::final f([](){ .. code to cleanup after exceptions ... });
    //! ...
    //! .. exception risky code ..
    //! ...
    //! // no exception occured -> no need for f to cleanup
    //! f.reset()
    //! \endcode
    class final
    {
    public:
        //! Creates a new final object holding the functor f that will be executed when the newly created object
        //! is destroyed, if it has not been reset before.
        template<typename F>
        explicit final(F f) noexcept;

        //! Destroys the object and executes the functor stored in it, if not reset before.
        ~final() noexcept;

        //! Resets the functor to f.
        template<typename F>
        void reset(F f) noexcept;

        //! Clears the functor so that it will not be executed in the object's destructor.
        void reset() noexcept;

    private:
        std::function<void()> functor_;
    };

} // namespace astl

template<typename F>
astl::final::final(F f) noexcept
        : functor_{f}
{}

astl::final::~final() noexcept
{
    if (functor_) {
        functor_();
    }
}

template<typename F>
void astl::final::reset(F f) noexcept
{
    functor_ = f;
}

void astl::final::reset() noexcept
{
    functor_ = std::function<void()>{};
}
