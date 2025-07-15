import {
  DetailsList,
  IconButton,
  IDetailsList,
  IGroup,
  Image,
  ImageFit,
  ScrollablePane,
  ScrollbarVisibility,
  SelectionMode,
  Text,
} from '@fluentui/react';
import { MakeLog } from '@freik/logger';
import { hasFieldType, isDefined, isNumber } from '@freik/typechk';
import { atom as jatom, useAtom, useAtomValue, useSetAtom } from 'jotai';
import { atomWithReset, useResetAtom } from 'jotai/utils';
import { ReactElement, useCallback, useState } from 'react';
import { AlbumKey, CurrentView, Song } from 'www/Shared/CommonTypes';
import { AddSongs, dataForAlbumByKey } from 'www/State/API';
import { useJotaiCallback } from 'www/State/Helpers';
import { MakeSetAtomFamily } from 'www/State/Hooks';
import { focusedKeysFuncFam } from 'www/State/KeyBuffer';
import { ignoreArticlesState } from 'www/State/SimpleSettings';
import {
  AlbumForSongRender,
  ArtistsForSongRender,
  YearForSongRender,
} from 'www/Tools/SimpleTags';
import {
  altRowRenderer,
  ProcessSongGroupData,
  StickyRenderDetailsHeader,
} from 'www/Tools/SongList';
import { SongListMenu, SongListMenuData } from 'www/Tools/SongMenus';
import {
  articlesCmp,
  MakeSortKey,
  noArticlesCmp,
  SortSongsFromAlbums,
} from 'www/Tools/Sorting';
import { getAlbumImageUrl, GetIndexOf } from 'www/Utils';

import { albumByKey, allAlbumsState } from 'www/State/Albums';
import { allArtistsState } from 'www/State/Artists';
import { allSongsState, songListFromKey } from 'www/State/Songs';
import './styles/Albums.css';

const { wrn } = MakeLog('EMP:render:Albums');
wrn.enabled = true;

// This is used to trigger the popup menu in the list view
const albumContextState = atomWithReset<SongListMenuData>({
  data: '',
  spot: { left: 0, top: 0 },
});

const [albumExpandedState, albumIsExpandedState] =
  MakeSetAtomFamily<AlbumKey>();

// For grouping to work properly, the sort order needs to be fully specified
// Also, the album year, VA type, and artist have to "stick" with the album name
// as a single group, or you get duplicate group IDs
const albumSortState = jatom(MakeSortKey(['l', 'n'], ['lry', 'nrt']));

type AHDProps = { group: IGroup };
function AlbumHeaderDisplay({ group }: AHDProps): ReactElement {
  const album = useAtomValue(albumByKey(group.key));
  const albumData = useAtomValue(dataForAlbumByKey(group.key));
  const picurl = getAlbumImageUrl(group.key);
  const onAddSongsClick = useCallback(() => {
    AddSongs(album.songs).catch(wrn);
  }, [album.songs]);
  const thisSetSetter = albumIsExpandedState(group.key);
  const onHeaderExpanderClick = useJotaiCallback(
    (get, set) => set(thisSetSetter, !group.isCollapsed),
    [thisSetSetter, group.isCollapsed],
  );
  const setAlbumContext = useSetAtom(albumContextState);
  const onRightClick = (event: React.MouseEvent<HTMLElement, MouseEvent>) => {
    const data = group.key;
    const spot = { left: event.clientX + 14, top: event.clientY };
    setAlbumContext({ data, spot });
  };

  return (
    <div className="album-header" onContextMenu={onRightClick}>
      <IconButton
        iconProps={{
          iconName: group.isCollapsed ? 'ChevronRight' : 'ChevronDown',
        }}
        onClick={onHeaderExpanderClick}
      />
      <div
        className="album-header-info"
        onDoubleClick={onAddSongsClick}
        style={{ padding: '2px 0px', cursor: 'pointer' }}>
        <Image
          imageFit={ImageFit.centerContain}
          height={50}
          width={50}
          src={picurl}
        />
        <Text style={{ margin: '4px' }}>
          {`${albumData.album}: ${albumData.artist} ` +
            (album.year > 0 ? `[${albumData.year}] ` : '') +
            (album.songs.length === 1
              ? '1 song'
              : `${album.songs.length} songs`)}
        </Text>
      </div>
    </div>
  );
}

