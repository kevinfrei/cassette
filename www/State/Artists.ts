import { Fail } from '@freik/react-tools';
import { atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import { Artist, ArtistKey } from 'www/Shared/CommonTypes';
import { allAlbumsState } from './Albums';
import { musicLibraryState } from './MusicLibrary';
import {
  minSongCountForArtistListState,
  showArtistsWithFullAlbumsState,
} from './SimpleSettings';

export const allArtistsState = atom(async (get) => {
  return (await get(musicLibraryState)).artists;
});

export const maybeArtistByKey = atomFamily((ak: ArtistKey) =>
  atom(async (get) => (await get(allArtistsState)).get(ak)),
);

export const artistByKey = atomFamily((ak: ArtistKey) =>
  atom(async (get) => {
    const artist = await get(maybeArtistByKey(ak));
    if (!artist) {
      Fail(ak, 'Unfound artist key');
    }
    return artist;
  }),
);

export const artistStringStateFamily = atomFamily((artistList: ArtistKey[]) =>
  atom(async (get): Promise<string> => {
    if (artistList.length === 0) {
      return '';
    }
    const artists: string[] = (
      await Promise.all(
        artistList.map(async (ak) => {
          const art: Artist = await get(artistByKey(ak));
          return art ? art.name : '';
        }),
      )
    ).filter((a: string) => a.length > 0);
    if (artists.length === 1) {
      return artists[0];
    } else {
      const lastPart = ' & ' + (artists.pop() || ''); // asssert :)
      return artists.join(', ') + lastPart;
    }
  }),
);

export const filteredArtistsState = atom(async (get) => {
  const fullAlbums = await get(showArtistsWithFullAlbumsState);
  const minSongCount = await get(minSongCountForArtistListState);
  const artists = [...(await get(allArtistsState)).values()];
  if (fullAlbums) {
    const albums = await get(allAlbumsState);
    return artists.filter((artist) => {
      for (const lKey of artist.albums) {
        const album = albums.get(lKey);
        if (album && album.primaryArtists.indexOf(artist.key) >= 0) {
          return true;
        }
      }
      return false;
    });
  } else if (minSongCount > 1) {
    return artists.filter((artist) => artist.songs.length >= minSongCount);
  }
  return artists;
});
