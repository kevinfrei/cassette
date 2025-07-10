import { hasFieldType, isArrayOfString } from '@freik/typechk';
import { useAtomValue } from 'jotai';
import { ReactElement } from 'react';
import { AlbumKey, ArtistKey, Song } from 'www/Shared/CommonTypes';

import { albumByKey } from '../Jotai/Albums';
import { artistStringStateFamily } from '../Jotai/Artists';

export function ArtistNameFromArtistIds({
  artistIds,
}: {
  artistIds: ArtistKey[];
}): ReactElement {
  return <>{useAtomValue(artistStringStateFamily(artistIds))}</>;
}

function AlbumNameForSong({ song }: { song: Song }): ReactElement {
  const album = useAtomValue(albumByKey(song.albumId));
  const diskNum = Math.floor(song.track / 100);
  if (
    diskNum > 0 &&
    hasFieldType(album, 'diskNames', isArrayOfString) &&
    album.diskNames.length >= diskNum &&
    album.diskNames[diskNum - 1].length > 0
  ) {
    return (
      <>
        {album.title}: {album.diskNames[diskNum - 1]}
      </>
    );
  }
  return <>{album.title}</>;
}

function YearForAlbum({ albumId }: { albumId: AlbumKey }): ReactElement {
  const album = useAtomValue(albumByKey(albumId));
  return <>{album.year !== 0 ? album.year : ''}</>;
}

export function ArtistsForSongRender(theSong: Song): ReactElement {
  return <ArtistNameFromArtistIds artistIds={theSong.artistIds} />;
}

export function AlbumForSongRender(song: Song): ReactElement {
  return <AlbumNameForSong song={song} />;
}

export function YearForSongRender(song: Song): ReactElement {
  return <YearForAlbum albumId={song.albumId} />;
}
