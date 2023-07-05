/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#ifndef CPPLOX_CHUNK_HPP
#define CPPLOX_CHUNK_HPP

#include "memory.hpp"
#include "value.hpp"

#include <cstddef>
#include <memory>

namespace lox {

enum class Op : std::uint8_t {
    constant,
    return_,
};

using ByteArray = malloc_dynarray<std::uint8_t>;
using LineArray = malloc_dynarray<int>;

class Chunk {
   public:
    const ByteArray& bytes() const { return bytes_; }
    const LineArray& lines() const { return lines_; }
    const ValueArray& constants() const { return constants_; }
    void write(std::uint8_t byte, int line);
    int addConstant(Value value);

   private:
    ByteArray bytes_;
    LineArray lines_;
    ValueArray constants_;
};

}  // namespace lox

#endif  // CPPLOX_CHUNK_HPP
