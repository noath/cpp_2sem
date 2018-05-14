#pragma once

#include <iostream>
#include <memory>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <new>

#define INT_PTR uintptr_t

class IMemoryManager {
public:
    virtual void *Alloc(size_t size) = 0;

    virtual void Free(void *ptr) = 0;

    template<class U, class... Args>
    void construct(U *p, Args &&... args) {
        new((void *) p) U(std::forward<Args>(args)...);
    }

    inline std::string name() {
        std::string name = typeid(*this).name();
        int pos = 0;
        while (isdigit(name[pos])) {
            ++pos;
        }
        std::string cleared = name.substr(pos, name.length() - pos + 1);
        return cleared;
    }
};

template<class T, size_t CHUNK_SIZE = 8 * 1024>
class StackAllocator : public IMemoryManager {
public:
    //typedefs
    typedef T value_type;

    typedef T *pointer;
    typedef const T *const_pointer;

    typedef T &reference;
    typedef const T &const_reference;

    typedef std::ptrdiff_t difference_type;
    typedef size_t size_type;

public:
    template<typename U>
    struct rebind {
        typedef StackAllocator<U> other;
    };

public:

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
    void *Alloc(size_type n) override {
        if (alloc_end_ == nullptr) {
            auto ptr = reinterpret_cast<pointer>(::operator new(Chunk::size * sizeof(value_type)));
            last_chunk_ = new Chunk(ptr);
            first_chunk_ = last_chunk_;
            alloc_end_ = last_chunk_->left_;
            chunk_cnt_++;
            return Alloc(n);
        }

        if (n <= (last_chunk_->left_ + Chunk::size) - alloc_end_) {
            pointer tmp = alloc_end_;
            alloc_end_ += n;
            last_chunk_->right_ = alloc_end_;
            return reinterpret_cast<void *>(tmp);

        } else {
            auto ptr = reinterpret_cast<pointer>(::operator new(Chunk::size * sizeof(value_type)));
            auto next_chunk = new Chunk(ptr);
            last_chunk_->next_ = next_chunk;
            alloc_end_ = next_chunk->left_ + n;
            last_chunk_ = next_chunk;
            chunk_cnt_++;
            return reinterpret_cast<void *>(ptr);
        }
    }

    inline void Free(void *ptr) override {
        //does nothing
    }

    template<class U, class... Args>
    void construct(U *p, Args &&... args) {
        new((void *) p) U(std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U *p) {
        ((T *) p)->~T();
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

class MallocAllocator : public IMemoryManager {
public:
    void *Alloc(size_t size) override {
        return malloc(size);
    }

    void Free(void *ptr) override {
        return free(ptr);
    }
};

class CMemoryManagerSwitcher {
public:
    static CMemoryManagerSwitcher &instance() {
        static CMemoryManagerSwitcher single_instance;
        return single_instance;
    }

    CMemoryManagerSwitcher(const CMemoryManagerSwitcher &other) = delete;

    CMemoryManagerSwitcher &operator=(const CMemoryManagerSwitcher &) = delete;

public:
    static std::shared_ptr<IMemoryManager> selected_;

    inline static void switch_allocator(IMemoryManager *other_allocator) {
        switches_.push_back("Switched from " + selected_->name() + " to " + other_allocator->name());
        selected_.reset(other_allocator);
    }

    static void print_switch_log() {
        for (const auto &i : switches_) {
            std::cout << i << "\n";
        }
    }

private:
    CMemoryManagerSwitcher() = default;

    static std::vector<std::string> switches_;

    friend class CurrentMemoryManager;
};

////Initializing static class-members
//std::shared_ptr<IMemoryManager> CMemoryManagerSwitcher::selected_ = std::make_shared<IMemoryManager>(new MallocAllocator());
std::shared_ptr<IMemoryManager> CMemoryManagerSwitcher::selected_(new MallocAllocator()); /* NOLINT */
std::vector<std::string> CMemoryManagerSwitcher::switches_ = std::vector<std::string>();

class CurrentMemoryManager {
public:
    static std::shared_ptr<IMemoryManager> allocator_;

    static CurrentMemoryManager &instance() {
        static CurrentMemoryManager single_instance;
        return single_instance;
    }

    static void update() {
        CurrentMemoryManager::allocator_ = CMemoryManagerSwitcher::selected_;
    }

    CurrentMemoryManager(const CurrentMemoryManager &other) = delete;

    CurrentMemoryManager &operator=(const CurrentMemoryManager &) = delete;

private:
    CurrentMemoryManager() = default;
};

////Initializing static class-members
std::shared_ptr<IMemoryManager> CurrentMemoryManager::allocator_ = CMemoryManagerSwitcher::selected_; /* NOLINT */

class RuntimeHeap {
public:
    static std::shared_ptr<IMemoryManager> allocator_;

    static RuntimeHeap &instance() {
        static RuntimeHeap single_instance;
        return single_instance;
    }

    RuntimeHeap(const RuntimeHeap &other) = delete;

    RuntimeHeap &operator=(const RuntimeHeap &) = delete;

private:
    RuntimeHeap() = default;
};

////Initializing static class-members
std::shared_ptr<IMemoryManager> RuntimeHeap::allocator_(new MallocAllocator()); /* NOLINT */


class MemoryLogger {
public:
    static void print_memory_use_log() {
        for (const auto &i : logs_) {
            std::cout << i << "\n";
        }
    }
private:
    static std::vector<std::string> logs_;
    template <typename U>
    friend class CAllocatedOn;
};
////Initializing static class-members
std::vector<std::string> MemoryLogger::logs_ = std::vector<std::string>();

template<class Alloc_strategy>
class CAllocatedOn {
public:
    static void *operator new(size_t size) {
        return Alloc(size);
    }

    static void *operator new[](size_t size) {
        return Alloc(size);
    }

    static void operator delete(void *p) {
        Free(p);
    }

    static void operator delete[](void *p) {
        Free(p);
    }

    static void *Alloc(size_t size) {
        CurrentMemoryManager::update();
        MemoryLogger::logs_.push_back("allocated " + std::to_string(size) + " by " +
                              Alloc_strategy::allocator_->name());

        size_t real_size = size + sizeof(std::shared_ptr<IMemoryManager>);
        void *ptr = Alloc_strategy::allocator_->Alloc(real_size);
        //::operator new(sizeof(std::shared_ptr<IMemoryManager>(Alloc_strategy::allocator_)), ptr);
        //Alloc_strategy::allocator_->construct(ptr, std::shared_ptr<IMemoryManager>(Alloc_strategy::allocator_));
        ::new(static_cast<void*>(ptr)) std::shared_ptr<IMemoryManager>(Alloc_strategy::allocator_);

        INT_PTR real_ptr = reinterpret_cast<INT_PTR>(ptr) + sizeof(std::shared_ptr<IMemoryManager>);
        return reinterpret_cast<void *>(real_ptr);
    }

    static void Free(void *ptr) {
        std::stringstream ss;
        std::string address;
        ss << ptr;
        ss >> address;
        size_t real_size = reinterpret_cast<INT_PTR>(ptr) - sizeof(std::shared_ptr<IMemoryManager>);

        ptr = reinterpret_cast<void *>(real_size);
        std::shared_ptr<IMemoryManager> *real_ptr = reinterpret_cast<std::shared_ptr<IMemoryManager> *>(ptr);
        std::shared_ptr<IMemoryManager> primal_allocator = *real_ptr;


        MemoryLogger::logs_.push_back("de-allocated from " + address + " by " +
                              primal_allocator->name());
        primal_allocator->Free(ptr);
        primal_allocator.reset();
    }

};
