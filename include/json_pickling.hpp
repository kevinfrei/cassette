#include <crow/json.h>
#include <cstdint>
#include <optional>
#include <string>

// #include "CommonTypes.hpp"

// The unspecialized template functions:

// By default, we can't read a random value from a json value
// (i.e. everything must be specialized)
template <typename T>
std::optional<T> from_json(const crow::json::rvalue& json) {
  return std::nullopt;
}

// By default, simple stuff can be converted to json
template <typename T>
crow::json::wvalue to_json(const T& value) {
  return crow::json::wvalue(static_cast<T>(value));
}

// Chars are a little weird, cuz, Javascript
template <>
std::optional<char> from_json<char>(const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::String) {
    return std::nullopt;
  }
  std::string str = json.s();
  if (str.size() != 1) {
    return std::nullopt;
  }
  return static_cast<char>(str[0]);
}

template <>
std::optional<uint8_t> from_json<uint8_t>(const crow::json::rvalue& json) {
  if (json.nt() != crow::json::num_type::Unsigned_integer) {
    return std::nullopt;
  }
  return static_cast<uint8_t>(json.u());
}

template <>
std::optional<int8_t> from_json<int8_t>(const crow::json::rvalue& json) {
  if (json.nt() == crow::json::num_type::Floating_point) {
    return std::nullopt;
  }
  return static_cast<int8_t>(json.u());
}

template <>
std::optional<uint16_t> from_json<uint16_t>(const crow::json::rvalue& json) {
  if (json.nt() != crow::json::num_type::Unsigned_integer) {
    return std::nullopt;
  }
  return static_cast<uint16_t>(json.u());
}

template <>
std::optional<int16_t> from_json<int16_t>(const crow::json::rvalue& json) {
  if (json.nt() == crow::json::num_type::Floating_point) {
    return std::nullopt;
  }
  return static_cast<int16_t>(json.u());
}

template <>
std::optional<uint32_t> from_json<uint32_t>(const crow::json::rvalue& json) {
  if (json.nt() != crow::json::num_type::Unsigned_integer) {
    return std::nullopt;
  }
  return static_cast<uint32_t>(json.u());
}

template <>
std::optional<int32_t> from_json<int32_t>(const crow::json::rvalue& json) {
  if (json.nt() == crow::json::num_type::Floating_point) {
    return std::nullopt;
  }
  return static_cast<int32_t>(json.u());
}

template <>
std::optional<uint64_t> from_json<uint64_t>(const crow::json::rvalue& json) {
  if (json.nt() != crow::json::num_type::Unsigned_integer) {
    return std::nullopt;
  }
  return static_cast<uint64_t>(json.u());
}

template <>
std::optional<int64_t> from_json<int64_t>(const crow::json::rvalue& json) {
  if (json.nt() == crow::json::num_type::Floating_point) {
    return std::nullopt;
  }
  return static_cast<int64_t>(json.u());
}

template <>
std::optional<float> from_json<float>(const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::Number) {
    return std::nullopt;
  }
  return static_cast<float>(json.d());
}

template <>
std::optional<double> from_json<double>(const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::Number) {
    return std::nullopt;
  }
  return static_cast<double>(json.d());
}

template <>
std::optional<bool> from_json<bool>(const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::True &&
      json.t() != crow::json::type::False) {
    return std::nullopt;
  }
  return json.t() == crow::json::type::True;
}

template <>
std::optional<std::string> from_json<std::string>(
    const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::String) {
    return std::nullopt;
  }
  return std::string(json.s());
}

// Handle vector<T> explicity...
template <typename T>
crow::json::wvalue to_json(const std::vector<T>& value) {
  crow::json::wvalue v = value;
  return v;
}

// Handle char's specifically:
template <>
crow::json::wvalue to_json<char>(const char& value) {
  return crow::json::wvalue(std::string(1, value));
}

// Tuples are just arrays which the right size & types:
template <typename... Args>
crow::json::wvalue to_json(const std::tuple<Args...>& value) {
  crow::json::wvalue v = value;
  return v;
}
template <typename... Args>
std::optional<std::tuple<Args...>> from_json(const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::Array) {
    return std::nullopt;
  }
  // Check exact tuple size
  if (json.size() != sizeof...(Args)) {
    return std::nullopt;
  }
  std::tuple<Args...> t;
  for (size_t i = 0; i < sizeof...(Args); ++i) {
    auto item =
        from_json<typename std::tuple_element<i, std::tuple<Args...>>::type>(
            json[i]);
    if (!item) {
      return std::nullopt;
    }
    std::get<i>(t) = *item;
  }
  return t;
}

// TODO: Support for maps, sets, and all 3 flavors of enums:
// plain, string, and numeric

// My pickling framework sends Maps as this (not just objects: They're TS
// Map<K,V>)
// {"@dataType":"freik.Map","@dataValue":[["a",1],["c",2],["b",3]]}
template <typename K, typename V>
crow::json::wvalue to_json(const std::map<K, V>& value) {
  crow::json::wvalue v;
  v["@dataType"] = "freik.Map";
  // Does this convert to an array of 2-tuples?
  v["@dataValue"] = to_json(value);
  return v;
}

template <typename K, typename V>
std::optional<std::map<K, V>> from_json(const crow::json::rvalue& json) {
  if (json.t() != crow::json::type::Object || json.size() != 2) {
    return std::nullopt;
  }
  if (json["@dataType"].s() != "freik.Map") {
    return std::nullopt;
  }
  auto dataValue = json["@dataValue"];
  if (dataValue.t() != crow::json::type::Array) {
    return std::nullopt;
  }
  std::map<K, V> m;
  for (const auto& item : dataValue) {
    auto key = from_json<K>(item[0]);
    auto value = from_json<V>(item[1]);
    if (!key || !value) {
      return std::nullopt;
    }
    m[*key] = *value;
  }
  return m;
}
