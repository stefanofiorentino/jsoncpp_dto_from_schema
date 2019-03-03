#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <cmake-build-debug/product.hpp>

SCENARIO("product dto", "[dto]")
{
    GIVEN("the generated header")
    {
        Product p;
        p.set_productName("sensor 42");
        REQUIRE("sensor 42" == p.get_productName());
        p.set_productId(42);
        auto id = p.get_mutable_productId();
        REQUIRE(42 == id);
        id = ++p.get_mutable_productId();
        REQUIRE(43 == id);
        id = --p.get_mutable_productId();
        REQUIRE(42 == id);

        auto s = p.toJsonValue().toStyledString();
        REQUIRE(!s.empty());
        REQUIRE_NOTHROW(std::cout << s << std::endl);
    }
}
