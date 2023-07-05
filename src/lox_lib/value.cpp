/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#include <lox_lib/value.hpp>

#include <fmt/format.h>

namespace lox {

void printValue(Value value)
{
    fmt::print("{}", value);
}

}  // namespace lox