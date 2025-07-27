import { isSongKey } from '@freik/media-core';
import { Catch } from '@freik/react-tools';
import {
  chkAnyOf,
  chkArrayOf,
  isBoolean,
  isNumber,
  isString,
  typecheck,
} from '@freik/typechk';
import { atom } from 'jotai';
import { atomWithReset } from 'jotai/utils';
import { IgnoreItem } from 'www/Constants';
import { Song, SongKey, StorageId } from 'www/Shared/CommonTypes';
import { MakeSortKey } from 'www/Tools/Sorting';
import { songByKey } from './Songs';
import { atomWithMainStorage } from './Storage';

export const isMiniplayerState = atom(false);

// This is the sort for the current playlist
export const nowPlayingSortState = atomWithReset(MakeSortKey([''], ['lyrnt']));

// The currently selected song(s) to display details for
export const songDetailState = atomWithReset<Set<SongKey>>(new Set<SongKey>());

// The # of recently added songs to show for a few seconds
export const recentlyQueuedState = atomWithReset<number>(0);

// Is the 'display message' showing
export const displayMessageState = atom<boolean>(false);

// Are we displaying the play order?
export const playOrderDisplayingState = atom<boolean>(false);

export const mutedState = atomWithMainStorage(StorageId.Mute, false, isBoolean);
export const volumeState = atomWithMainStorage(StorageId.Volume, 0.5, isNumber); // Volume as a percentage (0-100)

export type RepeatType = 'off' | 'one' | 'all';
export const isRepeatType: typecheck<RepeatType> = (
  val: unknown,
): val is RepeatType => val === 'off' || val === 'one' || val === 'all';

export const shuffleState = atomWithMainStorage(
  StorageId.Shuffle,
  false,
  isBoolean,
);
export const repeatState = atomWithMainStorage(
  StorageId.Repeat,
  'off',
  isRepeatType,
);
export function NextRepeat(current: RepeatType): RepeatType {
  switch (current) {
    case 'off':
      return 'one';
    case 'one':
      return 'all';
    case 'all':
    default:
      return 'off'; // Fallback to 'off' if something goes wrong
  }
}

export const searchTermState = atom<string>('');
export const ignoreItemsState = atom<IgnoreItem[]>([]);
export const minSongCountForArtistListState = atom<number>(5);

// The position in the active playlist of the current song
// For 'ordered' playback, it's the index in the songList
// otherwise it's an index into the songPlaybackOrderState
export const currentIndexState = atomWithMainStorage(
  StorageId.CurrentIndex,
  -1,
  isNumber,
);

// The order of the playlist to play
export const songPlaybackOrderState = atomWithMainStorage<'ordered' | number[]>(
  StorageId.PlaybackOrder,
  'ordered',
  chkAnyOf((a): a is 'ordered' => a === 'ordered', chkArrayOf(isNumber)),
);

// The name of the active playlist
// the emptry string means the playlist isn't saved as, or based on, anything
export const activePlaylistState = atomWithMainStorage(
  StorageId.NowPlaying,
  '',
  isString,
);

// The currently active playlist
export const songListState = atomWithMainStorage<SongKey[]>(
  StorageId.CurrentSongList,
  [],
  chkArrayOf(isSongKey),
);

// This is the current index into the nowPlayist (sorted, never shuffled) list
export const currentSongIndexState = atom(
  async (get) => {
    const curIdx = await get(currentIndexState);
    if (curIdx < 0) {
      return curIdx;
    }
    const order = await get(songPlaybackOrderState);
    if (order === 'ordered') {
      return curIdx;
    }
    return order[curIdx];
  },
  async (get, set, newVal: number) => {
    const order = await get(songPlaybackOrderState);
    if (order === 'ordered') {
      set(currentIndexState, newVal);
    } else if (isNumber(newVal)) {
      set(currentIndexState, order.indexOf(newVal));
    } else {
      set(currentIndexState, -1);
    }
  },
);

// Selector to get the current song key based on the rest of this nonsense
export const currentSongKeyState = atom(async (get) => {
  const curIndex = await get(currentIndexState);
  if (curIndex >= 0) {
    const songList = await get(songListState);
    const playbackOrder = await get(songPlaybackOrderState);
    if (curIndex >= 0 && curIndex < songList.length) {
      return playbackOrder === 'ordered'
        ? songList[curIndex]
        : songList[playbackOrder[curIndex]];
    }
  }
  return '';
});

// Is there a 'next song' to play?
export const hasNextSongState = atom(async (get) => {
  const songList = await get(songListState);
  if (songList.length === 0) {
    return false;
  }
  const curIndex = await get(currentIndexState);
  if (curIndex >= 0 && curIndex < songList.length - 1) {
    return true;
  }
  return get(repeatState);
});

// Is there a 'previous song' to play?
export const hasPrevSongState = atom(async (get) => {
  const songList = await get(songListState);
  if (songList.length === 0) {
    return false;
  }
  const curIndex = await get(currentIndexState);
  return curIndex > 0 || (await get(repeatState));
});

// Do we have any songs at all?
export const hasAnySongsState = atom(
  async (get) => (await get(songListState)).length > 0,
);

export const curSongsState = atom(async (get): Promise<Song[]> => {
  const curList = await get(songListState);
  const songList: Song[] = [];
  for (const sk of curList) {
    try {
      const s = await get(songByKey(sk));
      songList.push(s);
    } catch (e) {
      Catch(e, `Error for SongKey '${sk}'`);
    }
  }
  return songList; // curList.map((sk: SongKey) => get(getSongByKeyState(sk)));
});
