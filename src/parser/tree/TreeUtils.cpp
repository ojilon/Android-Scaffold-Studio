#include <parser/TreeUtils.h>

namespace parser
{

void visit(
    Widget& widget,
    WidgetVisitor visitor)
{
    visitor(widget);

    for (auto& child : widget.children)
    {
        visit(child, visitor);
    }
}

void visit(
    const Widget& widget,
    ConstWidgetVisitor visitor)
{
    visitor(widget);

    for (const auto& child : widget.children)
    {
        visit(child, visitor);
    }
}

Widget*
find_by_id(
    Widget& widget,
    const std::string& id)
{
    if (widget.id == id)
    {
        return &widget;
    }

    for (auto& child : widget.children)
    {
        if (auto* found =
            find_by_id(child, id))
        {
            return found;
        }
    }

    return nullptr;
}

const Widget*
find_by_id(
    const Widget& widget,
    const std::string& id)
{
    if (widget.id == id)
    {
        return &widget;
    }

    for (const auto& child :
         widget.children)
    {
        if (auto* found =
            find_by_id(child, id))
        {
            return found;
        }
    }

    return nullptr;
}

std::vector<Widget*>
find_by_type(
    Widget& widget,
    const std::string& type)
{
    std::vector<Widget*> result;

    visit(
        widget,
        [&](Widget& current)
        {
            if (current.type == type)
            {
                result.push_back(
                    &current);
            }
        });

    return result;
}

std::size_t
count_widgets(
    const Widget& widget)
{
    std::size_t total = 1;

    for (const auto& child :
         widget.children)
    {
        total += count_widgets(child);
    }

    return total;
}

}