#include <parser/TreeUtils.h>

namespace parser {

namespace {

void visit_impl(Widget& widget, const WidgetVisitor& visitor) {
    visitor(widget);
    for (auto& child : widget.children)
        visit_impl(child, visitor);
}

void visit_impl(const Widget& widget, const ConstWidgetVisitor& visitor) {
    visitor(widget);
    for (const auto& child : widget.children)
        visit_impl(child, visitor);
}

} // namespace

void visit(Widget& root, const WidgetVisitor& visitor) {
    visit_impl(root, visitor);
}

void visit(const Widget& root, const ConstWidgetVisitor& visitor) {
    visit_impl(root, visitor);
}

Widget* find_by_id(Widget& root, std::string_view id) {
    if (root.id == id)
        return &root;

    for (auto& child : root.children) {
        if (auto* found = find_by_id(child, id))
            return found;
    }
    return nullptr;
}

const Widget* find_by_id(const Widget& root, std::string_view id) {
    if (root.id == id)
        return &root;

    for (const auto& child : root.children) {
        if (auto* found = find_by_id(child, id))
            return found;
    }
    return nullptr;
}

std::vector<Widget*> find_by_type(Widget& root, std::string_view type) {
    std::vector<Widget*> matches;
    visit(root, [&](Widget& widget) {
        if (widget.type == type)
            matches.push_back(&widget);
    });
    return matches;
}

std::size_t count_widgets(const Widget& root) {
    std::size_t total = 1;
    for (const auto& child : root.children)
        total += count_widgets(child);
    return total;
}

std::vector<std::string> all_ids(const Widget& root) {
    std::vector<std::string> ids;
    visit(root, [&](const Widget& widget) {
        if (!widget.id.empty())
            ids.push_back(widget.id);
    });
    return ids;
}

} // namespace parser
