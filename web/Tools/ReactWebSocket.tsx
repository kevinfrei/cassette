import { MakeLog } from '@freik/logger';
import { hasField, hasFieldOf, isNumber } from '@freik/typechk';
import { ReactElement, useEffect } from 'react';
import { MyWindow } from '../Types';
import { WireUpIpc } from './Ipc';

declare const window: MyWindow;

const { con, log, err } = MakeLog('Tools:RealTimeUpdates');

export function WebSocketRegistrar(): ReactElement {
  useEffect(() => {
    // Let's make sure the port is specified properly
    if (!hasFieldOf(window, 'wsport', isNumber)) {
      err("Please make sure you're including the WebSocket server script.");
      return () => {};
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
    return () => {
      websocket.close();
    };
  }, []);

  /*
  const handleInputChange: ChangeEventHandler<HTMLInputElement> = (event) => {
    setMessage(event.target.value);
  };
  */

  return <></>;
  /*(
    <div>
      <h2>Real-time Comms w/ WebSockets and React Hooks</h2>
      <h3> Client ID {clientId}</h3>
      {messages.map((message, index) => (
        <p key={index}>{message}</p>
      ))}
      <input type="text" value={message} onChange={handleInputChange} />
      <button onClick={sendMessage}>Send Message</button>
    </div>
  );
  */
}
