import {
  ArrType,
  Simple,
  Enum,
  Int,
  MapType,
  NEnum,
  ObjType,
  SEnum,
  SetType,
  TupType,
  Types,
} from '../../www/Shared/IDL';
import { FileGenerator, MakeGenerator, EmitItem, Emitter } from './api';

async function header(writer: Bun.FileSink): Promise<void> {
  await writer.write(`// Generated from www/Shared/Enums.ts by scripts/gencpp.ts

#ifndef SHARED_CONSTANTS_HPP
#define SHARED_CONSTANTS_HPP

#pragma once

#include <cstdint>
#include <string_view>
#include <optional>
#include <vector>
#include <set>
#include <map>
#include <tuple>
#include <string>

namespace Shared {
`);
}

async function footer(writer: Bun.FileSink): Promise<void> {
  await writer.write(`

} // namespace Shared

#endif // SHARED_CONSTANTS_HPP
`);
}

function getTypeName(type: Types): string {
  if (typeof type === 'string') {
    switch (type) {
      case '0':
        return 'std::uint8_t';
      case '1':
        return 'std::int8_t';
      case '2':
        return 'std::uint16_t';
      case '3':
        return 'std::int16_t';
      case '4':
        return 'std::uint32_t';
      case '5':
        return 'std::int32_t';
      case '6':
        return 'std::uint64_t';
      case '7':
        return 'std::int64_t';
      case 's':
        return 'std::string';
      case 'c':
        return 'char';
      case 'b':
        return 'bool';
      case 'f':
        return 'float';
      case 'd':
        return 'double';
      default:
        throw new Error(`Unknown type: ${type}`);
    }
  }
  if (type.t === 'R') {
    return type.r; // Reference type, just return the name
  }
  if (type.t === 'A') {
    return `std::vector<${getTypeName(type.d)}>`;
  }
  if (type.t === 'S') {
    return `std::set<${getTypeName(type.d)}>`;
  }
  if (type.t === 'M') {
    return `std::map<${getTypeName(type.k)}, ${getTypeName(type.v)}>`;
  }
  if (type.t === 'T') {
    return `std::tuple<${type.l.map(getTypeName).join(', ')}>`;
  }
  throw new Error(`Unsupported unnamed type: ${JSON.stringify(type)}`);
}

const enumType: EmitItem<Enum> = async (writer, name, item) => {
  const typeName = item.u === '8' ? '' : `: ${getTypeName(item.u)}`;
  await writer.write(`
  enum class ${name}${typeName} {
${Object.entries(item.v)
  .map((key) => `    ${key},`)
  .join('\n')}
  };`);
};

const numEnumType: EmitItem<NEnum> = async (writer, name, item) => {
  const typeName = item.u === '8' ? '' : `: ${getTypeName(item.u)}`;
  await writer.write(`
  enum class ${name}:${typeName} {
${Object.entries(item.v)
  .map(([key, value]) => `    ${key} = ${value},`)
  .join('\n')}
  };`);
};

const strEnumType: EmitItem<SEnum> = async (writer, name, item) => {
  await writer.write(`
  // ${name}
  enum class ${name} {
${Object.keys(item.v)
  .map((key) => `    ${key},`)
  .join('\n')}
  };
`);

  await writer.write(`
  constexpr std::string_view get_string(${name} _value) {
    switch (_value) {
${Object.entries(item.v)
  .map(([key, val]) => `      case ${name}::${key}: return "${val}";`)
  .join('\n')}
    }
    return "<unknown>";
  }
`);

  // This is *super* simplistic, and could be optimized in various ways...
  await writer.write(`
  constexpr std::optional<${name}> string_to_${name}(const std::string_view &str) {
${Object.entries(item.v)
  .map(([key, val]) => `    if (str == "${val}") return ${name}::${key};`)
  .join('\n')}
    return std::nullopt;
  }
`);
};

function NYI<T>(name: string): EmitItem<T> {
  return async () => {
    console.error(`${name} not implemented in CppEmitter`);
    return Promise.resolve();
  };
}

const objType: EmitItem<ObjType> = async (writer, name, item) => {
  await writer.write(`
  struct ${name} {
  `);
  for (const [key, value] of Object.entries(item.d)) {
    const typeName = getTypeName(value);
    await writer.write(`    ${typeName} ${key};\n`);
  }
  await writer.write(`
  };\n`);
};

const arrType: EmitItem<ArrType> = async (writer, name, item) => {
  await writer.write(`
  using ${name} = std::vector<${getTypeName(item.d)}>;
  `);
};
const setType: EmitItem<SetType> = async (writer, name, item) => {
  await writer.write(`
  using ${name} = std::set<${getTypeName(item.d)}>;
  `);
};
const mapType: EmitItem<MapType> = async (writer, name, item) => {
  await writer.write(`
  using ${name} = std::map<${getTypeName(item.k)}, ${getTypeName(item.v)}>;
  `);
};
const tupType: EmitItem<TupType> = async (writer, name, item) => {
  await writer.write(`
  using ${name} = std::tuple<${item.l.map(getTypeName).join(', ')}>;
  `);
};
export const CppEmitter: Emitter = {
  header,
  footer,
  types: {
    objType,
    arrType,
    setType,
    mapType,
    tupType,
    enumType,
    numEnumType,
    strEnumType,
  },
  fields: {},
};

export function GetCppGenerator(): FileGenerator {
  // Returns the CppEmitter instance
  return MakeGenerator(CppEmitter);
}
