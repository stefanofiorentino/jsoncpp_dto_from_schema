/*
 * Copyright © 2011-2019 BTicino S.p.A.
 * 
 * This file is part of bt_drover.
 * 
 * bt_drover is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * bt_drover is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with bt_drover. If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 */
#ifndef JSONCPP_DTO_FROM_SCHEMA_JSONCPP_SUPPORT_HPP
#define JSONCPP_DTO_FROM_SCHEMA_JSONCPP_SUPPORT_HPP

#include <sstream>
#include <jsoncpp/json/json.h>

// SFINAE test
template<typename, typename T>
struct has_toJsonValue
{
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args>
struct has_toJsonValue<C, Ret(Args...)>
{
private:
    template<typename T>
    static constexpr auto
    check(T *) -> typename std::is_same<decltype(std::declval<T>().toJsonValue(std::declval<Args>()...)), Ret>::type;

    template<typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

public:
    static constexpr bool value = type::value;
};

inline Json::Value getJsonValueFromString(std::string const &in)
{
    Json::Value root;
    Json::CharReaderBuilder jsonReader;
    std::string errs;
    std::stringstream inStream(in);
    if (!Json::parseFromStream(jsonReader, inStream, &root, &errs))
    {
        throw std::runtime_error(errs);
    }
    return root;
}

template<typename T>
inline typename std::enable_if<std::is_constructible<T, Json::Value>::value &&
                               has_toJsonValue<T, Json::Value(void)>::value, std::vector<T>>::type
fromJsonStringToBeanVector(std::string const &jsonString)
{
    std::vector<T> returnSceneBeanVector;
    if (jsonString.empty())
    {
        return returnSceneBeanVector;
    }
    Json::Value const root = getJsonValueFromString(jsonString);
    auto type = root.type();
    if (type == Json::arrayValue)
    {
        for (Json::ArrayIndex i = 0; i < root.size(); ++i)
        {
            returnSceneBeanVector.push_back(T(root[i]));
        }
    }
    else
    {
        throw std::runtime_error("jsonString is not an array: use fromJsonStringToSceneBean instead.");
    }
    return returnSceneBeanVector;
}


template<typename T>
inline typename std::enable_if<!std::is_same<T, int>::value &&
!std::is_same<T, double>::value &&
!std::is_same<T, std::string>::value &&
!std::is_same<T, std::vector<std::string>>::value, T>::type
getGenericValue(Json::Value const &root, std::string const &member)
{
    return T(root[member]);
}

template<typename INT>
inline typename std::enable_if<std::is_same<INT, int>::value, INT>::type
getGenericValue(Json::Value const &root, std::string const &member)
{
    if (root.isMember(member))
    {
        if (root[member].isInt())
        {
            return root[member].asInt();
        }
    }
}

template<typename DOUBLE>
inline typename std::enable_if<std::is_same<DOUBLE, double>::value, DOUBLE>::type
getGenericValue(Json::Value const &root, std::string const &member)
{
    if (root.isMember(member))
    {
        if (root[member].isInt())
        {
            return root[member].asDouble();
        }
    }
}

template<typename STRING>
inline typename std::enable_if<std::is_same<STRING, std::string>::value, STRING>::type
getGenericValue(Json::Value const &root, std::string const &member)
{
    if (root.isMember(member))
    {
        if (root[member].isString())
        {
            return root[member].asString();
        }
    }
}

template<typename STRING_VECTOR>
inline typename std::enable_if<std::is_same<STRING_VECTOR, std::vector<std::string>>::value, STRING_VECTOR>::type
getGenericValue(Json::Value const &root, std::string const &member)
{
    STRING_VECTOR vec;
    if (root.isMember(member))
    {
        if (root[member].isArray())
        {
            if (!root[member].empty())
            {
                for (auto it = 0u; it < root[member].size(); ++it)
                {
                    if (root[member][it].isString())
                    {
                        vec.push_back(root[member][it].asString());
                    }
                }
            }
        }
    }
    return vec;
}

#endif //JSONCPP_DTO_FROM_SCHEMA_JSONCPP_SUPPORT_HPP
