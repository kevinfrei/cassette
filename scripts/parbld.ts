import { WebBuild } from './webbuild';
import { CppBuild } from './cppbuild';

import type { BuildType } from './types';

// Logically:
// bun wdbld && bun dbuild && ( cd build/Debug && ./cassette )
const args = process.argv.slice(2);
let arg = args[0] || 'Debug';
let buildType: BuildType;
switch (arg.toLowerCase()) {
  case 'debug':
    buildType = 'Debug';
    break;
  case 'release':
    buildType = 'Release';
    break;
  case 'relwithdebinfo':
    buildType = 'RelWithDebInfo';
    break;
  default:
    console.error(`Unknown build type: ${arg}`);
    process.exit(1);
}

async function parallelBuild(buildType: BuildType): Promise<void> {
  const res = await Promise.all([WebBuild(buildType), CppBuild(buildType)]);
  let success = true;
  if (!res[0].success) {
    console.error('WWW Build failed');
    console.error(res[0].logs);
    success = false;
  }
  if (res[1].exitCode !== 0) {
    console.error('CMake Build failed');
    console.error(res[1].stdout);
    console.error(res[1].stderr);
    success = false;
  }
  if (!success) {
    return;
  }
}

console.log(process.argv);

parallelBuild(buildType)
  .then(() => {})
  .catch((err) => {
    console.error(err);
    process.exit(1);
  });
