import { MakeLog } from '@freik/logger';
import { hasFieldOf, isNumber } from '@freik/typechk';
import { ChangeEventHandler, ReactElement, useEffect, useState } from 'react';

const { log, err } = MakeLog('Tools:RealTimeUpdates');

export function RealTimeUpdates(): ReactElement {
  const [messages, setMessages] = useState<string[]>([]);
  const [ws, setWs] = useState<WebSocket | null>(null);
  const [message, setMessage] = useState('');
  const [clientId, setClientId] = useState(-1);

  useEffect(() => {
    // Let's make sure the port is specified properly
    if (!hasFieldOf(window, 'wsport', isNumber)) {
      err("Please make sure you're including the WebSocket server script.");
      return () => {};
    }
    // Create a new WebSocket connection
    const websocket = new WebSocket(`ws://localhost:${window.wsport}/ws`);

    websocket.onopen = () => {
      log('WebSocket is connected');
      // Generate a unique client ID
      const id = Math.floor(Math.random() * 2147483647);
      setClientId(id);
    };

    websocket.onmessage = (evt: MessageEvent) => {
      const message = evt.data;
      // TODO: Route the message to an appropriate handler
      // I need to register handlers for different message types

      // Limit the number of messages to the last 10
      setMessages((prevMessages) => [...prevMessages.slice(-9), message]);
    };

    websocket.onclose = () => {
      log('WebSocket is closed');
    };

    setWs(websocket);

    return () => {
      websocket.close();
    };
  }, []);

  const sendMessage = () => {
    if (ws) {
      ws.send(
        JSON.stringify({
          type: 'message',
          payload: message,
          clientId: clientId,
        }),
      );
      setMessage('');
    }
  };

  const handleInputChange: ChangeEventHandler<HTMLInputElement> = (event) => {
    setMessage(event.target.value);
  };

  return (
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
}
