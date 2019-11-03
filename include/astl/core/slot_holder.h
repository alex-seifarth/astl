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

#include <vector>

namespace astl::core {

    namespace internal {

        class slot_entry_abstract
        {
        public:

            virtual bool is_connected() const = 0;
            virtual void disconnect() = 0;
        };

        template<typename T>
        class slot_entry : public slot_entry_abstract, public T
        {
        public:
            using T::T;


        };

    }

    class slot_holder
    {

    };

} // namespace astl::core
