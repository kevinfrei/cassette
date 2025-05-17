import React from 'react';
import { useInterval } from './hooks';

export function KeepAlive(): React.JSX.Element {
  useInterval(() => fetch('/keepalive', { method: 'GET' }), 5 * 1000);
  return <></>;
}
