import { ReactElement, useCallback, useEffect } from 'react';
import { useAtom } from 'jotai';

import { Button, Slider, SliderProps } from '@fluentui/react-components';
import {
  Speaker024Filled,
  Speaker124Filled,
  Speaker224Filled,
  SpeakerMute24Filled,
} from '@fluentui/react-icons';

import { mutedState, volumeState } from '../State/SongPlayback';
import { isValidRefObject } from '../Utils';

import '../styles/VolumeControl.css';

const volumeIcons = [
  <SpeakerMute24Filled />,
  <Speaker024Filled />,
  <Speaker124Filled />,
  <Speaker224Filled />,
];

export function VolumeControl({
  audioRef,
}: {
  audioRef: React.Ref<HTMLAudioElement>;
}): ReactElement {
  const [muted, setMuted] = useAtom(mutedState);
  const [volume, setVolume] = useAtom(volumeState);
  // Make the icon reflect approximate volume
  const iconNum = muted ? 0 : 1 + Math.min(2, Math.floor(3 * volume));
  useEffect(() => {
    if (isValidRefObject<HTMLAudioElement>(audioRef)) {
      audioRef.current.muted = muted;
      audioRef.current.volume = volume;
    }
  }, [audioRef, muted, volume]);
  const onSliderChange: SliderProps['onChange'] = useCallback((_, data) => {
    void setVolume(data.value);
    if (muted) void setMuted(false);
  }, []);
  return (
    <span id="volume-container">
      <Button
        appearance="transparent"
        id={muted ? 'mute' : 'volIcon'}
        icon={volumeIcons[iconNum]}
        onClick={() => void setMuted(!muted)}
        style={{ cursor: 'pointer' }}
      />
      <Slider
        id="volume-slider"
        min={0}
        max={1}
        value={volume}
        step={0.01}
        onChange={onSliderChange}
      />
    </span>
  );
}
