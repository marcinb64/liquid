#include <catch2/catch_test_macros.hpp>

#include <string.h>
#include <util.h>
#include <vector>

using namespace liquid;

TEST_CASE("Result<T, E> ok")
{
    Result<float, int> r = Result<float, int>::ok(1.2f);

    CHECK(r.isSuccess() == true);
    CHECK(r == true);
    CHECK(r.getValue() == 1.2f);
    CHECK(r.isError() == false);
}

TEST_CASE("Result<T, E> err")
{
    Result<float, int> r = Result<float, int>::err(99);

    CHECK(r.isSuccess() == false);
    CHECK(r == false);
    CHECK(r.isError() == true);
    CHECK(r.getError() == 99);
}

TEST_CASE("Result<void, E> ok")
{
    Result<void, int> r = Result<void, int>::ok();

    CHECK(r.isSuccess() == true);
    CHECK(r == true);
    CHECK(r.isError() == false);
}

TEST_CASE("Result<void, E> err")
{
    Result<void, int> r = Result<void, int>::err(99);

    CHECK(r.isSuccess() == false);
    CHECK(r == false);
    CHECK(r.isError() == true);
    CHECK(r.getError() == 99);
}

auto f1(int x, std::vector<int> &callOrder) -> Result<int, int>
{
    callOrder.emplace_back(1);
    if (x != 0)
        return Result<int, int>::err(88);
    else
        return Result<int, int>::ok(x + 10);
}

auto f2(char ch, std::vector<int> &callOrder) -> Result<char, int>
{
    callOrder.emplace_back(2);
    return Result<char, int>::ok(ch + 1);
}

TEST_CASE("Result chaining - success, then error")
{
    std::vector<int> callOrder;

    auto r12 = f1(9, callOrder).andThen<char>([&]() {
        return f2('A', callOrder);
    });
    CHECK(r12.isError() == true);
    CHECK(r12.getError() == 88);

    CHECK(callOrder.size() == 1);
    CHECK(callOrder[0] == 1);
}

TEST_CASE("Result chaining - 2 successes")
{
    std::vector<int> callOrder;

    auto r12 = f1(0, callOrder).andThen<char>([&]() {
        return f2('A', callOrder);
    });
    CHECK(r12.isSuccess() == true);
    CHECK(r12.getValue() == 'B');

    CHECK(callOrder.size() == 2);
    CHECK(callOrder[0] == 1);
    CHECK(callOrder[1] == 2);
}

auto f3(int x, int order, std::vector<int> &callOrder) -> Result<void, int>
{
    callOrder.emplace_back(order);
    if (x != 0)
        return Result<void, int>::err(88);
    else
        return Result<void, int>::ok();
}

TEST_CASE("Result chaining (void) - 2 successes")
{
    std::vector<int> callOrder;

    auto r12 = f3(0, 1, callOrder).andThen<void>([&]() {
        return f3(0, 2, callOrder);
    });
    CHECK(r12.isSuccess() == true);
    
    CHECK(callOrder.size() == 2);
    CHECK(callOrder[0] == 1);
    CHECK(callOrder[1] == 2);
}