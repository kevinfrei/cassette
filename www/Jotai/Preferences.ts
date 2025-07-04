// Only show artists in the list who appear on full albums

import { isBoolean, isNumber } from '@freik/typechk';
import { StorageId } from 'www/Shared/CommonTypes';
import { atomWithMainStorage } from './Storage';

export const showArtistsWithFullAlbumsState = atomWithMainStorage(
  StorageId.FullAlbumsOnly,
  false,
  isBoolean,
);

// The minimum # of songs an artist needs to show up in the artist list
export const minSongCountForArtistListState = atomWithMainStorage(
  StorageId.MinSongCount,
  1,
  isNumber,
);
