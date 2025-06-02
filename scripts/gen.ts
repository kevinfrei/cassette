import { hasFieldType } from '@freik/typechk';

import { TypesToGenerate } from '../www/Shared/Enums';
import { Aggregate } from '../www/Shared/IDL';
import { CppEmitter } from './emitters/cpp';
import { EmitItem, Emitter } from './emitters/interface';

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
