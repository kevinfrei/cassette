import {
  Pickle,
  SafelyUnpickle,
  isDefined,
  isNumber,
  isString,
  isUndefined,
  typecheck,
} from '@freik/typechk';
import { createStore } from 'jotai';
import { atomWithStorage } from 'jotai/utils';
import { AsyncStorage } from 'jotai/vanilla/utils/atomWithStorage';
import { IpcCall, SocketMsg, StorageId } from 'www/Shared/CommonTypes';
import {
  DeleteFromStorage,
  ReadFromStorage,
  Subscribe,
  SubscribeUnsafe,
  SubscribeWithDefault,
  Unsubscribe,
  WriteToStorage,
} from '../Tools/Ipc';
import { WritableAtomType } from './Hooks';

const theStore = createStore();

export type MyStore = typeof theStore;
export type MaybeStore = MyStore | undefined;

export function getStore(curStore?: MaybeStore): MyStore {
  return curStore || theStore;
}

// This is the simple "sync with main" storage provider for Jotai

function makeGetItem<T>(
  chk: typecheck<T>,
): (key: string, initialValue: T) => PromiseLike<T> {
  return async (key: string, initialValue: T): Promise<T> => {
    const maybeValue = await ReadFromStorage(key, chk);
    return isDefined(maybeValue) ? maybeValue : initialValue;
  };
}

function makeGetTranslatedItem<T, U>(
  chk: typecheck<U>,
  xlate: (val: U) => T,
): (key: string, initialValue: T) => PromiseLike<T> {
  return async (key: string, initialValue: T): Promise<T> => {
    const value = await ReadFromStorage(key, chk);
    return isDefined(value) ? xlate(value) : initialValue;
  };
}

async function setItem<T>(key: string, data: T): Promise<void> {
  return WriteToStorage(key, data);
}

async function setTranslatedItem<T, U>(
  key: string,
  newValue: T,
  xlate: (val: T) => U,
): Promise<void> {
  return WriteToStorage(key, xlate(newValue));
}

// eslint-disable-next-line @typescript-eslint/no-unused-vars
async function noSetItem<T>(_key: string, _newValue: T): Promise<void> {
  return Promise.resolve();
}

async function removeItem(key: string): Promise<void> {
  await DeleteFromStorage(key);
}

// eslint-disable-next-line @typescript-eslint/no-unused-vars
async function noRemoveItem(_key: string): Promise<void> {
  return Promise.resolve();
}

type Unsub = () => void;
type Subscriber<T> = (
  key: string, // Jotai key, which is the same as the IpcMsg key
  callback: (value: T) => void,
  initVal: T,
) => Unsub;

// TODO: This is really stupid and slow:
function getIpcCall(key: number): IpcCall {
  for (let [, value] of Object.entries(IpcCall)) {
    if (value === key) return value;
  }
  return IpcCall.Unknown;
}

function makeSubscribe<T>(chk: typecheck<T>): Subscriber<T> {
  return (key: IpcCall, callback: (value: T) => void, initialValue: T) => {
    const lk = SubscribeWithDefault(key, chk, callback, initialValue);
    return () => Unsubscribe(lk);
  };
}

function makeTranslatedSubscribe<T, U>(
  chk: typecheck<T>,
  xlate: (val: T) => U | undefined,
  def: U,
): Subscriber<U> {
  return (key: IpcCall, callback: (value: T) => void, initialValue: T) => {
    const lk = SubscribeUnsafe(key.toString(), (val: unknown) => {
      if (chk(val)) {
        callback(xlate(val));
      } else {
        callback(initialValue);
      }
    });
    return () => Unsubscribe(lk);
  };
}

export function getMainStorage<T>(chk: typecheck<T>): AsyncStorage<T> {
  return {
    getItem: makeGetItem(chk),
    setItem,
    removeItem,
    subscribe: makeSubscribe(chk),
  };
}

export function getMainReadOnlyStorage<T>(chk: typecheck<T>): AsyncStorage<T> {
  return {
    getItem: makeGetItem(chk),
    setItem: noSetItem,
    removeItem: noRemoveItem,
    subscribe: makeSubscribe(chk),
  };
}

export function atomWithMainStorage<T>(
  key: StorageId,
  init: T,
  chk: typecheck<T>,
): WritableAtomType<T> {
  return atomWithStorage(key, init, getMainStorage(chk), { getOnInit: true });
}

export function atomFromMain<T>(
  key: string,
  init: T,
  chk: typecheck<T>,
): WritableAtomType<T> {
  return atomWithStorage(key, init, getMainReadOnlyStorage(chk));
}

function getTranslatedMainStorage<T, U>(
  chk: typecheck<U>,
  fromMain: (val: U) => T,
  toMain: (val: T) => U,
): AsyncStorage<T> {
  return {
    getItem: makeGetTranslatedItem(chk, fromMain),
    setItem: async (k, v) => setTranslatedItem(k, toMain(v), fromMain),
    removeItem,
    subscribe: makeTranslatedSubscribe(chk, fromMain),
  };
}

function getTranslatedMainReadOnlyStorage<T, U>(
  chk: typecheck<U>,
  fromMain: (val: U) => T,
): AsyncStorage<T> {
  return {
    getItem: makeGetTranslatedItem(chk, fromMain),
    setItem: noSetItem,
    removeItem: noRemoveItem,
    subscribe: makeTranslatedSubscribe(chk, fromMain),
  };
}

export function atomWithTranslatedStorageInMain<T, U>(
  key: string,
  init: T,
  chk: typecheck<U>,
  toMain: (val: T) => U,
  fromMain: (val: U) => T,
): WritableAtomType<T> {
  return atomWithStorage(
    key,
    init,
    getTranslatedMainStorage<T, U>(chk, fromMain, toMain),
  );
}

export function atomFromTranslatedStorageFromMain<T, U>(
  key: string,
  init: T,
  chk: typecheck<U>,
  fromMain: (val: U) => T,
): WritableAtomType<T> {
  return atomWithStorage(
    key,
    init,
    getTranslatedMainReadOnlyStorage<T, U>(chk, fromMain),
  );
}

export function getTranslatedSubscribe<T, U>(
  key: IpcMsg,
  typechk: typecheck<T>,
  translate: (value: T) => U | undefined,
  callback: (value: U) => void,
  initialValue: U,
): () => void {
  const lk = Subscribe(key, typechk, (val: T) => {
    const xlate = translate(val);
    if (isDefined(xlate)) {
      callback(xlate);
    } else {
      callback(initialValue);
    }
  });
  return () => Unsubscribe(lk);
}
