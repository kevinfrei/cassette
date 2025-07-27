import { Provider, useAtomValue, useSetAtom } from 'jotai';
import {
  ReactElement,
  SyntheticEvent,
  useCallback,
  useEffect,
  useRef,
} from 'react';
import { KeepAlive } from 'www/KeepAlive';
import { useJotaiAsyncCallback } from 'www/State/Helpers';
import { mutedState } from 'www/State/SimpleSavedState';
import { repeatState, songListState } from 'www/State/SongPlayback';
import { getStore } from 'www/State/Storage';
import { MediaTime, mediaTimeState } from 'www/State/TimeState';
import { MusicDbListener } from 'www/Tools/MusicDbListener';
import { WebSocketRegistrar } from 'www/Tools/ReactWebSocket';
import { isValidRefObject } from 'www/Utils';
import { MaybePlayNext, PlaybackControls } from './PlaybackControls';
import { Sidebar } from './Sidebar';
import { SongPlaying } from './SongPlaying';
import { ViewSelector } from './Views/Selector';
import { VolumeControl } from './VolumeControl';

import '../styles/App.css';

type AudioElementProps = { audioRef: React.Ref<HTMLAudioElement> };

function AudioElement({ audioRef }: AudioElementProps): ReactElement {
  const setMediaTime = useSetAtom(mediaTimeState);
  const isMuted = useAtomValue(mutedState);
  const onEnded = useJotaiAsyncCallback(async (get, set) => {
    /* TODO: What happens when the song ends? */
    // log('Heading to the next song!!!');
    const songList = await get(songListState);
    const rep = await get(repeatState);
    if (rep && songList.length === 1) {
      // Because we rely on auto-play, if we just try to play the same song
      // again, it won't start playing
      if (isValidRefObject<HTMLAudioElement>(audioRef)) {
        void audioRef.current.play();
      }
    } else {
      await MaybePlayNext();
    }
  }, []);

  const onTimeUpdate = useCallback(
    (ev: SyntheticEvent<HTMLMediaElement>) => {
      const ae = ev.currentTarget;
      if (!Number.isNaN(ae.duration)) {
        setMediaTime((prevTime: MediaTime) => {
          if (
            Math.trunc(ae.duration) !== Math.trunc(prevTime.duration) ||
            Math.trunc(ae.currentTime) !== Math.trunc(prevTime.position)
          ) {
            return { position: ae.currentTime, duration: ae.duration };
          } else {
            return prevTime;
          }
        });
      }
    },
    [setMediaTime],
  );
  return (
    <audio
      // src={songKey !== '' ? '/tune/' + songKey : ''}
      src={'/tune/song.m4a'}
      ref={audioRef}
      onEnded={onEnded}
      onTimeUpdate={onTimeUpdate}
      muted={isMuted}
      controls={false}
    />
  );
}

function TheActualApp(): ReactElement {
  const audioRef = useRef<HTMLAudioElement>(null);
  return (
    <div id="app">
      <AudioElement audioRef={audioRef} />
      <span id="left-column" />
      <span id="top-row" />
      <PlaybackControls audioRef={audioRef} />
      <SongPlaying ref={audioRef} />
      <VolumeControl />
      <Sidebar />
      <ViewSelector />
      <div className="SongDetailPanel" />
    </div>
  );
}

export function App(): ReactElement {
  const store = getStore();
  return (
    <Provider store={store}>
      <WebSocketRegistrar />
      <MusicDbListener />
      <KeepAlive />
      <TheActualApp />
    </Provider>
  );
}
