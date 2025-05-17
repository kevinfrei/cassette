import React from 'react';
import { useInterval } from './hooks';
import { Post } from './io';

export function KeepAlive(): React.JSX.Element {
  useInterval(() => Post('keepalive'), 5 * 1000);
  return <></>;
}
