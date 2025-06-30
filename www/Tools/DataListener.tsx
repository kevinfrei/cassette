import { ReactElement, useEffect } from 'react';
import { IpcMsg, MusicDatabase } from 'www/Shared/CommonTypes';
import { Subscribe, Unsubscribe } from './Ipc';

// TODO: Fix this, like maybe generate the typeguard from the IDL compiler?
function isMusicDatabase(data: unknown): data is MusicDatabase {
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
