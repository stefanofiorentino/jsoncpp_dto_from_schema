#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <cmake-build-debug/product.hpp>

SCENARIO("populate product dto by setter", "[dto]")
{
    GIVEN("a clean instance of the product")
    {
        product p;
        p.set_productName("product 42");
        REQUIRE("product 42" == p.get_productName());
        p.set_productId(42);
        auto id = p.get_mutable_productId();
        REQUIRE(42 == id);
        id = ++p.get_mutable_productId();
        REQUIRE(43 == id);
        id = --p.get_mutable_productId();
        REQUIRE(42 == id);

        std::vector<std::string> vs;
        vs.emplace_back("one");
        vs.emplace_back("two");
        p.set_tags(vs);

        REQUIRE(!p.get_tags().empty());
        for (auto const &s : p.get_tags())
        { REQUIRE(std::find(std::begin(vs), std::end(vs), s) != std::end(vs)); }

        REQUIRE(vs.front() == "one");
        REQUIRE(vs.back() == "two");

        auto s = p.toJsonValue().toStyledString();
        REQUIRE(!s.empty());
        REQUIRE(p.toJsonValue() == getJsonValueFromString(s));

        auto s_direct = p.toStyledString();
        REQUIRE(!s_direct.empty());
        REQUIRE_NOTHROW(s == s_direct);

        auto s_to_string = to_string(p);
        REQUIRE(!s_to_string.empty());
        REQUIRE_NOTHROW(s == s_to_string);
    }
}

SCENARIO("populate product dto by json string", "[dto]")
{
    GIVEN("an instance of the product constructed by Json::Value")
    {
        std::string const &json_string = "{\n   \"productId\" : 42,\n   \"productName\" : \"product 42\",\n   \"tags\" : [ \"one\", \"two\" ]\n}\n";
        product p(getJsonValueFromString(json_string));

        auto s = p.toJsonValue().toStyledString();
        REQUIRE(!s.empty());
        REQUIRE(json_string == s);

        auto s_direct = p.toStyledString();
        REQUIRE(!s_direct.empty());
        REQUIRE(json_string == s_direct);

        auto s_to_string = to_string(p);
        REQUIRE(!s_to_string.empty());
        REQUIRE(json_string == s_to_string);
    }
}

SCENARIO("populate product dto with nested object by json string", "[dto]")
{
    GIVEN("an instance of the product constructed by Json::Value")
    {
        std::string const &json_string = "{\n   \"dimensions\" : {\n      \"height\" : 3,\n      \"length\" : 1,\n      \"width\" : 2\n   },\n   \"productId\" : 42,\n   \"productName\" : \"product 42\",\n   \"tags\" : [ \"one\", \"two\" ]\n}\n";
        auto const &jsonValue = getJsonValueFromString(json_string);
        auto const &jsonValueBackToString = jsonValue.toStyledString();

        REQUIRE(json_string == jsonValueBackToString);

        product p(jsonValue);
        auto s = p.toJsonValue().toStyledString();
        REQUIRE(!s.empty());
        REQUIRE(json_string == s);

        auto s_direct = p.toStyledString();
        REQUIRE(!s_direct.empty());
        REQUIRE(json_string == s_direct);

        auto s_to_string = to_string(p);
        REQUIRE(!s_to_string.empty());
        REQUIRE(json_string == s_to_string);
    }
}
