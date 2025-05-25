import { FontIcon, SearchBox, Text } from '@fluentui/react';
import { hasStrField, isObjectNonNull } from '@freik/typechk';
import { useAtom } from 'jotai';
import {
  CurrentView,
  CurrentViewEnum,
  Keys,
  KeysEnum,
  StrId,
  StrIdEnum,
} from 'www/Constants';
// import { GetHelperText } from './MenuHelpers';
// import { SetSearch } from '../MyWindow';
// import { Notifier } from './Notifier';

import '../styles/Sidebar.css';

type ViewEntry = {
  name: CurrentViewEnum;
  title: StrIdEnum;
  accelerator: KeysEnum;
};
const mkEntry = (
  name: CurrentViewEnum,
  title: StrIdEnum,
  accelerator: KeysEnum,
) => ({
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
  curView: CurrentViewEnum,
  setCurView: (newView: CurrentViewEnum) => Promise<void>,
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
      title="title" /* {GetHelperText(view.accelerator)} */
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

export function Sidebar(): JSX.Element {
  const [curView, setCurView] = useAtom(curViewFunc);
  /* const onSearch = useRecoilCallback(({ set }) => (newValue: string) => {
    void setCurView(CurrentView.search);
    set(searchTermState, newValue);
  });
  */
  const onFocus = () => void setCurView(CurrentView.search);
  return (
    <div id="sidebar">
      <br />
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
      <br />
      <Notifier />
    </div>
  );
}
