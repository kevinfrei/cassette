import { Fail } from '@freik/react-tools';
import { atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import { Album, AlbumKey, SongKey } from 'www/Shared/CommonTypes';
import { musicLibraryState } from './MusicLibrary';
import { maybeSongByKey } from './Songs';

export const allAlbumsState = atom(async (get) => {
  return (await get(musicLibraryState)).albums as Map<AlbumKey, Album>;
});

export const maybeAlbumByKey = atomFamily((ak: AlbumKey) =>
  atom(async (get) => (await get(allAlbumsState)).get(ak)),
);

export const albumByKey = atomFamily((ak: AlbumKey) =>
  atom(async (get) => {
    const album = await get(maybeAlbumByKey(ak));
    if (!album) {
      Fail(ak, 'Unfound album key');
    }
    return album;
  }),
);

export const albumKeyFromSongKey = atomFamily((sk: SongKey) =>
  atom(async (get): Promise<AlbumKey> => {
    if (sk.length > 0) {
      const song = await get(maybeSongByKey(sk));
      return song ? song.albumId : '';
    } else {
      return '';
    }
  }),
);
