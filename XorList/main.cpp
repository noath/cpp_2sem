#include <cstdint>
#include <list>
#include <ctime>
#include <random>
#include <iostream>
#include <algorithm>
#include <memory>
#include "XorList.h"
#include "StackAllocator.h"

#define ll_PTR ullptr_t
typedef long long ll;

std::mt19937 gen = std::mt19937(unsigned(time(nullptr)));
std::uniform_int_distribution<ll> uniform_distribution = std::uniform_int_distribution<ll>(-UINT32_MAX, UINT32_MAX);
ll randint() {
    return uniform_distribution(gen);
}

int main() {
    std::list<ll, StackAllocator<ll>> l1;
    std::list<ll> l2;
    srand(unsigned(time(nullptr)));

    time_t start = clock();

    for (long long i = 0; i < 1e6; i++) {
        l1.push_back(randint());
        l1.push_front(randint());
        if (i % 2) l1.pop_back();
        else l1.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i % 2) l1.pop_back();
        else l1.pop_front();
    }
    time_t finish = clock();
    std::cout << "std::list with StackAllocator: " << (finish - start) / 1000.0 << std::endl;


    start = clock();
    for (long long i = 0; i < 1e6; i++) {
        l2.push_back(randint());
        l2.push_front(randint());
        if (i % 2) l2.pop_back();
        else l2.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i % 2) l2.pop_back();
        else l2.pop_front();
    }
    finish = clock();
    std::cout << "std::list with std::allocator: " << (finish - start) / 1000.0 << std::endl;


    //tests with XorList
    XorList<ll, StackAllocator<ll>> l3;
    XorList<ll> l4;

    start = clock();
    for (long long i = 0; i < 1e6; i++) {
        l3.push_back(randint());
        l3.push_front(randint());
        if (i % 2) l3.pop_back();
        else l3.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i % 2) l3.pop_back();
        else l3.pop_front();
    }
    finish = clock();
    std::cout << "XorList with StackAllocator: " << (finish - start) / 1000.0 << std::endl;


    start = clock();
    for (long long i = 0; i < 1e6; i++) {
        l4.push_back(randint());
        l4.push_front(randint());
        if (i % 2) l4.pop_back();
        else l4.pop_front();
    }

    for (long long i = 0; i < 1e6 - 5; i++) {
        if (i % 2) l4.pop_back();
        else l4.pop_front();
    }
    finish = clock();
    std::cout << "XorList with std::allocator: " << (finish - start) / 1000.0 << std::endl;

}
