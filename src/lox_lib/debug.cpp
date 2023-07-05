/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#include <lox_lib/chunk.hpp>
#include <lox_lib/debug.hpp>
#include <lox_lib/value.hpp>

#include <fmt/format.h>

namespace lox {

void disassembleChunk(Chunk& chunk, const char* name)
{
    fmt::print("== {} ==\n", name);

    for (int offset{0}; offset < chunk.bytes().size();) {
        // TODO: Not this
        offset = disassembleInstruction(chunk, offset);
    }
}

namespace {

int constantInstruction(const char* name, const Chunk& chunk, int offset)
{
    std::uint8_t constant{chunk.bytes().at(offset + 1)};
    fmt::print("{:16} {:4} '", name, constant);
    printValue(chunk.constants().at(constant));
    fmt::print("'\n");
    return offset + 2;
}

int simpleInstruction(const char* name, int offset)
{
    fmt::print("{}\n", name);
    return offset + 1;
}

}  // namespace

int disassembleInstruction(Chunk& chunk, int offset)
{
    fmt::print("{:04} ", offset);
    if (offset > 0 &&
        chunk.lines().at(offset) == chunk.lines().at(offset - 1)) {
        fmt::print("   | ");
    }
    else {
        fmt::print("{:04} ", chunk.lines().at(offset));
    }

    Op instruction{static_cast<Op>(chunk.bytes().at(offset))};
    switch (instruction) {
        case Op::constant:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case Op::return_:
            return simpleInstruction("OP_RETURN", offset);
        default:
            fmt::print("Unknown opcode {}\n",
                       static_cast<std::uint8_t>(instruction));
            return offset + 1;
    }
}

}  // namespace lox
