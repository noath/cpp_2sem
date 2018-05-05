#pragma once

#include <iostream>
#include <algorithm>
#include <memory>

#define INT_PTR uintptr_t

template<typename T, class Allocator = std::allocator<T>>
class XorList {
public:
    explicit XorList(const Allocator &alloc = Allocator()) : size_(0), allocator_(allocator_type()) {
        first_ = new Node<T>;
        last_ = new Node<T>;
        first_->xor_address = reinterpret_cast<INT_PTR>(nullptr);
        last_->xor_address = reinterpret_cast<INT_PTR>(nullptr);
    }

    explicit XorList(size_t count, const T &value = T(), const Allocator &alloc = Allocator()) :
            allocator_(alloc), first_(nullptr), last_(nullptr), size_(0) {
        for (size_t i = 0; i < count; i++) {
            push_back(value);
        }
    }

    XorList(const XorList &other) : allocator_(allocator_type()), size_(0) {
        Node <T>*prev_node = other.first_;
        Node <T>*cur_node = reinterpret_cast<Node <T>*>(prev_node->xor_address);
        push_back(prev_node->value);
        while (cur_node != nullptr) {
            push_back(cur_node->value);
            Node <T>*tmp = cur_node;
            cur_node = next(prev_node, cur_node);
            prev_node = tmp;
        }
    }

    XorList(XorList &&other) noexcept : allocator_{other.allocator_}, first_{other.first_},
                                        last_{other.last_}, size_{other.size_} {
        other.allocator_ = allocator_type();
        other.first_ = nullptr;
        other.last_ = nullptr;
        other.size_ = 0;
    }

    XorList &operator=(const XorList &other) {
        if (this == &other)
            return *this;
        this->clear();
        *this = XorList(other);
        return *this;
    }

    XorList &operator=(XorList &&other) noexcept {
        if (this == &other)
            return *this;

        this->clear();

        size_ = other.size_;
        first_ = other.first_;
        last_ = other.last_;
        allocator_ = other.allocator_;

        other.size_ = 0;
        other.first_ = nullptr;
        other.last_ = nullptr;
        other.allocator_ = allocator_type();

        return *this;
    }

    ~XorList() {
        this->clear();
    }

public:
    template<typename U>
    void push_back(U &&new_elem) {
        Node<T> *new_node = allocator_.allocate(1);
        new_node->value = std::forward<T>(new_elem);
        //allocator_.construct(new_node, Node<T>());
        if (empty()) {
            new_node->xor_address = reinterpret_cast<INT_PTR>(nullptr);
            first_ = new_node;
            last_ = new_node;
        } else if (size_ > 1) {
            last_->xor_address ^= reinterpret_cast<INT_PTR>(new_node);
            new_node->xor_address = reinterpret_cast<INT_PTR>(last_);
            last_ = new_node;
        } else if (size_ == 1) {
            last_ = new_node;
            last_->xor_address = reinterpret_cast<INT_PTR>(first_);
            first_->xor_address = reinterpret_cast<INT_PTR>(last_);
        }
        ++size_;
    }

    template <typename U>
    void push_front(U &&new_elem) {
        Node<T> *new_node = allocator_.allocate(1);
        //allocator_.construct(new_node, Node<T>());
        new_node->value = std::forward <T> (new_elem);
        if (empty()) {
            new_node->xor_address = reinterpret_cast<INT_PTR>(nullptr);
            first_ = new_node;
            last_ = new_node;
        } else if (size_ > 1) {
            first_->xor_address ^= reinterpret_cast<INT_PTR>(new_node);
            new_node->xor_address = reinterpret_cast<INT_PTR>(first_);
            first_ = new_node;
        } else if (size_ == 1) {
            first_ = new_node;
            first_->xor_address = reinterpret_cast<INT_PTR>(last_);
            last_->xor_address = reinterpret_cast<INT_PTR>(first_);
        }
        ++size_;
    }

    const T pop_back() {
        Node<T> *old = last_;
        T ret_value;

        if (empty()){
            throw std::out_of_range("Try to delete from empty list");
        }
        else {
            if (size_ > 2) {
                ret_value = last_->value;
                Node<T>* pre_last = reinterpret_cast<Node<T>*>(last_->xor_address);
                pre_last->xor_address ^= reinterpret_cast<INT_PTR>(last_);
                last_ = pre_last;
            } else if (size_ == 2) {
                ret_value = last_->value;
                first_->xor_address = reinterpret_cast<INT_PTR>(nullptr);
                last_ = first_;
            } else if (size_ == 1) {
                ret_value = last_->value;
                first_ = nullptr;
                last_ = nullptr;
            }
            --size_;

            //allocator_.destroy(old);
            allocator_.deallocate(old, 1);
        }

        return ret_value;
    }

