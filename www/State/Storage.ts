import {
  isDefined,
  isString,
  SafelyUnpickle,
  typecheck,
  Unpickle,
} from '@freik/typechk';
import { createStore } from 'jotai';
import { atomWithStorage } from 'jotai/utils';
import { AsyncStorage } from 'jotai/vanilla/utils/atomWithStorage';
import {
  chkSocketMsg,
  IpcCall,
  SocketMsg,
  StorageId,
} from 'www/Shared/CommonTypes';
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
    const maybeValue = await ReadFromStorage(key, isString);
    if (!isDefined(maybeValue)) {
      return initialValue;
    }
    return SafelyUnpickle(maybeValue, chk) || initialValue;
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
  key: string, // Jotai key, which I use as the stringified SocketMsg key
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

function SocketMsgFromString(key: string): SocketMsg {
  const msg = Number.parseInt(key);
  if (chkSocketMsg(msg)) {
    return msg;
  } else {
    throw new Error(`Invalid SocketMsg key: ${key}`);
  }
}

export function makeSubscribe<T>(chk: typecheck<T>): Subscriber<T> {
  return (key: string, callback: (value: T) => void, initialValue: T) => {
    const lk = SubscribeWithDefault(
      SocketMsgFromString(key),
      chk,
      callback,
      initialValue,
    );
    return () => Unsubscribe(lk);
  };
}

function makeTranslatedSubscribe<T, U>(
  def: U,
  chk: typecheck<T>,
  xlate: (val: T) => U | undefined,
): Subscriber<U> {
  return (key: string, callback: (value: U) => void, defaultValue: U) => {
    const lk = SubscribeUnsafe(SocketMsgFromString(key), (val: unknown) => {
      if (chk(val)) {
        const xlateVal = xlate(val);
        callback(isDefined(xlateVal) ? xlateVal : def);
      } else {
        callback(def);
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
  };
}

export function getMainReadOnlyStorage<T>(chk: typecheck<T>): AsyncStorage<T> {
  return {
    getItem: makeGetItem(chk),
    setItem: noSetItem,
    removeItem: noRemoveItem,
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
  def: T,
  chk: typecheck<U>,
  fromMain: (val: U) => T,
  toMain: (val: T) => U,
): AsyncStorage<T> {
  return {
    getItem: makeGetTranslatedItem(chk, fromMain),
    setItem: async (k, v) => setTranslatedItem(k, toMain(v), fromMain),
    removeItem,
  };
}

function getTranslatedMainReadOnlyStorage<T, U>(
  def: T,
  chk: typecheck<U>,
  fromMain: (val: U) => T,
): AsyncStorage<T> {
  return {
    getItem: makeGetTranslatedItem(chk, fromMain),
    setItem: noSetItem,
    removeItem: noRemoveItem,
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
    getTranslatedMainStorage<T, U>(init, chk, fromMain, toMain),
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
    getTranslatedMainReadOnlyStorage<T, U>(init, chk, fromMain),
  );
}

export function getTranslatedSubscribe<T, U>(
  key: SocketMsg,
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
