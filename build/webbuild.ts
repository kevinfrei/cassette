import { BuildOutput } from 'bun';

import { BuildType, GetBuildType } from './buildtype';

export function WebBuild(buildType: BuildType): Promise<BuildOutput> {
  const isDebug = buildType === 'Debug';
  return Bun.build({
    entrypoints: ['www/index.html'],
    outdir: `build/${buildType}/www`,
    minify: !isDebug,
    sourcemap: isDebug ? 'inline' : 'none',
    target: 'browser',
    define: {
      'process.env.NODE_ENV': isDebug ? '"debug"' : '"production"',
    },
    env: 'BUN_PUBLIC_*',
    splitting: isDebug,
    // packages: isDebug ? 'external' : 'bundle',
    //external: isDebug ? ['*']: [],
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
