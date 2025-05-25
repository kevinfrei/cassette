import { SpinnerSize } from '@fluentui/react';
import { Spinner } from '@freik/fluentui-tools';

import './styles/App.css';

export function App(): JSX.Element {
  // const audioRef = useRef<HTMLAudioElement>(null);
  return (
    <>
      <span id="left-column" />
      <span id="top-row" />
      <Spinner>
        <div className="PlaybackControls" id="audioRef=[audioRef]">
          Playback Controls
        </div>
      </Spinner>
      <Spinner>
        <div className="ErrorBoundary">
          <div id="SongPlaying ref={audioRef}">Song Playing</div>
        </div>
      </Spinner>
      <Spinner>
        <div className="VolumeControl">Volume Control</div>
      </Spinner>
      <Spinner>
        <div className="Sidebar">Sidebar</div>
      </Spinner>
      <Spinner size={SpinnerSize.large}>
        <div className="ViewSelector">View Selector</div>
      </Spinner>
      <div className="SongDetailPanel">Song Detail Panel</div>
    </>
  );
}
