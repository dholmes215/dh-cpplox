/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#ifndef CPPLOX_VALUE_HPP
#define CPPLOX_VALUE_HPP

#include "memory.hpp"

namespace lox {

using Value = double;
using ValueArray = malloc_dynarray<Value>;

void printValue(Value value);

}  // namespace lox

#endif  // CPPLOX_VALUE_HPP
