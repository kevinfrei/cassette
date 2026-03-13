import { chkCurrentView, CurrentView, StorageId } from '../Shared/CommonTypes';
import { atomWithMainStorage } from './Storage';

// The currently selected item from the left bar
// artist, album, search, tools, settings, etc...
export const curViewState = atomWithMainStorage<CurrentView>(
  StorageId.CurrentView,
  CurrentView.settings,
  chkCurrentView,
);
