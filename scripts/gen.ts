import { hasFieldType } from '@freik/typechk';

import { TypesToGenerate } from '../www/Shared/Enums';
import { ADTs } from '../www/Shared/IDL';
import { CppEmitter } from './emitters/cpp';
import { EmitItem, Emitter } from './emitters/interface';

function forElement(emit: Emitter, adt: ADTs): EmitItem<any> {
  // Returns the type emitter for the given ADT
  switch (adt.t) {
    case 'O':
      return emit.types.objType;
    case 'A':
      return emit.types.arrType;
    case 'S':
      return emit.types.setType;
    case 'M':
      return emit.types.mapType;
    case 'T':
      return emit.types.tupType;
    case '#':
      return emit.types.enumType;
    case '%':
      return emit.types.numEnumType;
    case '$':
      return emit.types.strEnumType;
    default:
      throw new Error(`Unknown ADT type: ${adt.t}`);
  }
}

async function emitCode(
  fileName: string,
  items: Record<string, ADTs>,
): Promise<void> {
  const file = Bun.file(fileName);
  if (await file.exists()) {
    await file.delete();
  }
  const writer = file.writer();
  const emitter = CppEmitter;
  await emitter.header(writer);
  for (const [name, item] of Object.entries(items)) {
    // Emit the C++ code for each SharedConstants item, either numeric or string type
    const itemEmitter = forElement(emitter, item);
    await itemEmitter(writer, name, item);
  }
  await emitter.footer(writer);
  await writer.end();
}

async function main(fileName?: string): Promise<void> {
  // A script to generate C++ code from the SharedConstants.ts file
  if (!fileName) {
    console.error('Usage: scripts/gen.ts <output_file>');
    process.exit(1);
  }

  await emitCode(fileName, TypesToGenerate);
}

console.log(`Generating C++ code to ${Bun.argv[2]}...`);
main(Bun.argv[2]).catch((err) => {
  console.error('Error generating C++ code:', err);
  process.exit(1);
});
