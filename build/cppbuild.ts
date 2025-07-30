import { $ } from 'bun';

import { BuildType, GetBuildType } from './buildtype';

export function CppBuild(buildType: BuildType): $.ShellPromise {
  return $`cmake --build build/${buildType}`;
}

if (import.meta.main) {
  CppBuild(GetBuildType())
    .then(() => {})
    .catch((err) => {
      console.error(err);
      process.exit(1);
    });
}
