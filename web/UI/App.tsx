import {
  ReactElement,
  SyntheticEvent,
  useCallback,
  useId,
  useRef,
  useState,
} from 'react';
import { Provider, useAtomValue, useSetAtom } from 'jotai';

import { FluentProvider, webLightTheme } from '@fluentui/react-components';
import {
  Group,
  Layout,
  Panel,
  Separator,
  usePanelRef,
} from 'react-resizable-panels';

import { KeepAlive } from '../KeepAlive';
import { useJotaiAsyncCallback } from '../State/Helpers';
import {
  curSongKeyState,
  mutedState,
  repeatState,
  songListState,
} from '../State/SongPlayback';
import { getStore } from '../State/Storage';
import { MediaTime, mediaTimeState } from '../State/TimeState';
import { MusicDbListener } from '../Tools/MusicDbListener';
import { isValidRefObject } from '../Utils';
import { MaybePlayNext, PlaybackControls } from './PlaybackControls';
import { Sidebar } from './Sidebar';
import { SongPlaying } from './SongPlaying';
import { ViewSelector } from './Views/Selector';
import { VolumeControl } from './VolumeControl';

import '../styles/App.css';

type AudioElementProps = { audioRef: React.Ref<HTMLAudioElement> };

function AudioElement({ audioRef }: AudioElementProps): ReactElement {
  const songKey = useAtomValue(curSongKeyState);
  const setMediaTime = useSetAtom(mediaTimeState);
  const isMuted = useAtomValue(mutedState);
  const onEnded = useJotaiAsyncCallback(async (get, set) => {
    /* TODO: What happens when the song ends? */
    // log('Heading to the next song!!!');
    const songList = await get(songListState);
    const rep = await get(repeatState);
    if (rep && songList.length === 1) {
      // Because we rely on auto-play, if we just try to play the same song
      // again, it won't start playing
      if (isValidRefObject<HTMLAudioElement>(audioRef)) {
        void audioRef.current.play();
      }
    } else {
      await MaybePlayNext();
    }
  }, []);

  const onTimeUpdate = useCallback(
    (ev: SyntheticEvent<HTMLMediaElement>) => {
      const ae = ev.currentTarget;
      if (!Number.isNaN(ae.duration)) {
        setMediaTime((prevTime: MediaTime) => {
          if (
            Math.trunc(ae.duration) !== Math.trunc(prevTime.duration) ||
            Math.trunc(ae.currentTime) !== Math.trunc(prevTime.position)
          ) {
            return { position: ae.currentTime, duration: ae.duration };
          } else {
            return prevTime;
          }
        });
      }
    },
    [setMediaTime],
  );
  return (
    <audio
      src={songKey !== '' ? '/tune/' + songKey : ''}
      ref={audioRef}
      onEnded={onEnded}
      onTimeUpdate={onTimeUpdate}
      muted={isMuted}
      controls={false}
    />
  );
}

function TheActualApp(): ReactElement {
  const panelRef = usePanelRef();
  const audioRef = useRef<HTMLAudioElement>(null);
  const [collapsed, setCollapsed] = useState(false);
  const onLayoutChange = useCallback(() => {
    if (panelRef && panelRef.current) {
      const isCol = panelRef.current.isCollapsed();
      if (isCol !== collapsed) {
        setCollapsed(!collapsed);
      }
    }
  }, [collapsed, setCollapsed, panelRef]);
  const onToggle = useCallback(() => {
    if (panelRef && panelRef.current) {
      if (panelRef.current.isCollapsed()) {
        panelRef.current.expand();
      } else {
        panelRef.current.collapse();
      }
    }
  }, [panelRef]);
  return (
    <>
      <AudioElement audioRef={audioRef} />
      <div id="app">
        <PlaybackControls audioRef={audioRef} />
        <SongPlaying audioRef={audioRef} />
        <VolumeControl audioRef={audioRef} />
        <div className="SongDetailPanel" />
        <Group
          id="view-and-content"
          orientation="horizontal"
          onLayoutChange={onLayoutChange}>
          <Panel
            panelRef={panelRef}
            collapsible
            collapsedSize={40}
            minSize={140}
            maxSize={140}
            defaultSize={140}
            style={{ display: 'flex' }}>
            <Sidebar collapsed={collapsed} collapseToggle={onToggle} />
          </Panel>
          <Separator id="view-separator" />
          <Panel style={{ display: 'flex' }}>
            <ViewSelector />
          </Panel>
        </Group>
      </div>
    </>
  );
}

export function App(): ReactElement {
  const store = getStore();
  return (
    <Provider store={store}>
      <FluentProvider theme={webLightTheme}>
        <MusicDbListener />
        <KeepAlive />
        <TheActualApp />
      </FluentProvider>
    </Provider>
  );
}
