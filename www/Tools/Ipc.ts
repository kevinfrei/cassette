import { MakeLog } from '@freik/logger';
import MakeSeqNum from '@freik/seqnum';
import {
  isDefined,
  isObjectNonNull,
  isString,
  Pickle,
  SafelyUnpickle,
  typecheck,
} from '@freik/typechk';
import { IpcId } from 'www/Shared/CommonTypes';
import { ListenKey, MyWindow } from 'www/Types';

const { con, log, wrn, err } = MakeLog('tools:ipc');

declare const window: MyWindow;

/**
 * @async
 * Read a key-value from Electron persistent storage
 *
 * @param key The key to read
 * @returns A promise that resolves to the value read (or void if none found)
 */
export async function ReadFromStorage<T>(
  key: string,
  typechk: typecheck<T>,
): Promise<T | void> {
  return CallMain(IpcId.ReadFromStorage, typechk, key);
}

/**
 * @async
 * Write a key-value pair to Electron persistent storage
 *
 * @param key The key to write
 * @param data The value to be written
 */
export function WriteToStorage<T>(key: string, data: T): void {
  PostMain(IpcId.WriteToStorage, key, Pickle(data)).catch((err) => {
    err(`Failed to write to storage for key "${key}":`, err);
  });
}

/**
 * @async
 * Delete a key (and it's value...) from Electron persistent storage
 *
 * @param key The key to delete
 */
export function DeleteFromStorage(key: string): void {
  SendMessage(IpcId.DeleteFromStorage, key).catch((err) => {
    err(`Failed to delete from storage for key "${key}":`, err);
  });
}

const getNextListenerSeqNum = MakeSeqNum('Listen');

// map of message names to map of id's to funtions
type Handler<T> = { typechk: typecheck<T>; handler: (data: T) => void };
const listeners = new Map<IpcId, Map<string, Handler<any>>>();

/**
 * This subscribes the `handler` to listen for messages coming from the
 * main process.
 *
 * @param key The message key identified to listen for
 * @param handler The function to invoke upon receipt of the message
 * @returns the key to use to unsubscribe
 */
export function Subscribe<T>(
  ipcId: IpcId,
  typechk: typecheck<T>,
  handler: (data: T) => void,
): ListenKey {
  const theKey = { ipcId, id: getNextListenerSeqNum() };
  let handlerMap = listeners.get(ipcId);
  if (!handlerMap) {
    handlerMap = new Map<string, Handler<any>>();
    listeners.set(ipcId, handlerMap);
  }
  handlerMap.set(theKey.id, { typechk, handler });
  return theKey;
}

/**
 * Unsubscribe from a particular message
 *
 * @param listenKey The key returned by {@link Subscribe}
 */
export function Unsubscribe(listenKey: ListenKey): void {
  const lstn = listeners.get(listenKey.ipcId);
  if (lstn) {
    lstn.delete(listenKey.id);
  }
}

let ipcAttempts = 0;

export function WireUpIpc(): void {
  const ws = window.ws;
  if (!ws) {
    ipcAttempts++;
    setTimeout(WireUpIpc, 100);
    if (ipcAttempts % 10 === 0) {
      console.error(
        `Failed to wire up IPC after ${ipcAttempts} attempts. Please ensure the ReactWebScoket module is loaded.`,
      );
    }
    return;
  }
  con(`Wiring up IPC with WebSocket: ${ws.url}`);
  ws.onmessage = (evt: MessageEvent) => {
    const message = evt.data;
    if (!isString(message)) {
      err(`Received non-string message from websocket:`, message);
      return;
    }
    log(`Received message: ${message}`);
    HandleMessage(message);
  };
  window.ipc = {
    post: (channel: IpcId, ...args: unknown[]) => {
      if (!isObjectNonNull(window.ws)) {
        err('IPC connector is not wired up, cannot invoke');
        throw Error('IPC connector is not wired up');
      }
      const msg = `${channel};${Pickle(args)}`;
      log(`Sending message: ${msg}`);
      window.ws.send(msg);
    },
    // TODO: Listen for isDev, maybe?
  };
}

