/*
 * Copyright (c) 2021 Robert Nystrom
 * Copyright (c) 2023 David Holmes
 * Licensed under the MIT license. See LICENSE file in the project root for
 * details.
 */

#ifndef CPPLOX_MEMORY_HPP
#define CPPLOX_MEMORY_HPP

#include <array>
#include <cstddef>
#include <format>
#include <memory>
#include <stdexcept>

namespace lox {

class alloc_error : public std::exception {
   public:
    virtual const char* what() const noexcept override { return what_.data(); }

   protected:
    static constexpr const int what_size{80};
    alloc_error() : std::exception{} {}
    std::array<char, what_size> what_;
};

class malloc_error : public alloc_error {
   public:
    explicit malloc_error(std::size_t size);
};

class calloc_error : public alloc_error {
   public:
    calloc_error(std::size_t num, std::size_t size);
};

class realloc_error : public alloc_error {
   public:
    explicit realloc_error(std::size_t size);
};

/**
 * @brief Smart pointer owning malloc'd storage (void*).
 */
class malloc_ptr {
   public:
    malloc_ptr() = default;
    ~malloc_ptr() { std::free(ptr_); }

    malloc_ptr& operator=(const malloc_ptr&) = delete;
    malloc_ptr& operator=(nullptr_t) noexcept
    {
        std::free(ptr_);
        ptr_ = nullptr;
        return *this;
    };

    void realloc(std::size_t size);
    void* get() const noexcept { return ptr_; }

    void* release() noexcept
    {
        void* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    friend malloc_ptr make_malloc(std::size_t size);
    friend malloc_ptr make_calloc(std::size_t num, std::size_t size);

   private:
    explicit malloc_ptr(void* ptr) : ptr_{ptr} {}
    void* ptr_{nullptr};
};

/**
 * @brief Smart pointer owning malloc'd storage cast to a specific
 * implicit-lifetime type.
 *
 * @tparam T Type of the objects owned by the pointer.  This type MUST be an
 * implicit-lifetime type, or else trying to use the objects will result in
 * undefined behavior.
 */
template <typename T>
class malloc_ptr_generic {
    // TODO: Enforce that T is an implicit-lifetime type.  This is not presently
    // possible in C++20 but there are proposals to make it possible in C++23.
   public:
    malloc_ptr_generic() = default;

    malloc_ptr_generic& operator=(const malloc_ptr_generic&) = delete;
    malloc_ptr_generic& operator=(nullptr_t) noexcept
    {
        ptr_ = nullptr;
        return *this;
    };

    void realloc(std::size_t num) { ptr_.realloc(num * sizeof(T)); }
    T* get() const noexcept { return static_cast<T*>(ptr_.get()); }
    T* release() noexcept { return static_cast<T*>(ptr_.release()); }

    T& operator*() const noexcept { return *get(); }
    T& operator[](std::size_t idx) const noexcept { return *(get() + idx); }

    template <typename T>
    friend malloc_ptr_generic<T> make_malloc_generic(std::size_t num);
    template <typename T>
    friend malloc_ptr_generic<T> make_calloc_generic(std::size_t num);

   private:
    explicit malloc_ptr_generic(T* ptr) : ptr_{ptr} {}
    explicit malloc_ptr_generic(malloc_ptr&& ptr) : ptr_{std::move(ptr)} {}
    malloc_ptr ptr_;
};

template <typename T>
malloc_ptr_generic<T> make_malloc_generic(std::size_t num)
{
    return malloc_ptr_generic<T>{make_malloc(num * sizeof(T))};
}
template <typename T>
malloc_ptr_generic<T> make_calloc_generic(std::size_t num)
{
    return malloc_ptr_generic<T>{make_calloc(num, sizeof(T))};
}

//////

inline int grow_capacity(int capacity)
{
    return capacity < 8 ? 8 : capacity * 2;
}

template <typename T>
void reallocate(malloc_ptr_generic<T>& ptr,
                std::size_t /*oldSize*/,  // will apparently be used later
                std::size_t newSize)
{
    if (newSize == 0) {
        ptr = nullptr;
        return;
    }

    ptr.realloc(newSize);
}

/**
 * @brief Dynamic array (or vector) that stores implicit-lifetime objects in
 * malloc'd storage.
 *
 * @tparam T Type of the contained objects.  This type MUST be an
 * implicit-lifetime type, or else trying to use the objects will result in
 * undefined behavior.
 */
template <typename T>
class malloc_dynarray {
    // TODO: Enforce that T is an implicit-lifetime type.  This is not presently
    // possible in C++20 but there are proposals to make it possible in C++23.
   public:
    void push_back(T value)
    {
        if (capacity_ < count_ + 1) {
            int oldCapacity{capacity_};
            capacity_ = grow_capacity(oldCapacity);
            reallocate(data_, oldCapacity, capacity_);
        }

        data_[count_] = value;
        count_++;
    }
    T at(int idx) const noexcept { return data_[idx]; }
    int size() const noexcept { return count_; }

   private:
    int count_{0};
    int capacity_{0};
    malloc_ptr_generic<T> data_;
};

}  // namespace lox

#endif  // CPPLOX_MEMORY_HPP
