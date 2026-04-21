import { SocketMsg } from './Shared/CommonTypes';

export type ListenKey = { ipcId: SocketMsg; id: string };

export type Ipc = {
  post: (id: SocketMsg, ...args: unknown[]) => void;
};

export interface MyWindow extends Window {
  ws?: WebSocket;
  // clientId?: number;
  ipc?: Ipc;
  wsport?: number;
}

export type Setter<T> = (arg: T) => void;