const messageFormat = new RegExp(`^([0-9]+);`);
// Called when an async message comes in from the main process
// I think these should just be subscribed to as part of a useEffect?
function HandleMessage(message: string): void {
  // Walk the list of ID's to see if we've got anything with a format of:
  // { "id" : data }
  // This has an interesting side effect of letting the server process
  // send multiple "messages" in a single message:
  // { artists: ..., albums: ..., songs: ... } will invoke listeners for
  // all three of those 'messages'
  let handled = false;
  // Messages should come in as a IpcId, a semicolon, then a JSON string
  const match = message.match(messageFormat);
  if (!match || match.length < 2) {
    wrn('>>> Async malformed message begin');
    wrn(message);
    wrn('<<< Async malformed message end');
    return;
  }
  const maybeIpcId = Number.parseInt(match[1], 10);
  if (isNaN(maybeIpcId)) {
    wrn('>>> Async malformed message begin');
    wrn(message);
    wrn('<<< Async malformed message end');
    return;
  }
  const ipcId = maybeIpcId as IpcId;
  if (Object.values(IpcId).indexOf(ipcId) < 0) {
    // This is a malformed message, we don't know what to do with it
    wrn('>>> Async malformed message begin');
    wrn(`Received message with unknown IpcId: ${ipcId}`);
    wrn(message);
    wrn('<<< Async malformed message end');
    return;
  }
  const messageData = message.substring(match[0].length);
  const listener = listeners.get(ipcId);
  if (!listener) {
    wrn(`>>> Received message with no listeners: ${ipcId}`, message);
    return;
  }
  listener.forEach((hndlr, id) => {
    const { typechk, handler } = hndlr;
    const val = SafelyUnpickle(messageData, typechk);
    if (!isDefined(val)) {
      wrn(
        `>>> Async malformed message begin for ${ipcId} with id ${id}`,
        `Failed to unpickle message data:`,
        messageData,
      );
      wrn('<<< Async malformed message end');
      return;
    }
    handled = true;
    log(`Handling message for ${ipcId} with id ${id}`);
    handler(val);
  });
  if (!handled) {
    wrn('**********');
    wrn('Unhandled message (IpcId: ', ipcId, ')');
    wrn(message);
    wrn('**********');
  }
}

export async function SendMessage<T>(
  channel: IpcId,
  ...args: unknown[]
): Promise<unknown> {
  if (!window.ipc) {
    // Wait a bit, as Jotai may call IPC stuff pretty early
    const sleep = (ms: number) => new Promise((r) => setTimeout(r, ms));
    for (let i = 0; i < 20; i++) {
      await sleep(i * i);
      if (window.ipc) {
        break;
      }
    }
  }
  if (!window.ipc) {
    err('InvokeMain called with no IPC connector wired in');
    throw Error('no connector wired in');
  }
  log(`Invoking main("${channel}", "...")`);
  // FIXME: May need a client id of some sort. Not sure...
  let result = await window.ipc.post(channel, ...args);
  log(`Invoke main ("${channel}" "...") returned:`);
  log(result);
  return result;
}

export async function RawGet(endpoint: string): Promise<string | undefined> {
  try {
    const response = await fetch(endpoint, {
      method: 'GET',
    });
    if (response.ok) {
      return await response.text();
    }
  } catch (err) {
    console.error(`Failed to fetch ${endpoint}:`, err);
  }
  return undefined;
}

async function Get(endpoint: IpcId, ...args: string[]): Promise<unknown> {
  try {
    const response = await fetch(
      ['/api', endpoint.toString(10), ...args].join('/'),
      {
        method: 'GET',
      },
    );
    if (response.ok) {
      return await response.json();
    }
  } catch (err) {
    return err;
  }
  return { error: `Failed to fetch ${endpoint} with args: ${args.join(', ')}` };
}

async function GetAs<T>(
  validator: typecheck<T>,
  endpoint: IpcId,
  ...args: string[]
): Promise<T | undefined> {
  const res = await Get(endpoint, ...args);
  return validator(res) ? res : undefined;
}

/**
 * @async
 * Call a remote function with type checking on the return value.
 * If you have no return type, use {@link PostMain} instead.
 *
 * @param channel The channel to send a message to
 * @param typecheck The typecheck function to validate the return type R
 * @param args The data to communicate to the channel (if any)
 * @returns A promise that resolves to the typechecked return value of the RPC
 */
export async function CallMain<T>(
  channel: IpcId,
  typecheck: typecheck<T>,
  ...args: unknown[]
): Promise<T | void> {
  return await GetAs(typecheck, channel, ...args.map((a) => String(a)));
}

export async function PostMain(
  channel: IpcId,
  ...args: unknown[]
): Promise<void> {
  void (await CallMain(channel, (a): a is void => true, ...args));
  // No return value, so we just return void
}
