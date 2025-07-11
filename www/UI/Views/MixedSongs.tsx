import {
  DetailsList,
  IDetailsRowProps,
  ScrollablePane,
  ScrollbarVisibility,
  SelectionMode,
} from '@fluentui/react';
import { MakeLog } from '@freik/logger';
import { isNumber } from '@freik/typechk';
import { atom, useAtom, useAtomValue } from 'jotai';
import { ReactElement, useCallback } from 'react';
import { allAlbumsState } from 'www/Jotai/Albums';
import { allArtistsState } from 'www/Jotai/Artists';
import { getAll, useJotaiCallback } from 'www/Jotai/Helpers';
import {
  isSongHatedFam,
  isSongLikedFam,
  songListLikeNumberFromStringFam,
} from 'www/Jotai/LikesAndHates';
import { allSongsState } from 'www/Jotai/Songs';
import { Song, SongKey } from 'www/Shared/CommonTypes';
import {
  AlbumForSongRender,
  ArtistsForSongRender,
  YearForSongRender,
} from 'www/Tools/SimpleTags';
import {
  altRowRenderer,
  MakeColumns,
  StickyRenderDetailsHeader,
} from 'www/Tools/SongList';
import { SongListMenuData } from 'www/Tools/SongMenus';
import { MakeSortKey, SortSongList } from 'www/Tools/Sorting';
import { AddSongs } from '../../Jotai/API';
import { ignoreArticlesState } from '../../Jotai/SimpleSettings';
import { LikeOrHate } from '../Liker';
import './styles/MixedSongs.css';

const { wrn } = MakeLog('EMP:render:MixedSongs');

const sortOrderState = atom(MakeSortKey([''], ['nrlyt']));
const sortedSongsState = atom(async (get) => {
  const [allSongs, allAlbums, allArtists, ignoreArticles, sortOrder] =
    await getAll(
      get,
      allSongsState,
      allAlbumsState,
      allArtistsState,
      ignoreArticlesState,
      sortOrderState,
    );
  return SortSongList(
    [...allSongs.values()],
    allAlbums,
    allArtists,
    ignoreArticles,
    sortOrder,
  );
});
const songContextState = atom<SongListMenuData>({
  data: '',
  spot: { left: 0, top: 0 },
});

export function MixedSongsList(): ReactElement {
  const sortedItems = useAtomValue(sortedSongsState);
  const [sortOrder, setSortOrder] = useAtom(sortOrderState);
  const onAddSongClick = useCallback(
    (item: Song) => AddSongs([item.key]).catch(wrn),
    [],
  );
  const [songContext, setSongContext] = useAtom(songContextState);
  const onRightClick = (item?: Song, index?: number, ev?: Event) => {
    const event = ev as any as MouseEvent;
    if (ev && item) {
      setSongContext({
        data: item.key,
        spot: { left: event.clientX + 14, top: event.clientY },
      });
    }
  };
  const columns = MakeColumns(
    [
      ['n', 'track', '#', 30, 30],
      ['r', 'artistIds', 'Artist(s)', 150, 450, ArtistsForSongRender],
      ['l', 'albumId', 'Album', 150, 450, AlbumForSongRender],
      ['y', 'albumId', 'Year', 45, 45, YearForSongRender],
      ['t', 'title', 'Title', 150],
      ['', '', '👎/👍', 35, 35, LikeOrHate],
    ],
    () => sortOrder,
    setSortOrder,
  );
  return (
    <div className="songView" data-is-scrollable="true">
      <ScrollablePane scrollbarVisibility={ScrollbarVisibility.auto}>
        <DetailsList
          items={sortedItems}
          columns={columns}
          compact={true}
          selectionMode={SelectionMode.none}
          onRenderRow={altRowRenderer()}
          onRenderDetailsHeader={StickyRenderDetailsHeader}
          onItemContextMenu={onRightClick}
          onItemInvoked={onAddSongClick}
        />
        <SongListMenu
          context={songContext}
          onClearContext={() =>
            setSongContext({ data: '', spot: { left: 0, top: 0 } })
          }
          onGetSongList={(_xact, data) => (data.length > 0 ? [data] : [])}
        />
      </ScrollablePane>
    </div>
  );
}

export function SimpleSongsList({
  forSongs,
  bold,
  keyprefix,
}: {
  forSongs: SongKey[];
  bold?: (props: IDetailsRowProps) => boolean;
  keyprefix?: string;
}): ReactElement {
  const pfx = keyprefix || 'ssl';
  const songList = useAtomValue(dataForSongListFuncFam(forSongs));
  if (!songList) {
    return <></>;
  }
  const rl = songList
    .map((val) => val.artist.length)
    .reduce((pv, cv) => Math.max(pv, cv), 1);
  const ll = songList
    .map((val) => val.album.length)
    .reduce((pv, cv) => Math.max(pv, cv), 1);
  const nl = songList
    .map((val) => val.track.toString().length)
    .reduce((pv, cv) => Math.max(pv, cv), 1);
  const tl = songList
    .map((val) => val.title.length)
    .reduce((pv, cv) => Math.max(pv, cv), 1);
  const tot = rl + ll + nl + tl;
  return (
    <div>
      <DetailsList
        items={songList}
        columns={[
          {
            key: 'r',
            fieldName: 'artist',
            name: 'Artist',
            minWidth: (100 * rl) / tot,
            maxWidth: (400 * rl) / tot,
            isResizable: true,
          },
          {
            key: 'l',
            fieldName: 'album',
            name: 'Album',
            minWidth: (100 * ll) / tot,
            maxWidth: (400 * ll) / tot,
            isResizable: true,
          },
          {
            key: 'n',
            fieldName: 'track',
            name: '#',
            minWidth: (50 * nl) / tot,
            maxWidth: (50 * nl) / tot,
            isResizable: true,
          },
          {
            key: 't',
            fieldName: 'title',
            name: 'Title',
            minWidth: (100 * tl) / tot,
            maxWidth: (400 * tl) / tot,
            isResizable: true,
          },
        ]}
        compact={true}
        selectionMode={SelectionMode.none}
        onRenderRow={altRowRenderer(bold ? bold : () => false)}
        getKey={(item: any, index?: number) =>
          `${pfx}${isNumber(index) ? forSongs[index] : 'ERROR!'}`
        }
      />
    </div>
  );
}
