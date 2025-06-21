import { typecheck } from '@freik/typechk';

export type ListenKey = { key: string; id: string };

export type Ipc = {
  invoke: <T>(
    channel: string,
    retval: typecheck<T>,
    ...args: unknown[]
  ) => Promise<T>;
  listen: <T>(
    key: string,
    typechk: typecheck<T>,
    listener: (arg: T) => void,
  ) => ListenKey;
  removeEventListener: (listenKey: ListenKey) => void;
};

export interface MyWindow extends Window {
  ws?: WebSocket;
  clientId?: number;
  ipc?: Ipc;
  wsport?: number;
}
