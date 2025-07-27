import {
  DefaultButton,
  Dropdown,
  IconButton,
  IDropdownOption,
  Label,
  SpinButton,
  Text,
  TextField,
  TooltipHost,
} from '@fluentui/react';
import { Expandable, StateToggle } from '@freik/fluentui-tools';
import { useBoolState } from '@freik/react-tools';
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
import {
  ignoreItemsState,
  minSongCountForArtistListState,
} from 'www/State/SongPlayback';
import { PostMain, SendMessage } from 'www/Tools/Ipc';
import { GetHelperText } from 'www/Utils';

import './styles/Settings.css';

const btnWidth: React.CSSProperties = { width: '155px', padding: 0 };
const removeFromSet = (set: string[], val: string): string[] => {
  const newSet = new Set(set);
  newSet.delete(val);
  return [...newSet];
};

async function GetDirs(): Promise<string[] | void> {
  return Promise.resolve();
  /* TODO await Util.ShowOpenDialog({ properties: ['openDirectory'] }); */
}
/*
export async function addLocation({
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  set,
}: MyTransactionInterface): Promise<boolean> {
  const locs = await GetDirs();
  if (locs) {
    // set(locationsState, (curLocs) => [...locs, ...curLocs]);
    return true;
  }
  return false;
}*/

function MusicLocations(): ReactElement {
  /*

  TODO

  const [newLoc, setNewLoc] = useAtom(locationsState);
  const [defLoc, setDefLoc] = useAtom(defaultLocationState);
  const rescanInProgress = useAtomValue(rescanInProgressState);
  const onAddLocation = useMyTransaction((xact) => () => {
    addLocation(xact).catch(Catch);
  });
  const songs = useRecoilValue(allSongsFunc);
  const albums = useRecoilValue(allAlbumsFunc);
  const artists = useRecoilValue(allArtistsFunc);
  */
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
      {/*(newLoc || []).map((elem) => (
        <span key={elem} className="music-loc">
          <IconButton
            onClick={() => void setNewLoc(removeFromSet(newLoc, elem))}
            iconProps={{ iconName: 'Delete' }}
          />
          <Label>{elem}</Label>&nbsp;
          {defLoc === elem ? (
            <Text variant="small">[Default "Save" Location (NYI)]</Text>
          ) : (
            <DefaultButton
              styles={setSaveStyle}
              iconProps={{ iconName: 'Save' }}
              onClick={() => void setDefLoc(elem)}
              text="NYI: Set as"
            />
          )}
        </span>
      ))*/}
      <div>
        <DefaultButton
          text="Add Location"
          onClick={() => {} /*onAddLocation*/}
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
            disabled={false /*rescanInProgress*/}
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
      <Text>{`1 Artists, 2 Albums, 3 Songs`}</Text>
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
  const articles = useBoolState(true);
  return <StateToggle label="Ignore articles when sorting" state={articles} />;
}

function ArtistFiltering(): ReactElement {
  const onlyAlbumArtists = useBoolState(
    /*showArtistsWithFullAlbumsState*/ true,
  );
  const [songCount, setSongCount] = useAtom(minSongCountForArtistListState);
  const onIncrement = useCallback(
    () => setSongCount(Math.min(100, songCount + 1)),
    [minSongCountForArtistListState],
  );
  const onDecrement = useCallback(
    () => setSongCount(Math.max(1, songCount - 1)),
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
  const neverPlayHates = useBoolState(/*neverPlayHatesState*/ true);
  const onlyPlayLikes = useBoolState(/*onlyPlayLikesState*/ false);
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
  const dlAlbumArtwork = useBoolState(/*downloadAlbumArtworkState*/ true);
  const dlArtistArtwork = useBoolState(/*downloadArtistArtworkState*/ true);
  const saveAlbumArtwork = useBoolState(
    /*saveAlbumArtworkWithMusicState*/ true,
  );
  const [coverArtName, setCoverArtName] = useState(
    /*albumCoverNameState*/ 'cover.jpg',
  );
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
