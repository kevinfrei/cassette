import { PlaybackControls } from './PlaybackControls';
import { ReactElement, useRef } from 'react';
import { SongPlaying } from './SongPlaying';
import { VolumeControl } from './VolumeControl';
import { Sidebar } from './Sidebar';

import '../styles/App.css';
import { ViewSelector } from './Views/Selector';

export function App(): ReactElement {
  const audioRef = useRef<HTMLAudioElement>(null);
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
  );
}
