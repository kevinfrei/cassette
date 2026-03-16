import { $, BuildOutput } from 'bun';

import { BuildType, GetBuildType } from './buildtype';

export async function WebBuild(
  buildType: BuildType,
): Promise<[BuildOutput, $.ShellOutput]> {
  const isDebug = buildType === 'Debug';
  try {
    await $`mkdir -p build/${buildType}/web/img`;
  } catch {}
  const bunBuild = await Bun.build({
    entrypoints: ['web/index.html'],
    outdir: `build/${buildType}/web`,
    minify: !isDebug,
    sourcemap: isDebug ? 'inline' : 'none',
    target: 'browser',
    define: {
      // There's a bug when you set this to production...
      'process.env.NODE_ENV': '"debug"', //isDebug ? '"debug"' : '"production"',
    },
    env: 'BUN_PUBLIC_*',
    splitting: isDebug,
    // packages: isDebug ? 'external' : 'bundle',
    // external: isDebug ? ['*']: [],
  });
  const copy = $`cp web/img/* build/${buildType}/web/img`;
  const res = await copy;
  // Copy all the www/img files to build/${buildType}/www/img:
  return [bunBuild, res];
}

if (import.meta.main) {
  WebBuild(GetBuildType())
    .then(() => {})
    .catch((err) => {
      console.error(err);
      process.exit(1);
    });
}
