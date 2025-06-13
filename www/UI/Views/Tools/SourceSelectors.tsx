import { ReactElement } from 'react';
import { ComboBox, IComboBox, IComboBoxOption } from '@fluentui/react';
import { AlbumKey, Artist, ArtistKey, PlaylistName } from '@freik/media-core';
import { isDefined, isString } from '@freik/typechk';
import { useAtom, useAtomValue } from 'jotai';

import { useJotaiCallback } from '../../../Jotai/Helpers';
import { WritableAtomType } from '../../../Jotai/Hooks';
import { playlistNamesState } from '../../../Jotai/PlaylistControl';
import { AlbumDescriptionWithKey } from '../../../MusicLibrarySchema';
import {
  albumByKeyFuncFam,
  allAlbumsDataFunc,
  artistByKeyFuncFam,
  filteredArtistsFunc,
} from '../../../Recoil/ReadOnly';

export function PlaylistSelector({
  value,
}: {
  value: WritableAtomType<PlaylistName>;
}): ReactElement {
  const get = useAtomValue(value);
  const playlists = useAtomValue(playlistNamesState);
  const theList: IComboBoxOption[] = [...playlists]
    .sort()
    .map((name) => ({ key: name, text: name }));
  const onChange = useJotaiCallback(
    (
      get,
      set,
      event: React.FormEvent<IComboBox>,
      option?: IComboBoxOption,
      index?: number,
      val?: string,
    ): void => {
      if (isDefined(option) && isString(val) && playlists.includes(val)) {
        set(value, val);
      }
    },
    [value],
  );
  return (
    <ComboBox
      label="Which Playlist?"
      autoComplete="on"
      options={theList}
      selectedKey={get}
      onChange={onChange}
    />
  );
}

export function ArtistSelector({
  value,
}: {
  value: WritableAtomType<ArtistKey>;
}): ReactElement {
  const [selArtistKey, setSelArtistKey] = useAtom(value);
  const artists = useRecoilValue(filteredArtistsFunc);
  const theList: IComboBoxOption[] = artists.map((r: Artist) => ({
    key: r.key,
    text: r.name,
  }));
  const onChange = useMyTransaction(
    ({ get }) =>
      (
        event: React.FormEvent<IComboBox>,
        option?: IComboBoxOption,
        index?: number,
        newValue?: string,
      ) => {
        if (isDefined(option) && isString(option.key) && isString(newValue)) {
          try {
            const art = get(artistByKeyFuncFam(option.key));
            if (art.key === option.key) {
              setSelArtistKey(art.key);
            }
          } catch {
            /* */
          }
        }
      },
  );
  return (
    <ComboBox
      label="Which Artist?"
      autoComplete="on"
      options={theList}
      selectedKey={selArtistKey}
      onChange={onChange}
    />
  );
}

export function AlbumSelector({
  value,
}: {
  value: WritableAtomType<AlbumKey>;
}): ReactElement {
  const [selAlbumKey, setSelArtistKey] = useAtom(value);
  const albums = useRecoilValue(allAlbumsDataFunc);
  const theList: IComboBoxOption[] = albums.map(
    (r: AlbumDescriptionWithKey) => ({
      key: r.key,
      text: `${r.artist}: ${r.album} ${
        r.year ? '(' + r.year.toString() + ')' : ''
      }`,
    }),
  );
  const onChange = useMyTransaction(
    ({ get }) =>
      (
        event: React.FormEvent<IComboBox>,
        option?: IComboBoxOption,
        index?: number,
        newValue?: string,
      ) => {
        if (isDefined(option) && isString(option.key) && isString(newValue)) {
          try {
            const alb = get(albumByKeyFuncFam(option.key));
            if (alb.key === option.key) {
              setSelArtistKey(alb.key);
            }
          } catch {
            /* */
          }
        }
      },
  );
  return (
    <ComboBox
      label="Which Album?"
      autoComplete="on"
      options={theList}
      selectedKey={selAlbumKey}
      onChange={onChange}
    />
  );
}
