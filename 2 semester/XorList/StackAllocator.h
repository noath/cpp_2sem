#pragma once

#include <iostream>

template<class T, size_t CHUNK_SIZE = 8 * 1024>
class StackAllocator {
public:
    //typedefs
    typedef T value_type;

    typedef T *pointer;
    typedef const T *const_pointer;

    typedef T &reference;
    typedef const T &const_reference;

    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

public:
    template<typename U>
    struct rebind {
        typedef StackAllocator<U> other;
    };

public:
    //singleton
    static StackAllocator &getInstance() {
        static StackAllocator instance;
        return instance;
    }

    //StackAllocator(const StackAllocator &other) {};

    //StackAllocator &operator=(const StackAllocator &other) {};

public:
    explicit StackAllocator() : chunk_cnt_(0), alloc_end_(nullptr), allocated_(0),
                                first_chunk_(nullptr), last_chunk_(nullptr) {}

    ~StackAllocator() {
        Chunk *current_chunk = first_chunk_;
        while (current_chunk != nullptr) {
            Chunk *tmp = current_chunk->next_;
            delete current_chunk;
            current_chunk = tmp;
        }
        delete current_chunk;
    }

public:
    pointer allocate(size_type n) {
        if (alloc_end_ == nullptr) {
            auto ptr = reinterpret_cast<pointer>(::operator new(Chunk::size * sizeof(value_type)));
            last_chunk_ = new Chunk(ptr);
            first_chunk_ = last_chunk_;
            alloc_end_ = last_chunk_->left_;
            chunk_cnt_++;
            return allocate(n);
        }

        if (n <= (last_chunk_->left_ + Chunk::size) - alloc_end_) {
            pointer tmp = alloc_end_;
            alloc_end_ += n;
            last_chunk_->right_ = alloc_end_;
            return tmp;

        } else {
            auto ptr = reinterpret_cast<pointer>(::operator new(Chunk::size * sizeof(value_type)));
            auto next_chunk = new Chunk(ptr);
            last_chunk_->next_ = next_chunk;
            alloc_end_ = next_chunk->left_ + n;
            last_chunk_ = next_chunk;
            chunk_cnt_++;
            return ptr;
        }
    }

    inline void deallocate(pointer ptr, size_type n) {
        //does nothing
    }

private:
    struct Chunk {
        Chunk *next_;
        pointer left_;
        pointer right_;
        static const size_type size = CHUNK_SIZE;

        Chunk() : next_(nullptr), left_(nullptr), right_(nullptr) {}

        explicit Chunk(pointer left) : next_(nullptr), left_(left), right_(nullptr) {}
    };

private:
    pointer alloc_end_;
    Chunk *last_chunk_;
    Chunk *first_chunk_;
    size_type allocated_;
    int chunk_cnt_;
};
