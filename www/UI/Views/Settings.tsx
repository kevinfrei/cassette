import {
  DefaultButton,
  Dropdown,
  Icon,
  IconButton,
  IDropdownOption,
  Label,
  SpinButton,
  Text,
  TextField,
  TooltipHost,
} from '@fluentui/react';
import { Expandable, StateToggle } from '@freik/fluentui-tools';
import { isDefined } from '@freik/typechk';
import { useAtom, useAtomValue } from 'jotai';
import React, { ReactElement, useCallback, useState } from 'react';
import { st } from 'www/Constants';
import {
  IgnoreItemType,
  IpcCall,
  Keys,
  SocketMsg,
  StrId,
} from 'www/Shared/CommonTypes';
import { useJotaiAsyncCallback } from 'www/State/Helpers';
import { useJotaiBoolState } from 'www/State/Hooks';
import {
  neverPlayHatesState,
  onlyPlayLikesState,
} from 'www/State/LikesAndHates';
import { rescanInProgressState } from 'www/State/Miscellany';
import {
  albumCoverNameState,
  defaultLocationState,
  downloadAlbumArtworkState,
  downloadArtistArtworkState,
  ignoreArticlesState,
  locationsState,
  minSongCountForArtistListState,
  saveAlbumArtworkWithMusicState,
  showArtistsWithFullAlbumsState,
} from 'www/State/SimpleSettings';
import { ignoreItemsState } from 'www/State/SongPlayback';
import { PostMain, SendMessage } from 'www/Tools/Ipc';
import { ShowOpenDialog } from 'www/Tools/Utilities';
import { GetHelperText } from 'www/Utils';

import { allAlbumsState } from 'www/State/Albums';
import { allArtistsState } from 'www/State/Artists';
import { allSongsState } from 'www/State/Songs';
import './styles/Settings.css';

const btnWidth: React.CSSProperties = { width: '155px', padding: 0 };

const removeFromSet = (set: string[], val: string): string[] => {
  const newSet = new Set(set);
  newSet.delete(val);
  return [...newSet];
};

async function GetDirs(): Promise<string[] | void> {
  const vals = await ShowOpenDialog({
    title: 'Select Music Directory',
    folder: true,
    multiSelections: true,
  });
  if (vals) {
    console.log('Selected directories:', vals);
  }
  return vals;
}

function MusicLocations(): ReactElement {
  const [allLocs, setAllLocs] = useAtom(locationsState);
  const [defLoc, setDefLoc] = useAtom(defaultLocationState);
  const rescanInProgress = useAtomValue(rescanInProgressState);
  const onAddLocation = useJotaiAsyncCallback(async (get, set) => {
    const locs = await GetDirs();
    if (locs) {
      await set(locationsState, [...(await get(locationsState)), ...locs]);
    }
  }, []);
  const songs = useAtomValue(allSongsState);
  const albums = useAtomValue(allAlbumsState);
  const artists = useAtomValue(allArtistsState);
  const setSaveStyle = {
    textContainer: { fontSize: 11 },
    root: { height: 22, padding: 5, minWidth: 45 },
    flexContainer: {
      flexDirection: 'row-reverse',
      justifyContent: 'center',
      alignItems: 'center',
    },
  };
  return (
    <>
      {(allLocs || []).map((elem) => (
        <span key={elem} className="music-loc">
          <IconButton
            onClick={() => void setAllLocs(removeFromSet(allLocs, elem))}
            iconProps={{ iconName: 'Delete' }}
          />
          <Label>{elem}</Label>&nbsp;
          {defLoc === elem ? (
            <Text variant="small">Default "Save" (NYI)</Text>
          ) : (
            <DefaultButton
              styles={setSaveStyle}
              iconProps={{ iconName: 'Save' }}
              onClick={() => void setDefLoc(elem)}
              text="NYI: Set as Default"
            />
          )}
        </span>
      ))}
      <div>
        <DefaultButton
          text="Add Location"
          onClick={onAddLocation}
          iconProps={{ iconName: 'Add' }}
          title={GetHelperText(Keys.AddFileLocation)}
          style={btnWidth}
        />
        &nbsp;
        <TooltipHost
          id="RescanLocationsHelp"
          content="Necessary if you moved files around since launching the app">
          <DefaultButton
            text="Rescan Locations"
            iconProps={{ iconName: 'SearchData' }}
            disabled={rescanInProgress}
            onClick={() => SendMessage(SocketMsg.ManualRescan)}
            style={btnWidth}
          />
        </TooltipHost>
        &nbsp;
        <DefaultButton
          text="Import Files (NYI)"
          /* onClick={() => {}} */
          iconProps={{ iconName: 'DownloadDocument' }}
          title={st(StrId.ImportFiles)}
          style={btnWidth}
        />
      </div>
      <Text>{`${artists.size} Artists, ${albums.size} Albums, ${songs.size} Songs`}</Text>
    </>
  );
}

const ignoreTypeNameMap = new Map<IgnoreItemType, string>([
  ['path-root', 'Root Path'],
  ['dir-name', 'Directory Name'],
  ['path-keyword', 'Keyword'],
]);

const ignoreOptions: IDropdownOption[] = [...ignoreTypeNameMap.entries()].map(
  ([key, text]) => ({ key, text }),
);

