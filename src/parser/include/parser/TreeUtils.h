#pragma once

/*
============================================================

Module:
    parser

File:
    TreeUtils.h

Purpose:
    Read-only helpers for walking parser::Widget trees.

Why:
    The validator, the generator and the tests all need the
    same traversal. Keeping it in one place (free functions
    over the value type) means the model stays behaviour-free
    and no two call sites can disagree about ordering.

============================================================
*/

#include <parser/AppModel.h>

#include <cstddef>
#include <functional>
#include <string_view>
#include <vector>

namespace parser {

using WidgetVisitor = std::function<void(Widget&)>;
using ConstWidgetVisitor = std::function<void(const Widget&)>;

void visit(Widget& root, const WidgetVisitor& visitor);
void visit(const Widget& root, const ConstWidgetVisitor& visitor);

Widget* find_by_id(Widget& root, std::string_view id);
const Widget* find_by_id(const Widget& root, std::string_view id);

std::vector<Widget*> find_by_type(Widget& root, std::string_view type);

std::size_t count_widgets(const Widget& root);

// Every non-empty id in the tree, in document order.
std::vector<std::string> all_ids(const Widget& root);

} // namespace parser
