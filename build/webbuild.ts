import { BuildOutput } from 'bun';

import { BuildType, GetBuildType } from './buildtype';

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

if (import.meta.main) {
  WebBuild(GetBuildType())
    .then(() => {})
    .catch((err) => {
      console.error(err);
      process.exit(1);
    });
}
