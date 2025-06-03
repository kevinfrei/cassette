import { TypesToGenerate } from '../www/Shared/Definitions';
import { GetCppGenerator } from './emitters/cpp';
import { GetTypescriptGenerator } from './emitters/typescript';

async function main(header?: string, typescr?: string): Promise<void> {
  // A script to generate C++ code from the SharedConstants.ts file
  if (!header || !typescr) {
    console.error('Usage: scripts/gen.ts <cppheader.h> <tsoutput.ts>');
    process.exit(1);
  }
  const CppGen = GetCppGenerator();
  await CppGen(header, TypesToGenerate);
  const TypescriptGen = GetTypescriptGenerator();
  await TypescriptGen(typescr, TypesToGenerate);
}

console.log(`Generating C++ code to ${Bun.argv[2]}, ${Bun.argv[3]}...`);
main(Bun.argv[2], Bun.argv[3]).catch((err) => {
  console.error('Error generating C++ code:', err);
  process.exit(1);
});
