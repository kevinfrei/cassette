import {
  chkCustomType,
  chkMapOf,
  chkObjectOfType,
  hasFieldOf,
  isObjectNonNull,
  isString,
  typecheck,
} from '@freik/typechk';
import { ReactElement, useEffect } from 'react';
import { IpcMsg, MusicDatabase, Song, SongKey } from 'www/Shared/CommonTypes';
import { Subscribe, Unsubscribe } from './Ipc';

const isSongKey: typecheck<SongKey> = isString;
const isSong = chkObjectOfType<Song>({
  track: 'number',
  title: 'string',
  album: 'object', // This should be a more specific type if available
  artists: 'array', // This should be a more specific type if available
  secondaryArtists: 'array', // This should be a more specific type if available
  variations: 'array',
});
// TODO: Fix this, like maybe generate the typeguard from the IDL compiler?
function isMusicDatabase(data: unknown): data is MusicDatabase {
  if (!isObjectNonNull(data)) {
    return false;
  }
  if (hasFieldOf(data, 'songs', chkMapOf(isSongKey, isSong)) /* && */) {
  }
  const db = data as MusicDatabase;
  return (
    db !== null &&
    typeof db === 'object' &&
    Array.isArray(db.songs) &&
    Array.isArray(db.albums) &&
    Array.isArray(db.artists) &&
    Array.isArray(db.playlists)
  );
}

export function DataListener(): ReactElement {
  useEffect(() => {
    const lk = Subscribe(
      IpcMsg.MusicDBUpdate,
      isMusicDatabase,
      (data: MusicDatabase) => {
        // Handle the updated music database here
        console.log('Received MusicDBUpdate:', data);
        // You can update your state or perform other actions as needed
      },
    );
    return () => {
      Unsubscribe(lk);
    };
  });
  return <></>;
}
