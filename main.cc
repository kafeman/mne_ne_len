#include <iostream>
#include <signal.h>
#include "arena-inl.h"

#define SOMETHING_BIG ((size_t)9999999999)

using ctemplate::ArenaAllocator;

void test_operator_new()
{
    UnsafeArena arena(1000);

    try {
        int *foo = new (AllocateInArena, &arena) int[SOMETHING_BIG];
        std::cout << "Получен указатель: " << foo << std::endl;
    } catch (const std::bad_alloc &) {
        std::cout << "Поймали bad_alloc" << std::endl;
    }
}

void test_vector()
{
    UnsafeArena arena(1000);
    std::vector<int, ArenaAllocator<int, UnsafeArena>> v(&arena);

    try {
        v.resize(SOMETHING_BIG);
    } catch (const std::bad_alloc &) {
        std::cout << "Поймали bad_alloc" << std::endl;
    }
}

void err_handler(int)
{
    std::cout << "Похоже, я что-то не так сделал.\n"
              << "В документации к std::vector::resize сказано,\n"
              << "что allocate *может* кинуть исключение (may throw)\n"
              << "но это же не значит, что он обязан? (must throw).\n"
              << "И вообще, наверняка не из-за этого GMail падает... :-)"
              << std::endl;
    std::exit(0);
}

int main()
{
    struct sigaction handler;

    handler.sa_handler = err_handler;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags = 0;
    sigaction(SIGINT, &handler, NULL);
    sigaction(SIGSEGV, &handler, NULL);    

    test_operator_new();
    test_vector();

    return 0;
}
