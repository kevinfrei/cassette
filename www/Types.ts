import { IpcId } from './Shared/CommonTypes';

export type ListenKey = { ipcId: IpcId; id: string };

export type Ipc = {
  post: (id: IpcId, ...args: unknown[]) => void;
};

export interface MyWindow extends Window {
  ws?: WebSocket;
  // clientId?: number;
  ipc?: Ipc;
  wsport?: number;
}
