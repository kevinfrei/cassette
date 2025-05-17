import { $ } from 'bun';

import type { BuildType } from './types';

export function CppBuild(buildType: BuildType): $.ShellPromise {
  return $`cmake --build build --config ${buildType}`;
}
