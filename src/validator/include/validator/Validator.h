#pragma once
#include <string>
#include <vector>
#include <core/Result.h>

namespace validator {
    struct RuleViolation {
        std::string id;
        std::string path;
        std::string message;
        std::string severity;
    };

    class Validator {
    public:
        static core::Result<void, std::vector<RuleViolation>> validate(const std::string& json_content);
    };
}