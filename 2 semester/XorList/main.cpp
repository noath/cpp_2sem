#include <cstdint>
#include <list>
#include <ctime>
#include <random>
#include "XorList.h"
#include "StackAllocator.h"

int main() {
    std::list<int, StackAllocator<int>> l1;
    std::list<int> l2;
    srand(unsigned(time(nullptr)));

    time_t start = clock();

    for (long long i = 0; i < 1e6; i++) {
        l1.push_back(rand());
        l1.push_front(rand());
        if (i%2) l1.pop_back();
        else l1.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i%2) l1.pop_back();
        else l1.pop_front();
    }
    time_t finish = clock();
    std::cout <<"std::list with StackAllocator: "<< (finish - start) / 1000.0 << std::endl;


    start = clock();
    for (long long i = 0; i < 1e6; i++) {
        l2.push_back(rand());
        l2.push_front(rand());
        if (i%2) l2.pop_back();
        else l2.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i%2) l2.pop_back();
        else l2.pop_front();
    }
    finish = clock();
    std::cout <<"std::list with std::allocator: "<< (finish - start) / 1000.0 << std::endl;


    //tests with XorList
    XorList<int, StackAllocator<int>> l3;
    XorList<int> l4;

    start = clock();
    for (long long i = 0; i < 1e6; i++) {
        l3.push_back(rand());
        l3.push_front(rand());
        if (i%2) l3.pop_back();
        else l3.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i%2) l3.pop_back();
        else l3.pop_front();
    }
    finish = clock();
    std::cout <<"XorList with StackAllocator: "<< (finish - start) / 1000.0 << std::endl;


    start = clock();
    for (long long i = 0; i < 1e6; i++) {
        l4.push_back(rand());
        l4.push_front(rand());
        if (i%2) l4.pop_back();
        else l4.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i%2) l4.pop_back();
        else l4.pop_front();
    }
    finish = clock();
    std::cout <<"XorList with std::allocator: "<< (finish - start) / 1000.0 << std::endl;

}

//Testing capabilities of XorList
//    XorList<int, StackAllocator<int>> new_list;
//    for (int i = 0; i < 20; i++){
//        new_list.push_back(i);
//    }
//    for (int i = 0; i < 10; i++){
//        std::cout << new_list.pop_back() << " ";
//    }
//    for (int i = 0; i < 10; i++){
//        std::cout << new_list.pop_front() << " ";
//    }

//    XorList<int> l2;
//    for (int i = 0; i < 5; i++) {
//        l2.push_back(i);
//    }
//    for (auto it = l2.begin(); it != l2.end(); ++it) {
//        std::cout << *it <<" ";
//    }
//
//    XorList<int>::Iterator it = l2.begin();
//    std::cout<<"\n";
//    int a = -1;
//    it++;
//    it++;
//    std::cout<<*it<<"\ninsert before: ";
//    l2.insert_before(it, -1);
//    for (auto it = l2.begin(); it != l2.end(); ++it) {
//        std::cout << *it <<" ";
//    }
//    it = l2.begin();
//    it++;
//    it++;
//    it++;
//   // std::cout << "\ncurrent:" <<it.cur_node_->value;
//   // std::cout << "\nprev:" <<it.prev_node_->value;
//
//    std::cout<<"\ninsert after: ";
//    l2.insert_after(it, -1);
//    for (auto it = l2.begin(); it != l2.end(); ++it) {
//        std::cout << *it <<" ";
//    }

//    std::cout << "\n";
//    test_it--;
//    for (int i = 0; i < 20; i++) {
//        std::cout << *test_it << " ";
//        test_it++;
//    }
//    std::cout << "\n";