import { WebBuild } from './webbuild';
import { CppBuild } from './cppbuild';

import { GetBuildType, BuildType } from './buildtype';

// Logically:
// bun wdbld && bun dbuild && ( cd build/Debug && ./cassette )

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

parallelBuild(GetBuildType())
  .then(() => {})
  .catch((err) => {
    console.error(err);
    process.exit(1);
  });
