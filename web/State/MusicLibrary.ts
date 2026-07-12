import { atomWithStorage } from 'jotai/utils';

import { MakeLog } from '@freik/logger';
import { isUndefined } from '@freik/typechk';

import {
  Album,
  AlbumKey,
  Artist,
  ArtistKey,
  chkFlatMusicDatabase,
  chkMusicDatabase,
  FlatMusicDatabase,
  IpcCall,
  MusicDatabase,
  Playlist,
  SocketMsg,
  Song,
  SongKey,
} from '../Shared/CommonTypes';
import { CallMain } from '../Tools/Ipc';
import { makeSubscribe } from './Storage';

const { log, err } = MakeLog('EMP:render:Jotai:MusicLibrary');

const emptyLibrary: MusicDatabase = {
  artists: new Map<ArtistKey, Artist>(),
  albums: new Map<AlbumKey, Album>(),
  songs: new Map<SongKey, Song>(),
};

function MakeMusicLibraryFromFlat(fdb: FlatMusicDatabase): MusicDatabase {
  const artists = new Map<ArtistKey, Artist>();
  for (const r of fdb.artists) {
    artists.set(r.key, r);
  }
  const albums = new Map<AlbumKey, Album>();
  for (const l of fdb.albums) {
    albums.set(l.key, l);
  }
  const songs = new Map<SongKey, Song>();
  for (const s of fdb.songs) {
    songs.set(s.key, s);
  }
  return { artists, albums, songs };
}

export const musicLibraryState = atomWithStorage(
  SocketMsg.MusicDBUpdate.toString(),
  emptyLibrary,
  {
    getItem: async (key, init) => {
      const fdb = await CallMain<FlatMusicDatabase>(
        IpcCall.GetMusicDatabase,
        chkFlatMusicDatabase,
      );
      if (isUndefined(fdb)) {
        return init;
      }
      return MakeMusicLibraryFromFlat(fdb);
    },
    setItem: Promise.resolve,
    removeItem: Promise.resolve,
    subscribe: makeSubscribe(chkMusicDatabase),
  },
);
