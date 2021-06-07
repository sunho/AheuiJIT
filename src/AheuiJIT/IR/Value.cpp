#include "Value.h"

#include <fmt/format.h>

using namespace aheuijit;

ValueType Local::type() const {
    return ValueType::Local;
}

std::string Local::description() const {
    return fmt::format("%{}", id);
}

std::string Constant::description() const {
    return fmt::format("#{}", imm);
}

ValueType Constant::type() const {
    return ValueType::Constant;
}
