import { $, BuildOutput } from 'bun';

import { BuildType, GetBuildType } from './buildtype';

export async function WebBuild(
  buildType: BuildType,
): Promise<[BuildOutput, $.ShellOutput]> {
  const isDebug = buildType === 'Debug';
  try {
    const rmdir = await $`rm -r build/${buildType}/web`;
    console.log(rmdir.exitCode, rmdir.text());
  } catch {}
  try {
    const mkdir = await $`mkdir -p build/${buildType}/web/img`;
    console.log(mkdir.exitCode, mkdir.text());
  } catch {}
  const bunBuild = await Bun.build({
    entrypoints: ['web/index.html'],
    outdir: `build/${buildType}/web`,
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
  const copy = $`cp web/img/* build/${buildType}/web/img`;
  const res = await copy;
  console.log(res.exitCode, res.text());
  // Copy all the web/img files to build/${buildType}/web/img:
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
