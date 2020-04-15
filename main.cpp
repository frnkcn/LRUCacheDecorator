#include "LRUCacheDecorator.h"

#include <iostream>

using namespace std;

struct PlusOne : LRUCacheDecorator<PlusOne, int>
{
    int compute(int x)
    {
        return x + 1;
    }

    int operator()(int x)
    {
        return get(x);
    }
};

struct TriSum : LRUCacheDecorator<TriSum, int>
{
    int compute(int x, int y, int z)
    {
        return x + y + z;
    }

    int operator()(int x, int y, int z)
    {
        return get(x, y, z);
    }
};

struct FunkyObj
{
    string s;
    uint32_t uint;
    int64_t lng;
};
struct FunkyFunc : LRUCacheDecorator<FunkyFunc, FunkyObj>
{
    FunkyObj compute(string const& str, uint32_t uint, int64_t lng)
    {
        return FunkyObj{ str, uint, lng };
    }

    FunkyObj operator()(string const& str, uint32_t uint, int64_t lng)
    {
        return get(str, uint, lng);
    }
};

int
main()
{
    auto tri_sum = TriSum{};
    for (auto i = 0; i < 10; ++i) {
        std::cout << tri_sum(i, i + 1, i + 2) << std::endl;
    }

    return 0;
}
