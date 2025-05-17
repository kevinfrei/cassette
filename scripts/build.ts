import { $, build } from "bun";

// Logically:
// bun wdbld && bun dbuild && ( cd build/Debug && ./cassette )
const args = process.argv.slice(2);
let buildType = args[0] || "Debug";
switch (buildType.toLowerCase()) {
  case "debug":
    buildType = "Debug";
    break;
  case "release":
    buildType = "Release";
    break;
  case "relwithdebinfo":
    buildType = "RelWithDebInfo";
    break;
  default:
    console.error(`Unknown build type: ${buildType}`);
    process.exit(1);
}

async function parallelBuild(buildType: string) {
  const res = await Promise.all([
    build({
      entrypoints: ["www/index.html"],
      outdir: `build/${buildType}/www`,
      minify: buildType !== "Debug",
      sourcemap: buildType === "Debug" ? "linked" : "none",
      target: "browser",
      define: {
        "process.env.NODE_ENV":
          buildType === "Debug" ? '"debug"' : '"production"',
      },
      env: "BUN_PUBLIC_*",
    }),
    $`cmake --build build --config ${buildType}`,
  ]);
  let success = true;
  if (!res[0].success) {
    console.error("WWW Build failed");
    console.error(res[0].logs);
    success = false;
  }
  if (res[1].exitCode !== 0) {
    console.error("CMake Build failed");
    console.error(res[1].stdout);
    console.error(res[1].stderr);
    success = false;
  }
  if (!success) {
    return;
  }
}

parallelBuild(buildType)
  .then(() => {})
  .catch((err) => {
    console.error(err);
    process.exit(1);
  });
