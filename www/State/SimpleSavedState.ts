import { isBoolean, isNumber, typecheck } from '@freik/typechk';
import { atom } from 'jotai';

import { IgnoreItem } from 'www/Constants';
import { atomWithMainStorage } from 'www/Jotai/Storage';
import { CurrentView, StorageId } from 'www/Shared/CommonTypes';
import { isCurrentView } from 'www/Shared/EnumTypeSupport';

export const mutedState = atomWithMainStorage(StorageId.Mute, false, isBoolean);
export const volumeState = atomWithMainStorage(StorageId.Volume, 0.5, isNumber); // Volume as a percentage (0-100)

export type RepeatType = 'off' | 'one' | 'all';
export const isRepeatType: typecheck<RepeatType> = (
  val: unknown,
): val is RepeatType => val === 'off' || val === 'one' || val === 'all';
export const curViewState = atomWithMainStorage(
  StorageId.CurrentView,
  CurrentView.albums,
  isCurrentView,
);
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
