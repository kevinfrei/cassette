import { Slider, Text } from '@fluentui/react';
import { ListIcon } from '@fluentui/react-icons-mdl2';
import { MakeLog } from '@freik/logger';
import { useAtom, useAtomValue } from 'jotai';
import { ForwardedRef, forwardRef, ReactElement, useEffect } from 'react';
import { MaybePlayNext } from 'www/State/API';
import { useJotaiAsyncCallback } from 'www/State/Helpers';
import {
  curSongKeyState,
  mutedState,
  playOrderDisplayingState,
  repeatState,
  shuffleState,
  songListState,
  volumeState,
} from 'www/State/SongPlayback';
import { allSongsState } from 'www/State/Songs';
import {
  SongDescription,
  songDescriptionForSongState,
} from 'www/State/SongState';
import {
  MediaTime,
  mediaTimePercentState,
  mediaTimePositionState,
  mediaTimeRemainingState,
  mediaTimeState,
} from 'www/State/TimeState';
import { isValidRefObject } from 'www/Utils';
import { SongDetailClick } from './DetailPanel/Clickers';
import { onClickPlayPause } from './PlaybackControls';
/*
import { mySliderStyles } from './Utilities';
*/

import { mySliderStyles } from 'www/Tools/Utilities';
import '../styles/SongPlaying.css';

const { log } = MakeLog('EMP:render:SongPlayback');

function CoverArt(): ReactElement {
  /*
  const songKey = useRecoilValue(currentSongKeyFunc);
  const albumKey = useRecoilValue(albumKeyForSongKeyFuncFam(songKey));
  const picurl = getAlbumImageUrl(albumKey);
  */
  // TODO: Replace this with a real image URL
  const picurl = '/images/album/key';
  return (
    <span id="song-cover-art">
      <img id="img-current-cover-art" src={picurl} alt="album cover" />
    </span>
  );
}

function MediaTimePosition(): ReactElement {
  const mediaTimePosition = useAtomValue(mediaTimePositionState);
  return (
    <Text
      id="now-playing-current-time"
      variant="tiny"
      block={true}
      nowrap={true}>
      {mediaTimePosition}
    </Text>
  );
}

function MediaTimeRemaining(): ReactElement {
  const mediaTimeRemaining = useAtomValue(mediaTimeRemainingState);
  return (
    <Text
      id="now-playing-remaining-time"
      variant="tiny"
      block={true}
      nowrap={true}>
      {mediaTimeRemaining}
    </Text>
  );
}

function MediaTimeSlider(): ReactElement {
  const songKey = useAtomValue(curSongKeyState);
  const [mediaTimePercent, setMediaTimePercent] = useAtom(
    mediaTimePercentState,
  );
  return (
    <Slider
      className="song-slider" /* Can't put an ID on a slider :( */
      value={mediaTimePercent}
      min={0}
      max={1}
      disabled={songKey.length === 0}
      step={1e-7}
      styles={mySliderStyles}
      onChange={(value: number) => setMediaTimePercent(value)}
      // onChanged={(, value: number) => log('Changed:' + value)}
      showValue={false}
    />
  );
}

function SongName(): ReactElement {
  const songKey = useAtomValue(curSongKeyState);
  const { title }: SongDescription = useAtomValue(
    songDescriptionForSongState(songKey),
  );
  return (
    <Text id="song-name" variant="tiny" block={true} nowrap={true}>
      {title}
    </Text>
  );
}

function ArtistAlbum(): ReactElement {
  const songKey = useAtomValue(curSongKeyState);
  const { artist, album }: SongDescription = useAtomValue(
    songDescriptionForSongState(songKey),
  );
  if (songKey) {
    const split = artist.length && album.length ? ': ' : '';
    return (
      <Text
        id="artist-album"
        variant="tiny"
        block={true}
        nowrap={true}>{`${artist}${split}${album}`}</Text>
    );
  } else {
    return <span id="artist-album" />;
  }
}

