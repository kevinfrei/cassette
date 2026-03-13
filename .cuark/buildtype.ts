export type BuildType = 'Debug' | 'Release' | 'RelWithDebInfo';

export function GetBuildType(): BuildType {
  const args = process.argv.slice(2);
  let arg = args[0] || 'Debug';
  switch (arg.toLowerCase()) {
    case 'debug':
    case 'dbg':
    case 'd':
      return 'Debug';
    case 'release':
    case 'rel':
    case 'r':
      return 'Release';
    case 'relwithdebinfo':
    case 'chk':
    case 'c':
      return 'RelWithDebInfo';
    default:
      console.error(`Unknown build type: ${arg}`);
      process.exit(1);
  }
}
