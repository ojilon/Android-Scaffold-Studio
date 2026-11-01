#include <parser/ParseContext.h>

#include <utility>

namespace parser {

void add_error(ParseContext& context, ParseError error) {
    context.errors.push_back(std::move(error));
}

bool has_errors(const ParseContext& context) {
    return !context.errors.empty();
}

} // namespace parser
