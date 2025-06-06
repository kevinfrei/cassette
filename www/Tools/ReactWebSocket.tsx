import { ChangeEventHandler, ReactElement, useEffect, useState } from 'react';

export function RealTimeUpdates(): ReactElement {
  const [messages, setMessages] = useState<string[]>([]);
  const [ws, setWs] = useState<WebSocket | null>(null);
  const [message, setMessage] = useState('');
  const [clientId, setClientId] = useState(-1);

  useEffect(() => {
    const websocket = new WebSocket('ws://localhost:55555/ws');

    websocket.onopen = () => {
      console.log('WebSocket is connected');
      // Generate a unique client ID
      const id = Math.floor(Math.random() * 1000);
      setClientId(id);
    };

    websocket.onmessage = (evt) => {
      const message = evt.data;
      setMessages((prevMessages) => [...prevMessages, message]);
    };

    websocket.onclose = () => {
      console.log('WebSocket is closed');
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
