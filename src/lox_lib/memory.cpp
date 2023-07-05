/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#include <lox_lib/memory.hpp>

#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace lox {
namespace {
}
malloc_error::malloc_error(std::size_t size)
{
    std::format_to_n(what_.begin(), what_size, "malloc failed, size: {}", size);
}

calloc_error::calloc_error(std::size_t num, std::size_t size)
{
    std::format_to_n(what_.begin(), what_size,
                     "calloc failed, num: {} size: {}", num, size);
}

realloc_error::realloc_error(std::size_t size)
{
    std::format_to_n(what_.begin(), what_size, "realloc failed, size: {}",
                     size);
}

void malloc_ptr::realloc(std::size_t size)
{
    void* newptr{std::realloc(ptr_, size)};  // NOLINT
    if (newptr == nullptr) {
        throw realloc_error{size};
    }
    ptr_ = newptr;
}

malloc_ptr make_malloc(std::size_t size)
{
    void* ptr{std::malloc(size)};  // NOLINT
    if (ptr == nullptr) {
        throw malloc_error{size};
    }
    return malloc_ptr{ptr};
}

malloc_ptr make_calloc(std::size_t num, std::size_t size)
{
    void* ptr{std::calloc(num, size)};  // NOLINT
    if (ptr == nullptr) {
        throw calloc_error{num, size};
    }
    return malloc_ptr{ptr};
}

}  // namespace lox
