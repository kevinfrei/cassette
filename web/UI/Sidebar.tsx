import { ReactElement, useCallback } from 'react';
import { useAtom } from 'jotai';

import { SearchBox, Text } from '@fluentui/react';
import { Button } from '@fluentui/react-components';
import {
  ChevronLeft20Regular,
  ChevronRight20Regular,
} from '@fluentui/react-icons';
import { hasStrField, isObjectNonNull } from '@freik/typechk';

import { st } from '../Constants';
import { SetSearch } from '../Globals';
import { CurrentView, Keys, StrId } from '../Shared/CommonTypes';
import { curViewState } from '../State/CurrentView';
import { searchTermState } from '../State/SongPlayback';
import { GetHelperText } from '../Utils';

import '../styles/Sidebar.css';

type ViewEntry = {
  name: CurrentView;
  title: StrId;
  accelerator: Keys;
  file?: string;
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
  collapsed: boolean,
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
      title={GetHelperText(view.accelerator)}>
      <span className="sidebar-icon" id={st(view.title).replace(/ /g, '-')}>
        &nbsp;
      </span>
      <Text variant="mediumPlus" className={`sidebar-text${extra}`}>
        {collapsed ? '' : st(view.title)}
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

export function Sidebar({
  collapsed,
  collapseToggle,
}: {
  collapsed: boolean;
  collapseToggle: () => void;
}): ReactElement {
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
      <div id="sidebar-top">
        {!collapsed && (
          <>
            <SearchBox
              placeholder="Search"
              onSearch={onSearch}
              onFocus={onFocus}
              onChange={(e, nv) => nv && onSearch(nv)}
              componentRef={(ref) => SetSearch(ref)}
              title={GetHelperText(Keys.Find)}
            />
            <div style={{ height: 8 }} />
          </>
        )}
        {views.map((ve, index) =>
          getEntry(collapsed, curView, setCurView, ve, index),
        )}
      </div>
      <div id="sidebar-bottom">
        <Button
          id="sidebar-collapse"
          onClick={collapseToggle}
          appearance="subtle"
          icon={
            collapsed ? <ChevronRight20Regular /> : <ChevronLeft20Regular />
          }
          size="small"
          style={{ float: 'right' }}
        />
      </div>
    </div>
  );
}
