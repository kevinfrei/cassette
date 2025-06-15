#include <crow/json.h>
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// #include "CommonTypes.hpp"

/****
Conversion to JSON stuff
****/

// By default, simple stuff can be converted to json
template <typename T>
crow::json::wvalue to_json(const T& value) {
  return crow::json::wvalue{value};
}

// A little extra work for string constants:
crow::json::wvalue to_json(const char* value) {
  return crow::json::wvalue(std::string(value));
}

// Handle vector<T> explicity...
template <typename T>
crow::json::wvalue to_json(const std::vector<T>& value) {
  crow::json::wvalue vec{std::vector<crow::json::wvalue>()};
  for (size_t i = 0; i < value.size(); i++) {
    vec[i] = to_json(value[i]);
  }
  return vec;
}

// Handle char's specifically:
template <>
crow::json::wvalue to_json<char>(const char& value) {
  return crow::json::wvalue(std::string(1, value));
}

// Tuples are just arrays which the right size & type
template <typename Tuple, size_t Index, typename T>
void to_json_tuple_element_helper(const Tuple& tuple,
                                  crow::json::wvalue& container) {
  // std::cout << "Element " << Index << ": " << typeid(T).name() << std::endl;
  container[Index] = to_json(std::get<Index>(tuple));
}

template <typename Tuple, size_t... Is>
void to_json_tuple_recurse(const Tuple& tuple,
                           crow::json::wvalue& json_list,
                           std::index_sequence<Is...>) {
  ((to_json_tuple_element_helper<Tuple, Is, std::tuple_element_t<Is, Tuple>>(
       tuple, json_list)),
   ...);
}

template <typename... Args>
crow::json::wvalue to_json(const std::tuple<Args...>& value) {
  crow::json::wvalue vec{std::vector<crow::json::wvalue>()};
  to_json_tuple_recurse(
      value, vec, std::make_index_sequence<sizeof...(Args)>{});
  return vec;
}

// I didn't need this for Windows, but I do for Linux/Mac because the map
// type is std::tuple instead of std::pair in glibcxx
template <typename Iter>
crow::json::wvalue to_json_vec_pair(const Iter& begin, const Iter& end) {
  std::vector<crow::json::wvalue> vec;
  for (auto it = begin; it != end; ++it) {
    crow::json::wvalue pair{std::vector<crow::json::wvalue>()};
    auto& [first, second] = *it;
    pair[0] = to_json(first);
    pair[1] = to_json(second);
    vec.push_back(pair);
  }
  return vec;
}

// My pickling framework sends Maps as this (not just objects: They're TS
// Map<K,V>)
// {"@dataType":"freik.Map","@dataValue":[["a",1],["c",2],["b",3]]}
template <typename K, typename V>
crow::json::wvalue to_json(const std::map<K, V>& value) {
  crow::json::wvalue v;
  v["@dataType"] = "freik.Map";
  v["@dataValue"] = to_json_vec_pair(value.begin(), value.end());
  return v;
}
template <typename K, typename V>
crow::json::wvalue to_json(const std::unordered_map<K, V>& value) {
  crow::json::wvalue v;
  v["@dataType"] = "freik.Map";
  v["@dataValue"] = to_json_vec_pair(value.begin(), value.end());
  return v;
}

// My pickling framework sends Sets as this (not just objects: They're TS
// Set<K>'s)
// {"@dataType":"freik.Set","@dataValue":["a", "c", "b"]}
template <typename T>
crow::json::wvalue to_json(const std::unordered_set<T>& value) {
  crow::json::wvalue v;
  v["@dataType"] = "freik.Set";
  std::vector<T> flat;
  flat.reserve(value.size());
  flat.assign(value.begin(), value.end());
  v["@dataValue"] = to_json(flat);
  return v;
}
template <typename T>
crow::json::wvalue to_json(const std::set<T>& value) {
  crow::json::wvalue v;
  v["@dataType"] = "freik.Set";
  std::vector<T> flat;
  flat.reserve(value.size());
  flat.assign(value.begin(), value.end());
  v["@dataValue"] = to_json(flat);
  return v;
}

/*
template <typename T>
crow::json::wvalue to_json(T value) {
    std::enable_if_t<std::is_enum_v<T> && !std::is_convertible_v<T, typename
std::underlying_type_t<T>>> return to_json(static_cast<typename
std::underlying_type_t<T>>(value));
}
*/

/****
Conversion from JSON stuff
****/

// By default, we can't read a random value from a json value
// (i.e. everything must be specialized) and it's easier to
// do this with a partial specialization of a struct :/
template <typename T>
struct from_json_impl {
  static std::optional<T> process(const crow::json::rvalue& json) {
    return std::nullopt;
  }
};

