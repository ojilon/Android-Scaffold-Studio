#include <codegen/Template.h>

#include <nlohmann/json.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace codegen {

namespace {

constexpr std::string_view open_marker = "{{";
constexpr std::string_view close_marker = "}}";

std::string_view trim(std::string_view text) {
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t' || text.front() == '\n'))
        text.remove_prefix(1);
    while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\n'))
        text.remove_suffix(1);
    return text;
}

std::vector<std::string_view> split_path(std::string_view path) {
    std::vector<std::string_view> segments;
    std::size_t start = 0;
    while (start <= path.size()) {
        const std::size_t dot = path.find('.', start);
        const std::size_t end = (dot == std::string_view::npos) ? path.size() : dot;
        segments.push_back(path.substr(start, end - start));
        if (dot == std::string_view::npos)
            break;
        start = dot + 1;
    }
    return segments;
}

// Returns nullptr when the dotted path does not resolve.
const nlohmann::json* lookup(const nlohmann::json& values, std::string_view path) {
    const nlohmann::json* current = &values;

    for (const std::string_view segment : split_path(path)) {
        if (!current->is_object())
            return nullptr;
        const auto found = current->find(std::string(segment));
        if (found == current->end())
            return nullptr;
        current = &(*found);
    }
    return current;
}

} // namespace

core::Result<std::string> render(std::string_view template_text,
                                 const nlohmann::json& values,
                                 std::string_view template_name) {
    std::string output;
    output.reserve(template_text.size() + template_text.size() / 8);

    std::size_t cursor = 0;
    while (cursor < template_text.size()) {
        const std::size_t open = template_text.find(open_marker, cursor);
        if (open == std::string_view::npos) {
            output.append(template_text.substr(cursor));
            break;
        }

        output.append(template_text.substr(cursor, open - cursor));

        const std::size_t close = template_text.find(close_marker, open + open_marker.size());
        if (close == std::string_view::npos) {
            return std::unexpected(std::string(template_name) +
                                   ": unterminated placeholder starting at offset " +
                                   std::to_string(open));
        }

        const std::string_view path =
            trim(template_text.substr(open + open_marker.size(), close - open - open_marker.size()));

        const nlohmann::json* value = path.empty() ? nullptr : lookup(values, path);
        if (value == nullptr) {
            return std::unexpected(std::string(template_name) + ": unknown template variable {{" +
                                   std::string(path) + "}}");
        }
        if (!value->is_string()) {
            return std::unexpected(std::string(template_name) + ": template variable {{" +
                                   std::string(path) + "}} is not a string");
        }

        output.append(value->get<std::string>());
        cursor = close + close_marker.size();
    }

    return output;
}

} // namespace codegen
