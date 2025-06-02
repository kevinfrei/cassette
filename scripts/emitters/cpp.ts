import {
  ArrType,
  Atoms,
  Enum,
  Int,
  MapType,
  NEnum,
  ObjType,
  SEnum,
  SetType,
  TupType,
} from '../../www/Shared/IDL';
import { EmitItem, Emitter } from './interface';

async function header(writer: Bun.FileSink): Promise<void> {
  await writer.write(`// Generated from www/Shared/Enums.ts by scripts/gencpp.ts

#ifndef SHARED_CONSTANTS_HPP
#define SHARED_CONSTANTS_HPP

#pragma once

#include <string_view>
#include <optional>

namespace Shared {
`);
}

async function footer(writer: Bun.FileSink): Promise<void> {
  await writer.write(`

} // namespace Shared

#endif // SHARED_CONSTANTS_HPP
`);
}

function getTypeName(type: Atoms): string {
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
    case '8':
      return ''; // For Enum types, so it's empty
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
  writer.write(`
  // ${name}
  enum class ${name} {
${Object.keys(item.v)
  .map((key) => `    ${key},`)
  .join('\n')}
  };
`);

  writer.write(`
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
  writer.write(`
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

export const CppEmitter: Emitter = {
  header,
  footer,
  types: {
    objType: NYI<ObjType>('objType'),
    arrType: NYI<ArrType>('arrType'),
    setType: NYI<SetType>('setType'),
    mapType: NYI<MapType>('mapType'),
    tupType: NYI<TupType>('tupType'),
    enumType,
    numEnumType,
    strEnumType,
  },
};
