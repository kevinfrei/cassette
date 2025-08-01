import { MakeLog } from '@freik/logger';
import { onRejected } from '@freik/react-tools';
import { useAtom } from 'jotai';
import { ForwardedRef, ReactElement, useCallback } from 'react';
import { Keys } from 'www/Shared/CommonTypes';
import { NextRepeat, repeatState, shuffleState } from 'www/State/SongPlayback';
import { getStore, MyStore } from 'www/State/Storage';
import { playingState } from 'www/State/TimeState';
import { GetHelperText, isValidRefObject } from 'www/Utils';

import '../styles/PlaybackControls.css';

const { log, wrn } = MakeLog('EMP:render:SongControls');

export function onClickPlayPause(
  audioRef: ForwardedRef<HTMLAudioElement>,
  maybeStore?: MyStore,
): void {
  if (!isValidRefObject<HTMLAudioElement>(audioRef)) {
    wrn('Clicking but no audio element');
    return;
  }
  getStore(maybeStore).set(playingState, (isPlaying) => {
    if (isPlaying) {
      audioRef.current.pause();
      return false;
    } else if (audioRef.current.readyState === 4) {
      audioRef.current.play().catch(onRejected('Audio Element play failed'));
      return true;
    } else {
      log(
        `We're not playing, but also not state 4: ${audioRef.current.readyState}`,
      );
    }
    return isPlaying;
  });
}

export function MaybePlayNext() {
  // TODO: Implement this function
}

export function MaybePlayPrev() {
  // TODO: Implement this function
}

export type PlaybackControlsProps = {
  audioRef: ForwardedRef<HTMLAudioElement>;
};

export function PlaybackControls({
  audioRef,
}: PlaybackControlsProps): ReactElement {
  const isPlaying = true; // TODO: useAtomValue(playingState);

  const hasAnySong = true; // TODO: useRecoilValue(hasAnySongsFunc);
  const [shuf, setShuf] = useAtom(shuffleState);
  const [rep, setRep] = useAtom(repeatState);
  const hasNextSong = true; // useRecoilValue(hasNextSongFunc);
  const hasPrevSong = true; // useRecoilValue(hasPrevSongFunc);

  const shufClass = shuf ? 'enabled' : 'disabled';
  const repClass = rep ? 'enabled' : 'disabled';
  const playPauseClass = isPlaying
    ? 'playing'
    : hasAnySong
      ? 'paused'
      : 'paused disabled';
  const nextClass = hasNextSong ? 'enabled' : 'disabled';
  const prevClass = hasPrevSong ? 'enabled' : 'disabled';

  const clickShuffle = useCallback(() => setShuf(!shuf), [shuf]);
  const clickRepeat = useCallback(() => setRep(NextRepeat(rep)), [rep]);
  const clickPlayPause = useCallback(() => onClickPlayPause(audioRef), []);

  const clickPrev = useCallback(() => {
    if (hasPrevSong) {
      MaybePlayPrev();
    }
  }, [hasPrevSong]);
  const clickNext = useCallback(() => {
    if (hasNextSong) {
      MaybePlayNext();
    }
  }, []);
  return (
    <span id="control-container">
      <span
        id="shuffle"
        className={shufClass}
        onClick={clickShuffle}
        title={GetHelperText(Keys.Shuffle)}>
        &nbsp;
      </span>
      <span
        id="prev"
        className={prevClass}
        onClick={clickPrev}
        title={GetHelperText(Keys.PreviousTrack)}>
        &nbsp;
      </span>
      <span
        id="play-pause"
        className={playPauseClass}
        onClick={clickPlayPause}
        title={GetHelperText(Keys.Play)}>
        &nbsp;
      </span>
      <span
        id="next"
        className={nextClass}
        onClick={clickNext}
        title={GetHelperText(Keys.NextTrack)}>
        &nbsp;
      </span>
      <span
        id="repeat"
        className={repClass}
        onClick={clickRepeat}
        title={GetHelperText(Keys.Repeat)}>
        &nbsp;
      </span>
      &nbsp;
    </span>
  );
}
