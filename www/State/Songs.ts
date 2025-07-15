import { isAlbumKey, isArtistKey, isSongKey } from '@freik/media-core';
import { Fail } from '@freik/react-tools';
import { atom, Atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import { AlbumKey, ArtistKey, MediaKey, SongKey } from 'www/Shared/CommonTypes';
import { albumByKey } from './Albums';
import { artistByKey } from './Artists';
import { musicLibraryState } from './MusicLibrary';

export const allSongsState = atom(
  async (get) => (await get(musicLibraryState)).songs,
);

export const maybeSongByKey = atomFamily((sk: SongKey) =>
  atom(async (get) => (await get(allSongsState)).get(sk)),
);

export const songByKey = atomFamily((sk: SongKey) =>
  atom(async (get) => {
    const song = await get(maybeSongByKey(sk));
    if (!song) {
      Fail(sk, 'Unfound song key');
    }
    return song;
  }),
);

export const songListFromKey = atomFamily<MediaKey, Atom<Promise<SongKey[]>>>(
  (key: MediaKey) =>
    atom(async (get) => {
      if (key.length < 2) {
        return [];
      } else if (isSongKey(key)) {
        return [key];
      } else if (isAlbumKey(key)) {
        const alb = await get(albumByKey(key));
        if (alb) {
          return alb.songs;
        } else {
          return [];
        }
      } else if (isArtistKey(key)) {
        const art = await get(artistByKey(key));
        if (art) {
          return art.songs;
        } else {
          return [];
        }
      }
      return [];
    }),
);
