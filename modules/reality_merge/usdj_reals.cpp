/**************************************************************************/
/* usdj_reals.cpp                                                         */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             RealityMerge                               */
/*                          https://cavi.au.dk/                           */
/**************************************************************************/
/* Copyright (c) 2023-present RealityMerge contributors (see AUTHORS.md). */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include <sstream>
#include <stdexcept>
#include <typeinfo>

// third-party
#include <cavi/usdj_am/value.hpp>

// local
#include "usdj_reals.h"

Reals to_reals(cavi::usdj_am::Value const& value) {
    using cavi::usdj_am::ConstValues;
    using cavi::usdj_am::Number;

    std::ostringstream args;
    Reals result;
    try {
        auto const& values_ptr = std::get<std::unique_ptr<ConstValues>>(value);
        result.reserve(values_ptr->size());
        for (auto const& value : *values_ptr) {
            auto const& number = std::get<Number>(value);
            std::visit([&](auto const& alt) { result.push_back(static_cast<Reals::value_type>(alt)); }, number);
        }
    } catch (std::bad_variant_access const& thrown) {
        args << "std::get<std::unique_ptr<ConstValues>>(value): " << thrown.what();
    }
    if (!args.str().empty()) {
        std::ostringstream what;
        what << __func__ << "(" << args.str() << ")";
        std::invalid_argument(what.str());
    }
    return result;
}
