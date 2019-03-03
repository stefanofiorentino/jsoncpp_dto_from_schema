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

void populateCollections(const std::string &json_schema_title,
                         const Json::Value &json_value_schema, Context &ctx);

int main()
{
    string json_schema_name = "product";

    std::ifstream json_schema_file("schemas/product.nested.json.schema");
    std::string json_schema((std::istreambuf_iterator<char>(json_schema_file)),
                            std::istreambuf_iterator<char>());

    auto json_value_schema = getJsonValueFromString(json_schema);

    Context ctx;
    ctx.add("__not_to_escape", "");
    ctx.add("__remove_trailing_comma", "");
    auto json_schema_title = json_value_schema["title"].asString();
    ctx.add("json_schema_title", json_schema_title);
    ctx.add("json_schema_description",
            json_value_schema["description"].asString());

    populateCollections(json_schema_title, json_value_schema, ctx);

    std::ifstream json_template_file("templates/class.hpp.template");
    std::string json_template(
            (std::istreambuf_iterator<char>(json_template_file)),
            std::istreambuf_iterator<char>());

    template_t t;
    string result = t.render(json_template, ctx);

    if (freopen((json_schema_name + ".hpp").c_str(), "w", stdout))
    { std::cout << result; }


    return 0;
}

void populateCollections(const std::string &json_schema_title,
                         const Json::Value &json_value_schema, Context &ctx)
{
    CollectionType properties;
    CollectionType toJsonValue_properties;
    CollectionType fromJsonValue_properties;
    CollectionType nested_type_properties;

    for (auto const &property : json_value_schema["properties"].getMemberNames())
    {
        ObjectType objectProperty;
        auto property_description_string = json_value_schema["properties"][property]["description"].asString();
        objectProperty["description"] = property_description_string;
        auto property_type_string = json_value_schema["properties"][property]["type"].asString();
        if ("integer" == property_type_string)
        {
            objectProperty["type"] = "int";
        }
        if ("number" == property_type_string)
        {
            objectProperty["type"] = "double";
        }
        else if ("string" == property_type_string)
        {
            objectProperty["type"] = "std::string";
        }
        else if ("array" == property_type_string)
        {
            if ("integer" == property_type_string)
            {
                objectProperty["type"] = "std::vector<int>";
            }
            else if ("string" ==
                     json_value_schema["properties"][property]["items"]["type"].asString())
            {
                objectProperty["type"] = "std::vector<std::string>";

            }
        }
        else if ("object" == property_type_string)
        {
            Context _ctx;
            objectProperty["type"] = property + "_type";
            populateCollections(json_schema_title,
                                json_value_schema["properties"][property],
                                _ctx);

            ObjectType _property;
            _property["property"] = objectProperty["type"];
            nested_type_properties.push_back(_property);
        }
        objectProperty["name"] = property;
        properties.push_back(objectProperty);

        if ("object" != property_type_string)
        {
            ObjectType fromJsonValueProperty;
            fromJsonValueProperty["property"] = "\t\t";
            fromJsonValueProperty["property"] = json_schema_title;
            fromJsonValueProperty["property"] = "::";
            fromJsonValueProperty["property"] = property;
            fromJsonValueProperty["property"] += " = getGenericValue<";
            fromJsonValueProperty["property"] += objectProperty["type"];
            fromJsonValueProperty["property"] += ">(root, \"";
            fromJsonValueProperty["property"] += property;
            fromJsonValueProperty["property"] += "\");";
            fromJsonValue_properties.push_back(fromJsonValueProperty);
        }

        ObjectType toJsonValueProperty;
        toJsonValueProperty["property"] = "";
        if ("array" == property_type_string)
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
        else if ("object" != property_type_string)
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
    }

    ctx.add("properties", properties);
    ctx.add("toJsonValue_properties", toJsonValue_properties);
    ctx.add("fromJsonValue_properties", fromJsonValue_properties);
    ctx.add("nested_type_properties", nested_type_properties);
}