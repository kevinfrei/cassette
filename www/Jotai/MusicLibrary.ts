import { MakeLog } from '@freik/logger';
import { isDefined, typecheck } from '@freik/typechk';
import { atomWithStorage } from 'jotai/utils';
import { chkMusicDatabase, IpcCall, SocketMsg } from 'www/Shared/CommonTypes';
import { CallMain, Subscribe, Unsubscribe } from 'www/Tools/Ipc';
import {
  emptyLibrary,
  isFlatAudioDatabase,
  MakeMusicLibraryFromFlatAudioDatabase,
  MusicLibrary,
} from '../MusicLibrarySchema';
import { getTranslatedSubscribe } from './Storage';

const { log, err } = MakeLog('EMP:render:Jotai:MusicLibrary');

async function getTranslatedMusicDB(): Promise<MusicLibrary> {
  try {
    log('Calling GetMusicDatabase');
    const mdb = await CallMain(IpcCall.GetMusicDatabase, chkMusicDatabase);
    if (isDefined(mdb)) {
      log('Got a value from GetMusicDatabase');
      return mdb;
    }
  } catch {
    /* */
  }
  err('Failed to get a value from GetMusicDatabase');
  return emptyLibrary;
}

const subscribe = {
  getItem: getTranslatedMusicDB,
};

export const musicLibraryState = atomWithStorage(
  SocketMsg.MusicDBUpdate,
  emptyLibrary,
  {
    getItem: getTranslatedMusicDB,
    setItem: Promise.resolve,
    removeItem: Promise.resolve,
    subscribe: getTranslatedSubscribe,
  },
);
