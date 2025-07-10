import { MakeLog } from '@freik/logger';
import { atomWithStorage } from 'jotai/utils';
import {
  Album,
  AlbumKey,
  Artist,
  ArtistKey,
  chkMusicDatabase,
  MusicDatabase,
  Playlist,
  SocketMsg,
  Song,
  SongKey,
} from 'www/Shared/CommonTypes';
import { makeSubscribe } from './Storage';

const { log, err } = MakeLog('EMP:render:Jotai:MusicLibrary');

const emptyLibrary: MusicDatabase = {
  artists: new Map<ArtistKey, Artist>(),
  albums: new Map<AlbumKey, Album>(),
  songs: new Map<SongKey, Song>(),
  playlists: new Map<string, Playlist>(),
};

export const musicLibraryState = atomWithStorage(
  SocketMsg.MusicDBUpdate.toString(),
  emptyLibrary,
  {
    getItem: () => emptyLibrary,
    setItem: Promise.resolve,
    removeItem: Promise.resolve,
    subscribe: makeSubscribe(chkMusicDatabase),
  },
);
