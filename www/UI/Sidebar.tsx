import { FontIcon, SearchBox, Text } from '@fluentui/react';
import { hasStrField, isObjectNonNull } from '@freik/typechk';
import { useAtom } from 'jotai';

import { SetSearch } from 'www/Globals';
import { GetHelperText } from 'www/WebHelpers';
import { curViewState, searchTermState } from 'www/State/SimpleSavedState';
import { ReactElement, useCallback } from 'react';
import { CurrentView, Keys, StrId } from 'www/Shared/CommonTypes';

import '../styles/Sidebar.css';
import { st } from 'www/Constants';

type ViewEntry = {
  name: CurrentView;
  title: StrId;
  accelerator: Keys;
};

const mkEntry = (name: CurrentView, title: StrId, accelerator: Keys) => ({
  name,
  title,
  accelerator,
});

const views: (ViewEntry | null)[] = [
  // mkEntry('recent', 'Recently Added', ),
  mkEntry(CurrentView.now_playing, StrId.ViewNowPlaying, Keys.NowPlaying),
  null,
  mkEntry(CurrentView.albums, StrId.ViewAlbums, Keys.Albums),
  mkEntry(CurrentView.artists, StrId.ViewArtists, Keys.Artists),
  mkEntry(CurrentView.songs, StrId.ViewSongs, Keys.Songs),
  mkEntry(CurrentView.playlists, StrId.ViewPlaylists, Keys.Playlists),
  null,
  mkEntry(CurrentView.tools, StrId.ViewTools, Keys.Tools),
  mkEntry(CurrentView.settings, StrId.ViewSettings, Keys.Settings),
];

function getEntry(
  curView: CurrentView,
  setCurView: (newView: CurrentView) => Promise<void> | void,
  view: ViewEntry | null,
  index: number,
) {
  if (!view) {
    return <hr key={index} />;
  }
  const extra = curView === view.name ? ' sidebar-selected' : '';
  return (
    <div
      key={index}
      className={`sidebar-container${extra}`}
      onClick={() => void setCurView(view.name)}
      title={GetHelperText(view.accelerator)}
    >
      <span className="sidebar-icon" id={st(view.title).replace(/ /g, '-')}>
        &nbsp;
      </span>
      <Text variant="mediumPlus" className={`sidebar-text${extra}`}>
        {st(view.title)}
      </Text>
    </div>
  );
}

// This is used to prevent responding to global keypresses when the input box
// is active
export function isSearchBox(target: EventTarget | null): boolean {
  return (
    isObjectNonNull(target) &&
    hasStrField(target, 'type') &&
    hasStrField(target, 'tagName') &&
    hasStrField(target, 'placeholder') &&
    target.type === 'text' &&
    target.tagName === 'INPUT' &&
    target.placeholder === 'Search'
  );
}

export function Sidebar(): ReactElement {
  const [curView, setCurView] = useAtom(curViewState);
  const [searchTerm, setSearchTerm] = useAtom(searchTermState);
  const onSearch = useCallback(
    (newValue: string) => {
      void setCurView(CurrentView.search);
      setSearchTerm(newValue);
    },
    [setSearchTerm],
  );
  const onFocus = () => void setCurView(CurrentView.search);
  return (
    <div id="sidebar">
      <SearchBox
        placeholder="Search"
        onSearch={onSearch}
        onFocus={onFocus}
        onChange={(e, nv) => nv && onSearch(nv)}
        componentRef={(ref) => SetSearch(ref)}
        title={GetHelperText(Keys.Find)}
      />
      <div style={{ height: 8 }} />
      {views.map((ve, index) => getEntry(curView, setCurView, ve, index))}
    </div>
  );
}
