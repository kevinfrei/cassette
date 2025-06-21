import { hasFieldType, isObjectNonNull, typecheck } from '@freik/typechk';
import { ForwardedRef, MutableRefObject } from 'react';
import { Keys } from './Shared/CommonTypes';

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

export function isMutableRefObject<T>(
  ref: ForwardedRef<T>,
): ref is MutableRefObject<T> {
  return isObjectNonNull(ref) && hasFieldType(ref, 'current', isObjectNonNull);
}
