/**************************************************************************/
/* declaration_keyword.cpp                                                */
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

#include <iostream>
#include <map>
#include <string>
#include <string_view>

// local
#include "declaration_keyword.hpp"

namespace {

using cavi::usdj_am::DeclarationKeyword;

static std::map<std::string_view, DeclarationKeyword> const TAGS = {
    {"varying", DeclarationKeyword::VARYING}, {"uniform", DeclarationKeyword::UNIFORM},
    {"custom", DeclarationKeyword::CUSTOM},   {"prepend", DeclarationKeyword::PREPEND},
    {"append", DeclarationKeyword::APPEND},   {"delete", DeclarationKeyword::DELETE},
    {"add", DeclarationKeyword::ADD}};

}  // namespace

namespace cavi {
namespace usdj_am {

std::istream& operator>>(std::istream& is, DeclarationKeyword& out) {
    std::string token;
    if (is >> token) {
        auto const match = TAGS.find(token);
        if (match != TAGS.end()) {
            out = match->second;
            return is;
        }
    }
    is.setstate(std::ios::failbit);
    return is;
}

std::ostream& operator<<(std::ostream& os, DeclarationKeyword const& in) {
    for (auto item : TAGS) {
        if (item.second == in) {
            os << item.first;
            return os;
        }
    }
    os << "???";
    return os;
}

}  // namespace usdj_am
}  // namespace cavi
