import { useAtomValue } from 'jotai';
import { CSSProperties, ReactElement, Suspense, useState } from 'react';
import { CurrentView } from '../../Shared/CommonTypes';
import { curViewState } from '../../State/CurrentView';
import { GroupedAlbumList } from './Albums';
import { GroupedAristList } from './Artists';
import { MixedSongsList } from './MixedSongs';
import { NowPlayingView } from './NowPlaying';
import { PlaybackOrder } from './PlaybackOrder';
import { PlaylistView } from './Playlists';
import { SearchResultsView } from './SearchResults';
import { SettingsView } from './Settings';
import { ToolsView } from './Tools';

import './styles/Selector.css';

function ignore(view: CurrentView): boolean {
  return view === CurrentView.settings || view === CurrentView.tools;
}

function include(
  val: CurrentView,
  theSet: Set<CurrentView>,
  which: CurrentView,
): boolean {
  return theSet.has(val) || which === val;
}

export function ViewSelector(): ReactElement {
  const which = useAtomValue(curViewState);
  const [rendered, setRendered] = useState(new Set<CurrentView>([which]));
  // Let's see if I can speed this up a bit by not trying to render everything
  // the first time
  const vis = (v: CurrentView): CSSProperties =>
    which === v ? {} : { visibility: 'hidden' };
  const contents: [CurrentView, ReactElement][] = [];
  if (!rendered.has(which) && !ignore(which)) {
    const newrendered = new Set<CurrentView>([which, ...rendered]);
    setRendered(newrendered);
    // We still need to do a full render, because otherwise elements get
    // deleted and recreated, and that's probably bad, right?
  }
  if (include(CurrentView.albums, rendered, which)) {
    contents.push([CurrentView.albums, <GroupedAlbumList />]);
  }
  if (include(CurrentView.artists, rendered, which)) {
    contents.push([CurrentView.artists, <GroupedAristList />]);
  }
  if (include(CurrentView.songs, rendered, which)) {
    contents.push([CurrentView.songs, <MixedSongsList />]);
  }
  if (include(CurrentView.playlists, rendered, which)) {
    contents.push([CurrentView.playlists, <PlaylistView />]);
  }
  if (include(CurrentView.now_playing, rendered, which)) {
    contents.push([CurrentView.now_playing, <NowPlayingView />]);
  }
  if (include(CurrentView.settings, rendered, which)) {
    contents.push([CurrentView.settings, <SettingsView />]);
  }
  if (include(CurrentView.search, rendered, which)) {
    contents.push([CurrentView.search, <SearchResultsView />]);
  }
  if (include(CurrentView.tools, rendered, which)) {
    contents.push([CurrentView.tools, <ToolsView />]);
  }
  return (
    <>
      <PlaybackOrder />
      {contents.map(([view, elem]) => (
        <div key={view} className="current-view" style={vis(view)}>
          {elem}
        </div>
      ))}
    </>
  );
}
