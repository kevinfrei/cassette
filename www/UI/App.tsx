import { ReactElement, useRef } from 'react';
import { PlaybackControls } from './PlaybackControls';
import { Sidebar } from './Sidebar';
import { SongPlaying } from './SongPlaying';
import { VolumeControl } from './VolumeControl';

import { Provider } from 'jotai';
import { KeepAlive } from 'www/KeepAlive';
import { getStore } from 'www/State/Storage';
import { MusicDbListener } from 'www/Tools/MusicDbListener';
import { WebSocketRegistrar } from 'www/Tools/ReactWebSocket';
import '../styles/App.css';
import { ViewSelector } from './Views/Selector';

function Tools(): ReactElement {
  return (
    <>
      <WebSocketRegistrar />
      <MusicDbListener />
      <KeepAlive />
    </>
  );
}

function TheActualApp(): ReactElement {
  /*  const audioRef = useRef<HTMLAudioElement>(null);
  return (
    <div id="app">
      <span id="left-column" />
      <span id="top-row" />
      <PlaybackControls audioRef={audioRef} />
      <SongPlaying ref={audioRef} />
      <VolumeControl />
      <Sidebar />
      <ViewSelector />
      <div className="SongDetailPanel" />
    </div>
  );*/
  return (
    <audio
      autoPlay={true}
      src={'../tune/test.flac'}
      onPlay={() => {}}
      onPause={() => {}}
      onEnded={() => {}}
      onTimeUpdate={() => {}}
      muted={false}
    />
  );
}

export function App(): ReactElement {
  const store = getStore();
  return (
    <Provider store={store}>
      <Tools />
      <TheActualApp />
    </Provider>
  );
}
