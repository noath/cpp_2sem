#include <iostream>
#include "MemoryManager.h"

class A : public CAllocatedOn<CurrentMemoryManager> {
    int a;
};
class B : public CAllocatedOn<CurrentMemoryManager> {
    int a;
};
class C : public CAllocatedOn<RuntimeHeap> {
    int a;
};
class D : public CAllocatedOn<RuntimeHeap> {
    int a;
};

int main() {
    CMemoryManagerSwitcher::switch_allocator(new StackAllocator<int>());
    A* a = new A();
    B* b = new B();
    B* bb = new B();
    C* c = new C();
    delete c;
    CMemoryManagerSwitcher::switch_allocator(new MallocAllocator());
    D* d = new D();
    D* dd = new D();
    delete a;
    delete b;
    delete bb;
    delete d;
    delete dd;

    std::cout<<"Switch logs:\n";
    CMemoryManagerSwitcher::print_switch_log();
    std::cout<<"\nAllocations logs:\n";
    MemoryLogger::print_memory_use_log();
}