import { Ipc } from '@freik/electron-render';
import { IpcId } from '@freik/emp-shared';
import { MakeLog } from '@freik/logger';
import { atomWithStorage } from 'jotai/utils';
import {
  emptyLibrary,
  isFlatAudioDatabase,
  MakeMusicLibraryFromFlatAudioDatabase,
  MusicLibrary,
} from '../MusicLibrarySchema';

const { log, err } = MakeLog('EMP:render:Jotai:MusicLibrary');

async function getTranslatedMusicDB(): Promise<MusicLibrary> {
  try {
    log('Calling GetMusicDatabase');
    const strValue = await Ipc.CallMain(
      IpcId.GetMusicDatabase,
      undefined,
      isFlatAudioDatabase,
    );
    if (strValue) {
      log('Got a value from GetMusicDatabase');
      return MakeMusicLibraryFromFlatAudioDatabase(strValue);
    }
  } catch {
    /* */
  }
  err('Failed to get a value from GetMusicDatabase');
  return emptyLibrary;
}

function getTranslatedSubscribe(
  key: string,
  callback: (value: MusicLibrary) => void,
  initialValue: MusicLibrary,
) {
  const lk = Ipc.Subscribe(key, (val: unknown) => {
    log('Got a value from the MusicDBUpdate subscription');
    if (isFlatAudioDatabase(val)) {
      log("It's a flat audio database: Calling subscriber.");
      callback(MakeMusicLibraryFromFlatAudioDatabase(val));
      log('Subscriber completed.');
    } else {
      log(
        "It's not a flat audio database: Calling subscriber with initial value.",
      );
      callback(initialValue);
      log('Subscriber completed (initValue).');
    }
  });
  return () => Ipc.Unsubscribe(lk);
}

export const musicLibraryState = atomWithStorage(
  IpcId.MusicDBUpdate,
  emptyLibrary,
  {
    getItem: getTranslatedMusicDB,
    setItem: Promise.resolve,
    removeItem: Promise.resolve,
    subscribe: getTranslatedSubscribe,
  },
);
