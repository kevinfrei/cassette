import { Atom, atom } from 'jotai';
import { atomFamily } from 'jotai-family';
import { CurrentView } from '../Shared/CommonTypes';
import { curViewState } from './CurrentView';

// This is the currently 'typed' set of characters (for scrolling lists)
export const keyBufferState = atom('');

export const focusedKeysFuncFam = atomFamily<CurrentView, Atom<string>>(
  (view) =>
    atom<string>((get) =>
      get(curViewState) === view ? get(keyBufferState) : '',
    ),
);
