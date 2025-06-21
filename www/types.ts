export type ListenKey = { key: string; id: string };
export type MessageHandler = (val: unknown) => void;
/*hostOs: ((): 'mac' | 'win' | 'lin' | 'unk' => {
  const plat = process.platform;
  switch (plat) {
    case 'darwin':
      return 'mac';
    case 'win32':
      return 'win';
    case 'linux':
      return 'lin';
    default:
      return 'unk';
  }
})(),
*/
export type Ipc = {
  invoke: <T>(channel: string, key?: T) => Promise<unknown>;
  on: (key: string, listen: Function) => void;
  removeEventListener: (key: string, listen: Function) => void;
};
export interface MyWindow extends Window {
  ipc?: Ipc;
}
