import { IpcCall, SocketMsg } from './Shared/CommonTypes';

export type ListenKey = { ipcId: SocketMsg; id: string };

export type Ipc = {
  post: (id: IpcCall, ...args: unknown[]) => void;
};

export interface MyWindow extends Window {
  ws?: WebSocket;
  // clientId?: number;
  ipc?: Ipc;
  wsport?: number;
}
