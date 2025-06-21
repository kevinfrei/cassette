import { MakeLog } from '@freik/logger';
import { isString } from '@freik/typechk';
import { MyWindow } from './Types';

declare const window: MyWindow;

const { log, err } = MakeLog('ipc');

let ipcAttempts = 0;

export function WireUpIpc(): void {
  const ws = window.ws;
  if (!ws) {
    ipcAttempts++;
    setTimeout(WireUpIpc, 100);
    if (ipcAttempts % 10 === 0) {
      console.error(
        `Failed to wire up IPC after ${ipcAttempts} attempts. Please ensure the ReactWebScoket module is loaded.`,
      );
    }
    return;
  }
  ws.onmessage = (evt: MessageEvent) => {
    const message = evt.data;
    if (!isString(message)) {
      err(`Received non-string message from websocket:`, message);
      return;
    }
    log(`Received message: ${message}`);
    // TODO: Route the message to an appropriate handler
  };
}

/*
    const sendMessage = (msg: unknown) => {
      if (ws) {
        ws.send(
          Pickle({
            type: 'message',
            payload: msg,
            clientId: clientId,
          }),
        );
      }
    };
*/
