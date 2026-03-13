import { atom } from 'jotai';
import { atomFamily } from 'jotai-family';
import { SongKey } from 'www/Shared/CommonTypes';
import { albumByKey } from './Albums';
import { artistStringStateFamily } from './Artists';
import { songByKey } from './Songs';

export type AlbumDescription = {
  artist: string;
  album: string;
  year: string;
};

export type AlbumDescriptionWithKey = {
  key: string; // TODO: AlbumKey;
} & AlbumDescription;

export type SongDescription = {
  title: string;
  track: number;
} & AlbumDescription;

const defSongDescr: SongDescription = {
  title: `title`,
  track: 1,
  artist: `artist`,
  album: `album`,
  year: '2023',
};

export const songDescriptionForSongState = atomFamily((songKey: SongKey) =>
  atom(async (get): Promise<SongDescription> => {
    try {
      const song = await get(songByKey(songKey));
      const title = song.title;
      const track = song.track;
      const [artist, albumObj] = await Promise.all([
        get(artistStringStateFamily(song.artistIds)),
        get(albumByKey(song.albumId)),
      ]);
      const album = albumObj.title;
      const year = albumObj.year === 0 ? '' : albumObj.year.toString();
      return { track, title, album, artist, year };
    } catch {
      return defSongDescr;
    }
  }),
);

export const songDescriptionsForSongList = atomFamily((sks: SongKey[]) =>
  atom(
    async (get): Promise<SongDescription[]> =>
      Promise.all(
        sks.map((sk: SongKey) => get(songDescriptionForSongState(sk))),
      ),
  ),
);
