// The currently selected item from the left bar

import { atom } from 'jotai';
import { chkCurrentView, CurrentView, StorageId } from 'www/Shared/CommonTypes';
import { isMiniplayerState } from './SongPlayback';
import { atomWithMainStorage } from './Storage';

// The currently selected item from the left bar
// artist, album, search, tools, settings, etc...
const curViewBackerState = atomWithMainStorage<CurrentView>(
  StorageId.CurrentView,
  CurrentView.settings,
  chkCurrentView,
);

// This makes the miniplayer view always select the current view
export const curViewState = atom(
  (get) =>
    get(isMiniplayerState) ? CurrentView.now_playing : get(curViewBackerState),
  (_get, set, newVal: CurrentView) => set(curViewBackerState, newVal),
);
