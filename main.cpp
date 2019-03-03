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

    std::ifstream json_schema_file("schemas/product.tags.json.schema");
    std::string json_schema((std::istreambuf_iterator<char>(json_schema_file)), std::istreambuf_iterator<char>());

    auto json_value_schema = getJsonValueFromString(json_schema);

    Context ctx;
    ctx.add("__not_to_escape", "");
    ctx.add("__remove_trailing_comma", "");
    auto json_schema_title = json_value_schema["title"].asString();
    ctx.add("json_schema_title", json_schema_title);
    ctx.add("json_schema_description", json_value_schema["description"].asString());

    CollectionType properties;
    CollectionType toJsonValue_properties;
    CollectionType fromJsonValue_properties;
    bool needsVector = false;
    for (auto const &property : json_value_schema["properties"].getMemberNames())
    {
        ObjectType objectProperty;
        objectProperty["description"] = json_value_schema["properties"][property]["description"].asString();
        if ("integer" == json_value_schema["properties"][property]["type"].asString())
        {
            // TODO: manage exclusiveMinimum
            objectProperty["type"] = "int";
        }
        else if ("string" == json_value_schema["properties"][property]["type"].asString())
        {
            objectProperty["type"] = "std::string";
        }
        else if ("array" == json_value_schema["properties"][property]["type"].asString())
        {
            needsVector = true;
            // TODO: manage minItems
            // TODO: manage uniqueItems
            if ("integer" == json_value_schema["properties"][property]["type"].asString())
            {
                objectProperty["type"] = "std::vector<int>";
            }
            else if ("string" == json_value_schema["properties"][property]["items"]["type"].asString())
            {
                objectProperty["type"] = "std::vector<std::string>";

            }
        }
        objectProperty["name"] = property;
        properties.push_back(objectProperty);

        ObjectType toJsonValueProperty;
        toJsonValueProperty["property"] = "";
        if ("array" == json_value_schema["properties"][property]["type"].asString())
        {
            toJsonValueProperty["property"] += "\t\tif (!";
            toJsonValueProperty["property"] += json_schema_title;
            toJsonValueProperty["property"] += "::";
            toJsonValueProperty["property"] += property;
            toJsonValueProperty["property"] += ".empty())\n";
            toJsonValueProperty["property"] += "\t\t{ jsonValue[\"";
            toJsonValueProperty["property"] += property;
            toJsonValueProperty["property"] += "\"] = Json::arrayValue; }\n";
            toJsonValueProperty["property"] += "\t\tfor (auto const& s : ";
            toJsonValueProperty["property"] += json_schema_title;
            toJsonValueProperty["property"] += "::";
            toJsonValueProperty["property"] += property;
            toJsonValueProperty["property"] += ")\n";
            toJsonValueProperty["property"] += "\t\t{ jsonValue[\"";
            toJsonValueProperty["property"] += property;
            toJsonValueProperty["property"] += "\"].append(s); }\n";
        }
        else
        {
            toJsonValueProperty["property"] += "\t\tjsonValue[\"";
            toJsonValueProperty["property"] += property;
            toJsonValueProperty["property"] += "\"] = ";
            toJsonValueProperty["property"] += json_schema_title;
            toJsonValueProperty["property"] += "::";
            toJsonValueProperty["property"] += property;
            toJsonValueProperty["property"] += ";\n";
        }
        toJsonValue_properties.push_back(toJsonValueProperty);

        ObjectType fromJsonValueProperty;
        fromJsonValueProperty["property"] = property;
        fromJsonValueProperty["property"] += "(getGenericValue<";
        fromJsonValueProperty["property"] += objectProperty["type"];
        fromJsonValueProperty["property"] += ">(root, \"";
        fromJsonValueProperty["property"] += property;
        fromJsonValueProperty["property"] += "\"))";
        fromJsonValue_properties.push_back(fromJsonValueProperty);
    }

    if (needsVector)
    { ctx.add("needsVector", "#include <vector>\n"); }
    ctx.add("properties", properties);
    ctx.add("toJsonValue_properties", toJsonValue_properties);
    ctx.add("fromJsonValue_properties", fromJsonValue_properties);

    std::ifstream json_template_file("templates/class.hpp.template");
    std::string json_template((std::istreambuf_iterator<char>(json_template_file)), std::istreambuf_iterator<char>());

    template_t t;
    string result = t.render(json_template, ctx);

    if (auto yes = freopen((json_schema_name + ".hpp").c_str(), "w", stdout))
    { std::cout << result; }


    return 0;
}