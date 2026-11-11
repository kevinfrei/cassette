import { PrimitiveAtom, WritableAtom } from 'jotai';
import { atomFamily } from 'jotai-family';
import { RESET } from 'jotai/utils';

export type SetStateActionWithReset<T> =
  T | typeof RESET | ((prev: T) => T | typeof RESET);

export type SetStateAction<T> = T | ((prev: T) => T);

export type WritableAtomType<T> = WritableAtom<
  T | Promise<T>,
  [SetStateActionWithReset<T | Promise<T>>],
  Promise<void>
>;

export type WriteOnlyAtomType<T> = WritableAtom<
  T | Promise<T>,
  [SetStateAction<T | Promise<T>>],
  Promise<void>
>;

export type SetAtomFamily<T> = [
  WritableAtomType<Set<T>> | PrimitiveAtom<Set<T>>,
  ReturnType<typeof atomFamily<T, WritableAtom<boolean, [boolean], void>>>,
];

export type Unsubscribe = () => void;
type Subscribe<Value> = (
  key: string,
  callback: (value: Value) => void,
  initialValue: Value,
) => Unsubscribe | undefined;
export interface AsyncStorage<Value> {
  getItem: (key: string, initialValue: Value) => PromiseLike<Value>;
  setItem: (key: string, newValue: Value) => PromiseLike<void>;
  removeItem: (key: string) => PromiseLike<void>;
  subscribe?: Subscribe<Value>;
}
export interface SyncStorage<Value> {
  getItem: (key: string, initialValue: Value) => Value;
  setItem: (key: string, newValue: Value) => void;
  removeItem: (key: string) => void;
  subscribe?: Subscribe<Value>;
}
export interface AsyncCallback<Value> {
  getItem: (key: string) => PromiseLike<Value>;
  subscribe?: (key: string, callback: (value: Value) => void) => Unsubscribe;
}

export interface SyncCallback<Value> {
  getItem: (key: string) => Value;
  subscribe?: (key: string, callback: (value: Value) => void) => Unsubscribe;
}
