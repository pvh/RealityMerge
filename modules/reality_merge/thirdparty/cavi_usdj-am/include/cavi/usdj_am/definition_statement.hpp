/**************************************************************************/
/* definition_statement.hpp                                               */
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

#ifndef CAVI_USDJ_AM_DEFINITION_STATEMENT_HPP
#define CAVI_USDJ_AM_DEFINITION_STATEMENT_HPP

#include <variant>

// local
#include "declaration.hpp"
#include "statement.hpp"

// export type USDA_DefinitionStatement =
//     | USDA_Statement
//     | USDA_Declaration<any>

struct AMdoc;
struct AMitem;

namespace cavi {
namespace usdj_am {

template <typename T>
class ArrayInputIterator;

class Visitor;

/// \brief A struct representing a "USDA_DefinitionStatement" node in a syntax
///        tree that was parsed out of a USDA document, encoded as JSON and
///        stored within an Automerge document.
struct DefinitionStatement : public std::variant<std::monostate, Statement, Declaration> {
    using std::variant<std::monostate, Statement, Declaration>::variant;

    /// \param document[in] A pointer to a borrowed Automerge document.
    /// \param map_object[in] A pointer to a borrowed Automerge map object.
    /// \pre \p document `!= nullptr`
    /// \pre \p map_object `!= nullptr`
    /// \pre `AMitemValType(` \p map_object `) == AM_VAL_TYPE_OBJ_TYPE`
    /// \pre `AMobjObjType(` \p document `, AMitemObjId(` \p map_object `)) == AM_OBJ_TYPE_MAP`
    /// \throws std::invalid_argument
    DefinitionStatement(AMdoc const* const document, AMitem const* const map_object);

    DefinitionStatement(DefinitionStatement const&) = delete;
    DefinitionStatement& operator=(DefinitionStatement const&) = delete;

    DefinitionStatement(DefinitionStatement&&) = default;
    DefinitionStatement& operator=(DefinitionStatement&&) = default;

    /// \note An inlined destructor can't delete incomplete types.
    ~DefinitionStatement();

    /// \brief Accepts a visitor that can only read this node.
    ///
    /// \param[in] visitor A node visitor.
    void accept(Visitor& visitor) const&;

    /// \brief Accepts a visitor that can take ownership of this node.
    ///
    /// \param[in] visitor A node visitor.
    void accept(Visitor& visitor) &&;

private:
    DefinitionStatement() = default;

    friend class ArrayInputIterator<DefinitionStatement>;
};

}  // namespace usdj_am
}  // namespace cavi

#endif  // CAVI_USDJ_AM_DEFINITION_STATEMENT_HPP
