import { CppBuild } from './cppbuild';
import { WebBuild } from './webbuild';

import { BuildType, GetBuildType } from './buildtype';

// Logically:
// bun wdbld && bun dbuild

async function parallelBuild(buildType: BuildType): Promise<void> {
  const res = await Promise.all([WebBuild(buildType), CppBuild(buildType)]);
  let success = true;
  if (!res[0][0].success || res[0][1].exitCode) {
    console.error('WWW Build failed');
    console.error(res[0][0].logs);
    console.error(res[0][1].text());
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

parallelBuild(GetBuildType())
  .then(() => {})
  .catch((err) => {
    console.error(err);
    process.exit(1);
  });
