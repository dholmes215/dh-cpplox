/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#ifndef CPPLOX_DEBUG_HPP
#define CPPLOX_DEBUG_HPP

#include "chunk.hpp"

namespace lox {

void disassembleChunk(Chunk& chunk, const char* name);
int disassembleInstruction(Chunk& chunk, int offset);

}  // namespace lox

#endif  // CPPLOX_DEBUG_HPP
