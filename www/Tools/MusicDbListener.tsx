import { ReactElement, useEffect } from 'react';
import {
  chkMusicDatabase,
  MusicDatabase,
  SocketMsg,
  Song,
  SongKey,
} from 'www/Shared/CommonTypes';
import { Subscribe, Unsubscribe } from './Ipc';

export function MusicDbListener(): ReactElement {
  useEffect(() => {
    const lk = Subscribe(
      SocketMsg.MusicDBUpdate,
      chkMusicDatabase,
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
