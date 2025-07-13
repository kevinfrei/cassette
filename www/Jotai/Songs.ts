import { Fail } from '@freik/react-tools';
import { atom, Atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import { AlbumKey, ArtistKey, MediaKey, SongKey } from 'www/Shared/CommonTypes';
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

export const songListByKey = atomFamily<MediaKey, Atom<Promise<SongKey[]>>>(
  (key: MediaKey) =>
    atom(async (get) => {
      if (key.length < 2) {
        return [];
      }
      return [];
    }),
);
