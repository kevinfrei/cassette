import { BuildOutput } from 'bun';

import type { BuildType } from './types';

export function WebBuild(buildType: BuildType): Promise<BuildOutput> {
  return Bun.build({
    entrypoints: ['www/index.html'],
    outdir: `build/${buildType}/www`,
    minify: buildType !== 'Debug',
    sourcemap: buildType === 'Debug' ? 'linked' : 'none',
    target: 'browser',
    define: {
      'process.env.NODE_ENV':
        buildType === 'Debug' ? '"debug"' : '"production"',
    },
    env: 'BUN_PUBLIC_*',
  });
}
