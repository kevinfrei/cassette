import { ReactElement, useEffect } from 'react';

import {
  chkMusicDatabase,
  MusicDatabase,
  SocketMsg,
  Song,
  SongKey,
} from '../Shared/CommonTypes';
import { Subscribe, Unsubscribe } from './Ipc';

export function MusicDbListener(): ReactElement {
  useEffect(() => {
    console.error('Registering MusicDbListener');
    const lk = Subscribe(
      SocketMsg.MusicDBUpdate,
      chkMusicDatabase,
      (data: MusicDatabase) => {
        // Handle the updated music database here
        console.error('Received MusicDBUpdate:', data);
        // You can update your state or perform other actions as needed
      },
    );
    return () => {
      console.error('Unregistering MusicDbListener');
      Unsubscribe(lk);
    };
  });
  return <></>;
}
