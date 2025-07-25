// This is for getting at "global" stuff from the window object
import { RefObject } from '@fluentui/react';
import { MakeLog } from '@freik/logger';
import {
  hasField,
  hasFieldOf,
  isArrayOfString,
  isObjectNonNull,
  isString,
} from '@freik/typechk';
import { ForwardedRef } from 'react';
import {
  AlbumKey,
  ArtistKey,
  Keys,
  PlaylistName,
} from 'www/Shared/CommonTypes';
import { SongInfo } from './Types';

const { log } = MakeLog('EMP:render:Tools');

/*
 * Searching
 */

/**
 * Binary search a sorted set of items, finding either the value, or it's
 * closest relative
 *
 * @param  {T[]} sortedItems
 * @param  {E} searchValue
 * @param  {(v:T)=>E} selector
 * @param  {(a:E,b:E)=>number} comparator
 * @returns number
 */
export function GetIndexOf<T, E>(
  sortedItems: T[],
  searchValue: E,
  selector: (v: T) => E,
  comparator: (a: E, b: E) => number,
): number {
  // Binary search, assuming the items are sorted in either ascending or
  // descending value of the selector
  const ascending =
    comparator(
      selector(sortedItems[0]),
      selector(sortedItems[sortedItems.length - 1]),
    ) > 0;
  const before = (a: number): boolean => {
    const sort = comparator(selector(sortedItems[a]), searchValue) < 0;
    return ascending ? sort : !sort;
  };
  let lo = 0;
  let hi = sortedItems.length - 1;
  while (lo < hi) {
    const mid = Math.floor((lo + hi) / 2);
    if (before(mid)) {
      hi = mid - 1;
    } else {
      lo = mid + 1;
    }
  }
  log(lo);
  return lo;
}

/*
 * Miscellaney
 */

export function isPlaylistName(plName?: string): plName is PlaylistName {
  return isString(plName) && plName.length > 0;
}

export function RandomInt(max: number): number {
  return Math.floor(Math.random() * max);
}

export function ShuffleArray<T>(array: T[]): T[] {
  const res = [...array];
  for (let i = res.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    const t = res[i];
    res[i] = res[j];
    res[j] = t;
  }
  return res;
}

export function secondsToTime(val: number): string {
  const expr = new Date(val * 1000).toISOString();
  if (val < 600) {
    return expr.substring(15, 19);
  } else if (val < 3600) {
    return expr.substring(14, 19);
  } else if (val < 36000) {
    return expr.substring(12, 19);
  } else {
    return expr.substring(11, 19);
  }
}

export function fractionalSecondsStrToHMS(vals: string): string {
  const decimal = vals.indexOf('.');
  let suffix: string = decimal > 0 ? vals.substring(decimal) : '';
  suffix = suffix.replace(/0+$/g, '');
  suffix = suffix.length === 1 ? '' : suffix.substring(0, 3);
  const val = parseInt(vals, 10);
  return secondsToTime(val) + suffix;
}

export function divGrand(val: string): string {
  let flt = (parseFloat(val) / 1000.0).toPrecision(4);
  if (flt.indexOf('.') < 0) {
    return flt;
  }
  flt = flt.replace(/0+$/g, '');
  flt = flt.replace(/\.$/, '');
  return flt;
}

export function isValidRefObject<T>(
  ref: ForwardedRef<T>,
): ref is RefObject<T> & { current: T } {
  return isObjectNonNull(ref) && hasFieldOf(ref, 'current', isObjectNonNull);
}

export function getAlbumImageUrl(albumKey: AlbumKey) {
  return `pic://key/${albumKey}`;
}

export function getArtistImageUrl(artistKey: ArtistKey) {
  return `pic://key/${artistKey}`;
}

export function diskNumName(
  songData: SongInfo,
): [string | null, string | null] {
  const diskNo = Math.floor(songData.song.track / 100);
  if (diskNo > 0) {
    if (hasFieldOf(songData.album, 'diskNames', isArrayOfString)) {
      return [diskNo.toString(), songData.album.diskNames[diskNo - 1]];
    }
    return [diskNo.toString(), null];
  } else {
    return [null, null];
  }
}

// Hurray for IIFE's :)
const HostOs: 'mac' | 'windows' | 'linux' = (() => {
  const ua = window.navigator.userAgent;
  if (ua.indexOf('Mac') >= 0) {
    return 'mac';
  }
  if (ua.indexOf('Windows') >= 0) {
    return 'windows';
  }
  return 'linux';
})();

const accPrefix = HostOs === 'mac' ? '⌘' : 'Ctrl';

export function GetHelperText(key: Keys) {
  if (key.length === 1) {
    return `${accPrefix}-${key}`;
  }
  if (key === Keys.PreviousTrack) {
    return accPrefix + '-←';
  }
  if (key === Keys.NextTrack) {
    return accPrefix + '-→';
  }
  return `${accPrefix}-${key}`;
}
