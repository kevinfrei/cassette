import { DependencyList, useEffect } from 'react';

/**
 * Add a listener for a media query, and invoke it once, which
 * is necessary to get it to start paying attention, apparently?
 *
 * Use it like this:
 * ```typescript
 * const handleWidthChange = useMyTransaction(
 *   ({ set }) =>
 *     ev: MediaQueryList | MediaQueryListEvent) => {
 *       set(isMiniplayerState, ev.matches);
 *     },
 * );
 * useMediaEffect('(max-width: 499px)', handleWidthChange);
 * ```
 *
 * @param mq The media query to listen for changes in
 * @param handler The function to invoke when the media query changes
 */
export function useMediaEffect(
  mq: string,
  handler: (ev: MediaQueryList | MediaQueryListEvent) => void,
  deps?: DependencyList,
): void {
  let mediaQuery: MediaQueryList | null = null;

  const subscribeMediaMatcher = () => {
    mediaQuery = window.matchMedia(mq);
    mediaQuery.addEventListener('change', handler);
    handler(mediaQuery);
  };

  /**
   * Remove the mediaquery listener. See {@link SubscribeMediaMatcher} for
   * an example
   *
   * @param handler the handler that had been previously subscribed
   */
  const unsubscribeMediaMatcher = () =>
    mediaQuery?.removeEventListener('change', handler);

  return useEffect(() => {
    subscribeMediaMatcher();
    return () => unsubscribeMediaMatcher();
    // eslint-disable-next-line react-hooks/exhaustive-deps, @typescript-eslint/no-unsafe-assignment
  }, [handler, mq, ...(deps || [])]);
}
