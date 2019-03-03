#include <iostream>
#include <sstream>
#include <jsoncpp_support/jsoncpp_support.hpp>
#include <plustache/plustache_types.hpp>
#include <plustache/template.hpp>

using std::string;
using Plustache::Context;
using Plustache::template_t;
using PlustacheTypes::ObjectType;
using PlustacheTypes::CollectionType;

int main()
{
    string json_schema_name = "product";
    
    std::ifstream json_schema_file("schemas/product.json.schema");
    std::string json_schema((std::istreambuf_iterator<char>(json_schema_file)), std::istreambuf_iterator<char>());

    auto json_value_schema = getJsonValueFromString(json_schema);

    Context ctx;
    ctx.add("json_schema_title", json_value_schema["title"].asString());
    ctx.add("json_schema_description", json_value_schema["description"].asString());

    CollectionType properties;
    for (auto const &property : json_value_schema["properties"].getMemberNames())
    {
        ObjectType objectProperty;
        objectProperty["description"] = "\n//" + json_value_schema["properties"][property]["description"].asString();
        if ("integer" == json_value_schema["properties"][property]["type"].asString())
        {
            objectProperty["type"] = "int";
        }
        else if ("string" == json_value_schema["properties"][property]["type"].asString())
        {
            objectProperty["type"] = "std::string";
        }
        objectProperty["name"] = property;

        properties.push_back(objectProperty);
    }
    ctx.add("properties", properties);

    std::ifstream json_template_file("templates/class.hpp.template");
    std::string json_template((std::istreambuf_iterator<char>(json_template_file)), std::istreambuf_iterator<char>());

    template_t t;
    string result = t.render(json_template, ctx);

    if (auto yes = freopen((json_schema_name + ".hpp").c_str(), "w", stdout))
    { std::cout << result; }


    return 0;
}