function IgnoreList(): ReactElement {
  const ignoreItems = useAtomValue(ignoreItemsState);
  const [newType, setNewType] = useState<IgnoreItemType | ''>('');
  const [newValue, setNewValue] = useState<string>('');
  return (
    <div id="ignore-list">
      {ignoreItems.map(({ type, value }, idx) => (
        <div key={idx} style={{ display: 'contents' }}>
          <span style={{ gridRow: idx + 1 }} className="ignore-type">
            {ignoreTypeNameMap.get(type) || 'ERROR!'}:
          </span>
          <span style={{ gridRow: idx + 1 }} className="ignore-value">
            <TextField readOnly value={value} />
          </span>
          <span style={{ gridRow: idx + 1 }} className="ignore-button">
            <IconButton
              onClick={() => {
                /* TODO RemoveIgnoreItem({ type, value } ); */
              }}
              iconProps={{ iconName: 'Delete' }}
            />
          </span>
        </div>
      ))}
      <span style={{ gridRow: ignoreItems.length + 1 }} className="ignore-type">
        <Dropdown
          selectedKey={newType}
          onChange={(ev: unknown, option?: IDropdownOption) => {
            if (isDefined(option) && option.key !== '') {
              setNewType(option.key as IgnoreItemType);
            }
          }}
          options={ignoreOptions}
          dropdownWidth={125}
        />
      </span>
      <span
        style={{ gridRow: ignoreItems.length + 1 }}
        className="ignore-value">
        <TextField
          value={newValue}
          onChange={(ev: unknown, value?: string) => {
            if (isDefined(value)) {
              setNewValue(value);
            }
          }}
        />
      </span>
      <span
        style={{ gridRow: ignoreItems.length + 1 }}
        className="ignore-button">
        <IconButton
          onClick={() => {
            if (newType !== '') {
              // TODO: AddIgnoreItem({ type: newType, value: newValue });
            }
            setNewType('');
            setNewValue('');
          }}
          iconProps={{ iconName: 'Add' }}
          disabled={newValue.length === 0}
        />
      </span>
    </div>
  );
}

function ArticleSorting(): ReactElement {
  const articles = useJotaiBoolState(ignoreArticlesState);
  return <StateToggle label="Ignore articles when sorting" state={articles} />;
}

function ArtistFiltering(): ReactElement {
  const onlyAlbumArtists = useJotaiBoolState(showArtistsWithFullAlbumsState);
  const [songCount, setSongCount] = useAtom(minSongCountForArtistListState);
  const onIncrement = useCallback(
    () => void setSongCount(Math.min(100, songCount + 1)),
    [minSongCountForArtistListState],
  );
  const onDecrement = useCallback(
    () => void setSongCount(Math.max(1, songCount - 1)),
    [minSongCountForArtistListState],
  );
  return (
    <>
      <StateToggle
        label="Only show artists with full albums (JODO)"
        state={onlyAlbumArtists}
      />
      <SpinButton
        label="Only show artists with at least this many songs (JODO)"
        disabled={onlyAlbumArtists[0]}
        value={songCount.toString()}
        onIncrement={onIncrement}
        onDecrement={onDecrement}
        style={{ width: '10px' }}
      />
    </>
  );
}

function LikeFiltering(): ReactElement {
  const neverPlayHates = useJotaiBoolState(neverPlayHatesState);
  const onlyPlayLikes = useJotaiBoolState(onlyPlayLikesState);
  return (
    <>
      <StateToggle
        label="Never queue up songs you dislike"
        state={neverPlayHates}
      />
      <StateToggle label="Only queue up songs you like" state={onlyPlayLikes} />
    </>
  );
}

function ArtworkSettings(): ReactElement {
  const dlAlbumArtwork = useJotaiBoolState(downloadAlbumArtworkState);
  const dlArtistArtwork = useJotaiBoolState(downloadArtistArtworkState);
  const saveAlbumArtwork = useJotaiBoolState(saveAlbumArtworkWithMusicState);
  const [coverArtName, setCoverArtName] = useAtom(albumCoverNameState);
  return (
    <>
      <StateToggle label="Download Album Artwork" state={dlAlbumArtwork} />
      <div className="artwork-settings">
        <StateToggle
          disabled={!dlAlbumArtwork[0]}
          label="Try to save Album Artwork with audio files:"
          state={saveAlbumArtwork}
        />
        &nbsp;
        <TextField
          disabled={!saveAlbumArtwork[0] || !dlAlbumArtwork[0]}
          description="Filename to save the artwork as"
          value={coverArtName}
          onChange={(_ev, nv) => nv && void setCoverArtName(nv)}
        />
      </div>
      <StateToggle label="Download Artist Artwork" state={dlArtistArtwork} />
      <DefaultButton
        text="Flush Image Cache"
        style={{ ...btnWidth, gridRow: 4 }}
        onClick={() => PostMain(IpcCall.FlushImageCache)}
      />
    </>
  );
}

export function SettingsView(): ReactElement {
  return (
    <div className="settings-view">
      <Expandable separator label="Music Locations" defaultShow={true}>
        <MusicLocations />
        <Expandable
          indent={30}
          separator
          label="Ignore filters"
          defaultShow={false}>
          <IgnoreList />
        </Expandable>
      </Expandable>
      <Expandable separator label="Sorting & Filtering" defaultShow={true}>
        <LikeFiltering />
        <ArticleSorting />
        <ArtistFiltering />
      </Expandable>
      <Expandable separator label="Artwork" defaultShow={true}>
        <ArtworkSettings />
      </Expandable>
      <Expandable separator label="Metadata" defaultShow={true}>
        <>
          <DefaultButton
            text="Flush Metadata Cache"
            style={btnWidth}
            onClick={() => PostMain(IpcCall.FlushMetadataCache)}
          />
          &nbsp;
          <DefaultButton
            text="Clear Local Overrides"
            style={btnWidth}
            onClick={() => PostMain(IpcCall.ClearLocalOverrides)}
          />
        </>
      </Expandable>
    </div>
  );
}
