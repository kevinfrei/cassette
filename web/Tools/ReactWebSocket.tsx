import { hasField, hasFieldOf, isNumber } from '@freik/typechk';
import { SocketMsg } from '../Shared/CommonTypes';
import { MyWindow } from '../Types';
import { SendMessage, WireUpIpc } from './Ipc';

declare const window: MyWindow;

// const { con, err } = MakeLog('Tools:RealTimeUpdates');

const con = console.error;
const err = console.error;

export function WebSocketRegistrar(): void {
  // Let's make sure the port is specified properly
  if (!hasFieldOf(window, 'wsport', isNumber)) {
    err("Please make sure you're including the WebSocket server script.");
    return;
  }
  // Create a new WebSocket connection
  const websocket = new WebSocket(`ws://localhost:${window.wsport}/ws`);

  websocket.onopen = () => {
    con('WebSocket is connected');
    // Generate a unique client ID
    // const id = Math.floor(Math.random() * 16777216);
    window.ws = websocket; // Store the WebSocket in the global window object
    // window.clientId = id; // Store the client ID in the global window object
    WireUpIpc(); // Trigger the IPC setup code
    Ready('ipc');
  };

  websocket.onclose = () => {
    con('WebSocket is closed');
    if (hasField(window, 'ws')) {
      delete window.ws; // Clear the WebSocket reference
    }
    // if (hasField(window, 'clientId')) {
    //   delete window.clientId; // Clear the client ID reference
    // }
  };

  window.ws = websocket; // Store the WebSocket in the global window object
}

let loaded = false;
let wired = false;
export function Ready(from: 'content' | 'ipc') {
  if (from === 'content') {
    err('Content loaded.');
    loaded = true;
  } else if (from === 'ipc') {
    err('IPC wired up.');
    wired = true;
  } else {
    err('Unknown source for Ready: ' + from);
    return;
  }
  if (!loaded || !wired) {
    err('Waiting for the other one...');
    return;
  }
  err('Content loaded, sending message to server.');
  // This is the thing to tell the server everything's loaded
  SendMessage(SocketMsg.ContentLoaded);
}
