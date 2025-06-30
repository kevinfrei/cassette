import { ReactElement, useRef } from 'react';
import { PlaybackControls } from './PlaybackControls';
import { Sidebar } from './Sidebar';
import { SongPlaying } from './SongPlaying';
import { VolumeControl } from './VolumeControl';

import { Provider } from 'jotai';
import { getStore } from 'www/Jotai/Storage';
import { KeepAlive } from 'www/KeepAlive';
import { WebSocketRegistrar } from 'www/Tools/ReactWebSocket';
import '../styles/App.css';
import { ViewSelector } from './Views/Selector';

export function App(): ReactElement {
  const audioRef = useRef<HTMLAudioElement>(null);
  const store = getStore();
  return (
    <Provider store={store}>
      <WebSocketRegistrar />
      <DataListener />
      <KeepAlive />
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
    </Provider>
  );
}
