import { FontIcon, ISliderStyles, Slider } from '@fluentui/react';
import { useAtom } from 'jotai';
import { ReactElement } from 'react';
import { mutedState, volumeState } from 'www/State/SongPlayback';
import { mySliderStyles } from 'www/Tools/Utilities';

import '../styles/VolumeControl.css';

export function VolumeControl(): ReactElement {
  const [muted, setMuted] = useAtom(mutedState);
  const [volume, setVolume] = useAtom(volumeState);
  // Make the icon reflect approximate volume
  const iconNum = Math.min(3, Math.floor(4 * (volume + 0.1))).toString();
  const cls = 'volume-container-win-linux'; // Yoinked Mac, cuz I have a title bar
  return (
    <span className={cls} id="volume-container">
      <FontIcon
        id={muted ? 'mute' : 'volIcon'}
        iconName={muted ? 'VolumeDisabled' : `Volume${iconNum}`}
        onClick={() => void setMuted(!muted)}
        style={{ cursor: 'pointer' }}
      />
      <Slider
        className="volume-slider"
        styles={mySliderStyles}
        min={0}
        max={1}
        value={volume}
        step={0.01}
        showValue={false}
        onChange={(value: number) => {
          void setVolume(value);
          if (muted) void setMuted(false);
        }}
      />
    </span>
  );
}