export function GroupedAlbumList(): ReactElement {
  const [detailRef, setDetailRef] = useState<IDetailsList | null>(null);

  const albums = useAtomValue(allAlbumsState);
  const ignoreArticles = useAtomValue(ignoreArticlesState);
  const keyBuffer = useAtomValue(focusedKeysFuncFam(CurrentView.albums));
  const allSongs = useAtomValue(allSongsState);
  const allArtists = useAtomValue(allArtistsState);
  const newAlbumSort = useAtomValue(albumSortState);
  const [albumContext, setAlbumContext] = useAtom(albumContextState);

  const curExpandedState = useAtom(albumExpandedState);
  const [curSort, setSort] = useAtom(albumSortState);
  const resetAlbumContext = useResetAtom(albumContextState);

  const onAddSongClick = useCallback(
    (item: Song) => AddSongs([item.key]).catch(wrn),
    [],
  );
  const onRightClick = (item: Song, _index?: number, ev?: Event) => {
    if (
      isDefined(ev) &&
      hasFieldType(ev, 'clientX', isNumber) &&
      hasFieldType(ev, 'clientY', isNumber)
    ) {
      const data = item.key;
      const spot = { left: ev.clientX + 14, top: ev.clientY };
      setAlbumContext({ data, spot });
    }
  };
  const onGetSongList = useJotaiCallback(
    (get, set, data: string) => get(songListFromKey(data)),
    [],
  );
  // Get the sorted song & group lists
  const { songs: sortedSongs, groups } = SortSongsFromAlbums(
    albums.values(),
    allSongs,
    allArtists,
    ignoreArticles,
    newAlbumSort,
  );
  const [columns, groupProps] = ProcessSongGroupData(
    groups,
    curExpandedState,
    'albumId',
    [
      ['l', 'albumId', 'Album', 50, 175, AlbumForSongRender],
      ['r', 'primaryArtists', 'Artist', 50, 250, ArtistsForSongRender],
      ['y', 'albumId', 'Year', 55, 25, YearForSongRender],
      ['n', 'track', '#', 15, 25],
      ['t', 'title', 'Title', 50, 150],
    ],
    (group: IGroup) => <AlbumHeaderDisplay group={group} />,
    () => curSort,
    setSort,
  );

  // This doesn't quite work: It's sometimes off by a few rows.
  // It looks like DetailsList doesn't do the math quite right, unfortunately.
  // I should check it out on Songs to see if it's related to groups...
  if (detailRef && keyBuffer.length > 0) {
    const index = GetIndexOf(
      groups,
      keyBuffer,
      (s: IGroup) => s.name,
      ignoreArticles ? noArticlesCmp : articlesCmp,
    );
    detailRef.focusIndex(index);
  }
  return (
    <div className="songListForAlbum" data-is-scrollable="true">
      <ScrollablePane scrollbarVisibility={ScrollbarVisibility.always}>
        <DetailsList
          componentRef={(ref) => setDetailRef(ref)}
          items={sortedSongs}
          selectionMode={SelectionMode.none}
          groups={groups}
          columns={columns}
          compact
          onRenderRow={altRowRenderer()}
          onRenderDetailsHeader={StickyRenderDetailsHeader}
          onItemContextMenu={onRightClick}
          onItemInvoked={onAddSongClick}
          groupProps={groupProps}
        />
        <SongListMenu
          context={albumContext}
          onClearContext={resetAlbumContext}
          onGetSongList={onGetSongList}
        />
      </ScrollablePane>
    </div>
  );
}