template <typename T>
std::optional<T> from_json(const crow::json::rvalue& json) {
  return from_json_impl<T>::process(json);
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

// std::vector specialization:
template <typename T>
struct from_json_impl<std::vector<T>> {
  using value_type = T;
  static std::optional<std::vector<value_type>> process(
      const crow::json::rvalue& json) {
    if (json.t() != crow::json::type::List) {
      return std::nullopt;
    }
    std::vector<value_type> vec;
    vec.reserve(json.size());
    for (size_t i = 0; i < json.size(); i++) {
      auto item = from_json<value_type>(json[i]);
      if (!item) {
        return std::nullopt;
      }
      vec.push_back(*item);
    }
    return vec;
  }
};

// std::tuple specialization:
// TODO: Explain this, as it is rather messsy.
template <typename... Args>
struct from_json_impl<std::tuple<Args...>> {
 private:
  using tup_type = std::tuple<Args...>;
  template <size_t Index, typename T>
  static std::optional<T> element_helper(tup_type& tuple,
                                         bool& failed,
                                         const crow::json::rvalue& container) {
    // std::cout << "Element " << Index << ": " << typeid(T).name() <<
    // std::endl;
    std::optional<T> res = from_json<T>(container[Index]);
    failed = failed || !res.has_value();
    if (res.has_value()) {
      std::get<Index>(tuple) = res.value();
    }
    return res;
  }

  template <size_t... Is>
  static std::optional<tup_type> recurse(const crow::json::rvalue& container,
                                         std::index_sequence<Is...>) {
    tup_type res;
    bool failed = false;
    ((element_helper<Is, std::tuple_element_t<Is, tup_type>>(
         res, failed, container)),
     ...);
    if (failed)
      return std::nullopt;
    return res;
  }

 public:
  static std::optional<tup_type> process(const crow::json::rvalue& json) {
    if (json.t() != crow::json::type::List) {
      return std::nullopt;
    }
    // Check exact tuple size
    if (json.size() != sizeof...(Args)) {
      return std::nullopt;
    }
    tup_type t;
    return recurse(json, std::make_index_sequence<sizeof...(Args)>{});
  }
};

// std::set specialization:
template <template <typename...> class SetType, typename Elem>
struct set_helper_from_json {
  static std::optional<SetType<Elem>> process(const crow::json::rvalue& json) {
    if (json.t() != crow::json::type::Object || json.size() != 2) {
      return std::nullopt;
    }
    if (json["@dataType"].s() != "freik.Set") {
      return std::nullopt;
    }
    auto dataValue = json["@dataValue"];
    if (dataValue.t() != crow::json::type::List) {
      return std::nullopt;
    }
    SetType<Elem> m;
    for (const auto& item : dataValue) {
      auto value = from_json<Elem>(item);
      if (!value) {
        return std::nullopt;
      }
      m.insert(*value);
    }
    return m;
  }
};

// std::set specialization
template <typename Elem>
struct from_json_impl<std::set<Elem>> {
  static std::optional<std::set<Elem>> process(const crow::json::rvalue& json) {
    return set_helper_from_json<std::set, Elem>::process(json);
  }
};

// std::unordered_set specialization
template <typename Elem>
struct from_json_impl<std::unordered_set<Elem>> {
  static std::optional<std::unordered_set<Elem>> process(
      const crow::json::rvalue& json) {
    return set_helper_from_json<std::unordered_set, Elem>::process(json);
  }
};

// std::set specialization:
template <template <typename...> class MapType, typename K, typename V>
struct map_helper_from_json {
  static std::optional<MapType<K, V>> process(const crow::json::rvalue& json) {
    if (json.t() != crow::json::type::Object || json.size() != 2) {
      return std::nullopt;
    }
    if (json["@dataType"].s() != "freik.Map") {
      return std::nullopt;
    }
    auto dataValue = json["@dataValue"];
    if (dataValue.t() != crow::json::type::List) {
      return std::nullopt;
    }
    MapType<K, V> m;
    for (const auto& item : dataValue) {
      if (item.t() != crow::json::type::List || item.size() != 2) {
        return std::nullopt;
      }
      auto key = from_json<K>(item[0]);
      auto value = from_json<V>(item[1]);
      if (!key || !value) {
        return std::nullopt;
      }
      m[*key] = *value;
    }
    return m;
  }
};

// std::map specialization
template <typename K, typename V>
struct from_json_impl<std::map<K, V>> {
  static std::optional<std::map<K, V>> process(const crow::json::rvalue& json) {
    return map_helper_from_json<std::map, K, V>::process(json);
  }
};

// std::unordered_set specialization
template <typename K, typename V>
struct from_json_impl<std::unordered_map<K, V>> {
  static std::optional<std::unordered_map<K, V>> process(
      const crow::json::rvalue& json) {
    return map_helper_from_json<std::unordered_map, K, V>::process(json);
  }
};

/*
template <typename T>
std::optional<T> from_json(std::istream& is, T& value) {
    std::enable_if_t<std::is_enum_v<T> && !std::is_convertible_v<T, typename
std::underlying_type_t<T>>> typename std::underlying_type_t<T> underlyingValue;
    is >> underlyingValue;
    value = static_cast<T>(underlyingValue);
}
*/
