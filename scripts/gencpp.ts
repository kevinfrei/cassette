import { hasField } from '@freik/typechk';

// A script to generate C++ code from the SharedConstants.ts file
type SharedConstants<T> = {
  description: string;
  typechecker: boolean;
  type: [string, string];
  values: Record<string, T>;
};

async function generateCppConstants<T>(tc: SharedConstants<T>) {
  // file shared_constants.hpp

  // #ifndef SHARED_CONSTANTS_HPP
  // #define SHARED_CONSTANTS_HPP
  //
  // #pragma once
  //
  // #include <string_view>
  // #include <unordered_map>
  // #include <optional>
  //
  // namespace Shared {
  //   /* For numeric types: */
  //   // <description>
  //   enum class <name>: <int> {
  //     <keyName1> = <value1>,
  //     <keyName2> = <value2>,
  //   };
  //
  //  /* For string types: */
  //  // <description>
  //   enum class <name> {
  //     <keyName1>,
  //     <keyName2>,
  //     ...
  //   };
  //   constexpr std::string_view <name>_to_string(<name> _value) {
  //     switch (_value) {
  //       case <name>::<keyName1>:
  //         return "<keyName1>";
  //       case <name>::<keyName2>:
  //         return "<keyName2>";
  //     }
  //   }
  //   constexpr std::unordered_map<std::string_view, <name>> string_to_<name>_map = {
  //     {<name>_to_string(<name>::<KeyName1>), <name>::<keyName1>},
  //     {<name>_to_string(<name>::<KeyName2>), <name>::<keyName2>},
  //   };
  //   constexpr std::optional<<name>> string_to_<name>(std::string_view &str) {
  //     auto it = string_to_<name>_map.find(str);
  //     if (it != string_to_<name>_map.end()) {
  //       return it->second;
  //     }
  //     return std::nullopt;
  //   }
  //
  // }
  //
  // #endif // SHARED_CONSTANTS_HPP

  for (const keyName in tc.values) {
    if (hasField(tc.values, keyName)) {
      const value = tc.values[keyName];
      // Generate C++ code for each constant
    }
  }
}