    const T pop_front() {
        Node <T>*old = first_;
        T ret_value;

        if (empty()){
            throw std::out_of_range("Try to delete from empty list");
        }
        else {
            if (size_ > 2) {
                ret_value = first_->value;
                Node<T>* second = reinterpret_cast<Node<T>*>(first_->xor_address);
                second->xor_address ^= reinterpret_cast<INT_PTR>(first_);
                first_ = second;
            } else if (size_ == 2) {
                ret_value = first_->value;
                last_->xor_address = reinterpret_cast<INT_PTR>(nullptr);
                first_ = last_;
            } else if (size_ == 1){
                ret_value = first_->value;
                first_ = nullptr;
                last_ = nullptr;
            }

            --size_;

            //allocator_.destroy(old);
            allocator_.deallocate(old, 1);
        }

        return ret_value;
    }

public:
    void clear() {
        Node<T> *prev_node = first_;
        if (prev_node == nullptr)
            return;
        Node<T> *cur_node = reinterpret_cast<Node<T> *>(prev_node->xor_address);
        while (cur_node != nullptr) {
            Node<T> *tmp = cur_node;
            cur_node = next(prev_node, cur_node);
            allocator_.deallocate(prev_node, 1);
            prev_node = tmp;
        }
        allocator_.deallocate(cur_node, 1);
        first_ = nullptr;
        last_ = nullptr;
    }

    inline bool empty() const {
        return size() == 0;
    }

    inline int size() const {
        return XorList::size_;
    }

private:
    template <typename U>
    struct Node {
        U value;
        INT_PTR xor_address;
    };

    Node <T>*first_;
    Node <T>*last_; //not after the last!
    int size_;

    using allocator_type = typename Allocator::template rebind<Node<T>>::other;
    allocator_type allocator_;

    inline Node<T> *next(Node<T> *prev, Node<T> *current) const {
        return reinterpret_cast<Node<T> *>(reinterpret_cast<INT_PTR>(prev) ^ current->xor_address);
    }
    inline Node<T> *prev(Node<T> *current, Node<T> *next) const {
        return reinterpret_cast<Node<T> *>(reinterpret_cast<INT_PTR>(next) ^ current->xor_address);
    }

public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, T*, T&>{
    public:
        Iterator() : prev_node_(nullptr), cur_node_(nullptr) {};
        Iterator(const Iterator& other) : prev_node_(other.prev_node_), cur_node_(other.cur_node_) {}
        explicit operator bool() {
            return cur_node_ != nullptr;
        }
        bool operator == (const Iterator& other){
            return cur_node_ == other.cur_node_;
        }
        bool operator != (const Iterator& other){
            return not (*this == other);
        }
        T& operator *() const{
            return cur_node_->value;
        }
        T* operator &() const{
            return &cur_node_->value;
        }
        Iterator& operator++(){
            if (cur_node_){
                Node<T>* tmp = cur_node_;
                cur_node_ = reinterpret_cast<Node<T>*>(reinterpret_cast<INT_PTR>(prev_node_) ^ cur_node_->xor_address);
                prev_node_ = tmp;
            }
            return *this;
        }
        const Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        Iterator& operator--(){
            if (prev_node_){
                Node<T>* tmp = prev_node_;
                prev_node_ = reinterpret_cast<Node<T>*>(reinterpret_cast<INT_PTR>(cur_node_) ^ prev_node_->xor_address);
                cur_node_ = tmp;
            }
            return *this;
        }
        const Iterator operator--(int) {
            Iterator tmp(*this);
            --(*this);
            return tmp;
        }

    //should be private here
    public:
        Node<T>* prev_node_;
        Node<T>* cur_node_;
        Iterator(Node<T>* prev_node, Node<T>* cur_node) :
                prev_node_(prev_node), cur_node_(cur_node) {}
        friend class XorList;
    };

public:
    Iterator end(){
        return Iterator(last_, nullptr);
    }

    Iterator begin(){
        return Iterator(nullptr, first_);
    }

public:
    template <typename U>
    void insert_before(Iterator it, U&& value){
        if (it == begin()){
            push_front(value);
        }
        else{
            Node<T> *new_node = allocator_.allocate(1);
            new_node->value = std::forward<T>(value);

            new_node->xor_address = reinterpret_cast<INT_PTR>(it.prev_node_) ^ reinterpret_cast<INT_PTR>(it.cur_node_);

            it.prev_node_->xor_address ^= reinterpret_cast<INT_PTR>(it.cur_node_);
            it.prev_node_->xor_address ^= reinterpret_cast<INT_PTR>(new_node);

            it.cur_node_->xor_address ^= reinterpret_cast<INT_PTR>(it.prev_node_);
            it.cur_node_->xor_address ^= reinterpret_cast<INT_PTR>(new_node);
        }
    }

    template <typename U>
    void insert_after(Iterator it, U&& value){
        if (it == end()){
            push_back(value);
        }
        else{
            Node<T> *new_node = allocator_.allocate(1);
            new_node->value = std::forward<T>(value);
            Node<T> *next_node = next(it.prev_node_, it.cur_node_);

            new_node->xor_address = reinterpret_cast<INT_PTR>(next_node) ^ reinterpret_cast<INT_PTR>(it.cur_node_);

            it.cur_node_->xor_address ^= reinterpret_cast<INT_PTR>(next_node);
            it.cur_node_->xor_address ^= reinterpret_cast<INT_PTR>(new_node);

            next_node->xor_address ^= reinterpret_cast<INT_PTR>(it.cur_node_);
            next_node->xor_address ^= reinterpret_cast<INT_PTR>(new_node);
        }
    }
};
