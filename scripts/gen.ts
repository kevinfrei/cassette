import { TypesToGenerate } from '../www/Shared/Enums';
import { GetCppGenerator } from './emitters/cpp';

async function main(fileName?: string): Promise<void> {
  // A script to generate C++ code from the SharedConstants.ts file
  if (!fileName) {
    console.error('Usage: scripts/gen.ts <output_file>');
    process.exit(1);
  }
  const CppGen = GetCppGenerator();
  await CppGen(fileName, TypesToGenerate);
}

console.log(`Generating C++ code to ${Bun.argv[2]}...`);
main(Bun.argv[2]).catch((err) => {
  console.error('Error generating C++ code:', err);
  process.exit(1);
});
