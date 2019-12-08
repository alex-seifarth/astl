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
#include <vector>

namespace astl {

    //! A multi_final object holds a number of functors void() that will be executed when the functor is being destroyed.
    //!
    //! multi_final objects are typically used in exception safe programming when a certain tasks - the functors - have
    //! to be executed when an exception occurs. If instead the method reaches a point where the exception risk is over
    //! the final object can be reset to avoid execution of the functor:
    //! \code
    //! #include <astl/multi-final.h>
    //!
    //! astl::multi_final mf{};
    //! ...
    //! mf.append([](){ .. cleanup code });
    //! .. exception risky code ..
    //! mf.append([](){ .. other cleanup code });
    //! .. exception risky code ..
    //! ...
    //! // no exception occured -> no need for f to cleanup
    //! mf.reset()
    //! \endcode
    class multi_final
    {
    public:
        multi_final();

        //! Creates a new final object holding the functor f that will be executed when the newly created object
        //! is destroyed, if it has not been reset before.
        template<typename F>
        explicit multi_final(F f) noexcept;

        //! Destroys the object and executes the functors stored in it, if not reset before.
        ~multi_final() noexcept;

        //! Clears the functor so that it will not be executed in the object's destructor.
        void reset() noexcept;

        template<typename F>
        void append(F f) noexcept;

    private:
        std::vector<std::function<void()>> functors_;
    };

} // namespace astl

astl::multi_final::multi_final()
    : functors_{}
{}

astl::multi_final::~multi_final() noexcept {
    for (auto const &f : functors_) {
        f();
    }
}

template<typename F>
astl::multi_final::multi_final(F f) noexcept
    : multi_final{}
{
    append(f);
}

void astl::multi_final::reset() noexcept
{
    functors_.clear();
}

template<typename F>
void astl::multi_final::append(F f) noexcept
{
    functors_.push_back(f);
}
