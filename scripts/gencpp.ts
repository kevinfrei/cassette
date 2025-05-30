import { hasFieldType } from '@freik/typechk';

import {
  AllConstants,
  chkSharedConstants,
  SharedConstants,
} from '../www/SharedConstants';

async function emitPreamble(stream: Bun.FileSink): Promise<void> {
  await stream.write(`// Generated from www/SharedConstants.ts by scripts/gencpp.ts

#ifndef SHARED_CONSTANTS_HPP
#define SHARED_CONSTANTS_HPP

#pragma once

#include <string_view>
#include <optional>

namespace Shared {
`);
}

async function emitPostamble(writer: Bun.FileSink): Promise<void> {
  await writer.write(`

} // namespace Shared

#endif // SHARED_CONSTANTS_HPP
`);
}

async function emitNumericConstants(
  writer: Bun.FileSink,
  name: string,
  item: SharedConstants<number>,
): Promise<void> {
  await writer.write(`
  // ${item.description}
  enum class ${name}: ${item.type[0]} {
${Object.entries(item.values)
  .map(([key, value]) => `    ${key} = ${value},`)
  .join('\n')}
  };`);
}

async function emitStringConstants(
  writer: Bun.FileSink,
  name: string,
  item: SharedConstants<string>,
): Promise<void> {
  writer.write(`
  // ${item.description}
  enum class ${name} {
${Object.keys(item.values)
  .map((key) => `    ${key},`)
  .join('\n')}
  };
`);

  writer.write(`
  constexpr std::string_view get_string(${name} _value) {
    switch (_value) {
${Object.entries(item.values)
  .map(([key, val]) => `      case ${name}::${key}: return "${val}";`)
  .join('\n')}
    }
    return "<unknown>";
  }
`);
  writer.write(`
  constexpr std::optional<${name}> string_to_${name}(const std::string_view &str) {
${Object.entries(item.values)
  .map(([key, val]) => `    if (str == "${val}") return ${name}::${key};`)
  .join('\n')}
    return std::nullopt;
  }
`);
}

async function emitCode(
  fileName: string,
  items: [string, SharedConstants<unknown>][],
): Promise<void> {
  const file = Bun.file(fileName);
  if (await file.exists()) {
    await file.delete();
  }
  const writer = file.writer();
  await emitPreamble(writer);
  for (const [name, item] of items) {
    // Emit the C++ code for each SharedConstants item, either numeric or string type
    switch (item.type[0]) {
      case 'char':
      case 'unsigned char':
      case 'short':
      case 'unsigned short':
      case 'int':
      case 'unsigned int':
      case 'long':
      case 'unsigned long':
      case 'long long':
      case 'unsigned long long': {
        await emitNumericConstants(
          writer,
          name,
          item as SharedConstants<number>,
        );
        break;
      }
      case 'string': {
        await emitStringConstants(
          writer,
          name,
          item as SharedConstants<string>,
        );
        break;
      }
      default: {
        console.warn(`Unknown type for SharedConstants: ${item.type}`);
      }
    }
  }
  await emitPostamble(writer);
  await writer.end();
}

async function main(fileName?: string): Promise<void> {
  // A script to generate C++ code from the SharedConstants.ts file
  if (!fileName) {
    console.error('Usage: scripts/gencpp.ts <output_file>');
    process.exit(1);
  }

  const items: [string, SharedConstants<unknown>][] = [];
  for (const sc of Object.keys(AllConstants)) {
    if (!hasFieldType(AllConstants, sc, chkSharedConstants)) {
      console.warn(`Skipping ${sc} as it does not match SharedConstants type.`);
      continue;
    }
    items.push([sc, AllConstants[sc]]);
  }

  await emitCode(fileName, items);
}

console.log(`Generating C++ code to ${Bun.argv[2]}...`);
main(Bun.argv[2]).catch((err) => {
  console.error('Error generating C++ code:', err);
  process.exit(1);
});
