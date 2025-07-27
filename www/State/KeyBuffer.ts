import { Atom, atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import { CurrentView } from 'www/Shared/CommonTypes';
import { curViewState } from './CurrentView';

// This is the currently 'typed' set of characters (for scrolling lists)
export const keyBufferState = atom('');

export const focusedKeysFuncFam = atomFamily<CurrentView, Atom<string>>(
  (view) =>
    atom<string>((get) =>
      get(curViewState) === view ? get(keyBufferState) : '',
    ),
);
