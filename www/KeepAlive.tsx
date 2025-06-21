import { isString } from '@freik/typechk';
import { ReactElement } from 'react';

import { useInterval } from './hooks';
import { RawGet } from './Tools/ipc';

// This is a 'two-way' keep-alive mechanism.
// It sends a request to the server every 5 seconds to keep the connection alive.
// If the server does not respond with 'OK', it will close the window after a short delay.
// This is useful for preventing the server from shutting down due to inactivity
// and for ensuring that the client is still connected to the server, and will close the window if the server is not responding.

function closeWindow() {
  // TODO: Turn this off for Debug builds...
  window.close();
}
export function KeepAlive(): ReactElement {
  useInterval(() => {
    RawGet('/keepalive')
      .then((res) => {
        if (!isString(res) || res !== 'OK') {
          closeWindow();
        }
      })
      .catch((err) => {
        closeWindow();
      });
  }, 5 * 1000);
  return <></>;
}
