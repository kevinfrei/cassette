import { Spinner, SpinnerSize } from '@fluentui/react';
// import { Spinner } from '@freik/fluentui-tools';

import '../styles/App.css';
import { PlaybackControls } from './PlaybackControls';
import { useRef } from 'react';
import { SongPlaying } from './SongPlaying';

export function App(): JSX.Element {
  const audioRef = useRef<HTMLAudioElement>(null);
  return (
    <div id="app">
      <span id="left-column" />
      <span id="top-row" />
      <PlaybackControls audioRef={audioRef} />
      <SongPlaying ref={audioRef} />
      <div className="Spinner">
        <div className="VolumeControl">Volume Control</div>
      </div>
      <div className="Spinner">
        <div className="Sidebar">Sidebar</div>
      </div>
      <div className="Spinner">
        <div className="ViewSelector">View Selector</div>
      </div>
      <div className="SongDetailPanel">Song Detail Panel</div>
    </div>
  );
}
