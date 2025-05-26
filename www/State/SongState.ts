import { Atom, atom } from "jotai";
import { atomFamily } from "jotai/utils";

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

export const curSongKeyState = atom<string>('');
export const songDescriptionForSongState = atomFamily(
  (songKey: string) => atom<SongDescription>(defSongDescr));
);