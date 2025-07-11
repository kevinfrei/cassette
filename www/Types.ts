import {
  Album,
  AlbumKey,
  IpcCall,
  SocketMsg,
  Song,
  SongKey,
  VAType,
} from './Shared/CommonTypes';

export type ListenKey = { ipcId: SocketMsg; id: string };

export type Ipc = {
  post: (id: IpcCall, ...args: unknown[]) => void;
};

export interface MyWindow extends Window {
  ws?: WebSocket;
  // clientId?: number;
  ipc?: Ipc;
  wsport?: number;
}

export type MetadataProps = {
  forSong?: SongKey;
  forSongs?: SongKey[];
  artist?: string;
  album?: string;
  track?: string;
  title?: string;
  year?: string;
  va?: VAType;
  variations?: string;
  moreArtists?: string;
  albumId?: AlbumKey;
  diskName?: string;
};

export type SongInfo = {
  song: Song;
  artists: string;
  moreArtists: string;
  album: Album;
};
