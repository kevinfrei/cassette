import { FontIcon, ISliderStyles, Slider } from '@fluentui/react';
import { useAtom } from 'jotai';

import { mutedState, volumeState } from 'www/State/SimpleSavedState';

import { ReactElement } from 'react';
import '../styles/VolumeControl.css';

export const mySliderStyles: Partial<ISliderStyles> = {
  thumb: {
    borderWidth: 1,
    width: 6,
    height: 10,
    top: -3,
    zIndex: 100,
  },
  line: {
    zIndex: 100,
  },
};

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
