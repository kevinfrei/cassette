import { atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import { SongKey } from 'www/Shared/CommonTypes';

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

export const songDescriptionForSongState = atomFamily((songKey: string) =>
  atom<SongDescription>(defSongDescr),
);

export const songDescriptionsForSongList = atomFamily((sks: SongKey[]) =>
  atom(
    async (get): Promise<SongDescription[]> =>
      Promise.all(
        sks.map((sk: SongKey) => get(songDescriptionForSongState(sk))),
      ),
  ),
);
