import { $ } from 'bun';

import { GetBuildType, BuildType } from './buildtype';

export function CppBuild(buildType: BuildType): $.ShellPromise {
  return $`cmake --build build --config ${buildType}`;
}

if (import.meta.main) {
  CppBuild(GetBuildType())
    .then(() => {})
    .catch((err) => {
      console.error(err);
      process.exit(1);
    });
}
