import { useEffect, useRef } from 'react';

export function useInterval(callback: () => void, delayInMs: number) {
  const savedCallback = useRef<() => void>();

  // Remember the latest callback.
  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  // Set up the interval.
  useEffect(() => {
    function tick() {
      savedCallback.current!();
    }
    let id = setInterval(tick, delayInMs);
    return () => clearInterval(id);
  }, [delayInMs]);
}