export const SongPlaying = forwardRef(
  (_props, audioRef: ForwardedRef<HTMLAudioElement>): ReactElement => {
    const songKey = useAtomValue(curSongKeyState);
    const isShuffle = useAtomValue(shuffleState);
    const volumeLevel = useAtomValue(volumeState);
    const playbackPercent = useAtomValue(mediaTimePercentState);
    // const setMediaTime = useSetAtom(mediaTimeState);
    // const onPlayPause = useCallback(
    //   () => onClickPlayPause(audioRef),
    //   [audioRef],
    // );
    // const onEnded = useJotaiAsyncCallback(async (get, set) => {
    //   /* TODO: What happens when the song ends? */
    //   log('Heading to the next song!!!');
    //   const songList = await get(songListState);
    //   const rep = await get(repeatState);
    //   if (rep && songList.length === 1) {
    //     // Because we rely on auto-play, if we just try to play the same song
    //     // again, it won't start playing
    //     if (isValidRefObject<HTMLAudioElement>(audioRef)) {
    //       void audioRef.current.play();
    //     }
    //   } else {
    //     await MaybePlayNext();
    //   }
    // }, []);
    // const onTimeUpdate = (ev: SyntheticEvent<HTMLMediaElement>) => {
    //   const ae = ev.currentTarget;
    //   log('time update');
    //   log(ev);

    //   if (!Number.isNaN(ae.duration)) {
    //     setMediaTime((prevTime: MediaTime) => {
    //       if (
    //         Math.trunc(ae.duration) !== Math.trunc(prevTime.duration) ||
    //         Math.trunc(ae.currentTime) !== Math.trunc(prevTime.position)
    //       ) {
    //         return { position: ae.currentTime, duration: ae.duration };
    //       } else {
    //         return prevTime;
    //       }
    //     });
    //   }
    // };
    const metadata = useAtomValue(songDescriptionForSongState(songKey));
    const picDataUri = '/images/tune/key'; // JODO: useRecoilValue(picForKeyFam(songKey));
    useEffect(() => {
      navigator.mediaSession.metadata = new MediaMetadata({
        artist: metadata.artist,
        album: metadata.album,
        title: metadata.title,
        artwork: [
          {
            src: picDataUri,
          },
        ],
      });
    }, [songKey, metadata, picDataUri]);
    useEffect(() => {
      if (isValidRefObject<HTMLAudioElement>(audioRef)) {
        audioRef.current.volume = volumeLevel * volumeLevel;
      }
    }, [audioRef, volumeLevel]);

    // TODO: Make this effect only trigger due to user intervention
    useEffect(() => {
      if (isValidRefObject<HTMLAudioElement>(audioRef)) {
        const targetTime = audioRef.current.duration * playbackPercent;
        const currentTime = audioRef.current.currentTime;

        if (
          targetTime < Number.MAX_SAFE_INTEGER &&
          targetTime >= 0 &&
          Math.abs(targetTime - currentTime) > 1.5
        ) {
          audioRef.current.currentTime = targetTime;
        }
      }
    }, [audioRef, playbackPercent]);
    // const audio = (
    //   <audio
    //     // src={songKey !== '' ? '/tune/' + songKey : ''}
    //     src={'/tune/song.m4a'}
    //     ref={audioRef}
    //     autoPlay={true}
    //     onPlay={onPlayPause}
    //     onPause={onPlayPause}
    //     onEnded={onEnded}
    //     onTimeUpdate={onTimeUpdate}
    //     muted={isMuted}
    //     controls={true}
    //   />
    // );
    const showDetail = useJotaiAsyncCallback(
      async (
        get,
        set,
        event: React.MouseEvent<HTMLSpanElement, MouseEvent>,
      ) => {
        if (songKey !== '') {
          const songs = await get(allSongsState);
          const song = songs.get(songKey);
          if (song) {
            SongDetailClick(song, event.shiftKey);
          }
        }
      },
      [songKey],
    );
    const flipDisplay = useJotaiAsyncCallback(
      async (get, set) =>
        await set(
          playOrderDisplayingState,
          !(await get(playOrderDisplayingState)),
        ),
      [],
    );
    return (
      <span id="song-container" onAuxClick={showDetail}>
        <CoverArt />
        <SongName />
        <ArtistAlbum />
        <MediaTimePosition />
        <MediaTimeSlider />
        <MediaTimeRemaining />
        <ListIcon
          id="showPlayOrder"
          onClick={flipDisplay}
          style={{
            width: '12px',
            display: isShuffle ? 'block' : 'none',
            cursor: 'pointer',
          }}
        />
      </span>
    );
  },
);
