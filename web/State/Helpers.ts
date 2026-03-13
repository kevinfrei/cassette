import { Atom, Getter, Setter, useSetAtom } from 'jotai';
import { useAtomCallback } from 'jotai/utils';
import { useCallback } from 'react';

export function AsyncHandler<Args extends unknown[], Res>(
  fn: (...args: Args) => Promise<Res>,
): (...args: Args) => void {
  return (...args) => {
    fn.apply(args).catch(console.error);
  };
}

type Options = Parameters<typeof useSetAtom>[1];

export function useJotaiCallback<Result, Args extends unknown[]>(
  fn: (get: Getter, set: Setter, ...args: Args) => Result,
  deps: any[],
  options?: Options,
): (...args: Args) => Result {
  return useAtomCallback(useCallback(fn, deps), options);
}

export function useJotaiAsyncCallback<Result, Args extends unknown[]>(
  fn: (get: Getter, set: Setter, ...args: Args) => Promise<Result>,
  deps: any[],
  options?: Options,
): (...args: Args) => Promise<Result> {
  return useAtomCallback(useCallback(fn, deps), options);
}

// Typescript magic is...not particularly obvious, here. We're passing the return
// types of each get up to the resulting tuple, whether it's a promise or not.
export function getAll<Atoms extends readonly Atom<any>[]>(
  get: Getter,
  ...atoms: Atoms
): Promise<{
  [K in keyof Atoms]: Atoms[K] extends Atom<infer T> ? Awaited<T> : never;
}> {
  return Promise.all(atoms.map(get)) as Promise<{
    [K in keyof Atoms]: Atoms[K] extends Atom<infer T> ? Awaited<T> : never;
  }>;
}
