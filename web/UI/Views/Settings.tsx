import { CSSProperties, ReactElement, useCallback, useState } from 'react';
import { useAtom, useAtomValue } from 'jotai';

import {
  Button,
  Dropdown,
  DropdownProps,
  Input,
  InputProps,
  Label,
  Option,
  SpinButton,
  SpinButtonProps,
  Text,
  Tooltip,
} from '@fluentui/react-components';
import {
  Add20Regular,
  ArrowDownload20Regular,
  Delete20Regular,
  SaveImage20Filled,
  Search20Regular,
} from '@fluentui/react-icons';
import { Expandable } from '@freik/fluent9-tools';
import {
  isArrayOfString,
  isDefined,
  isNumber,
  isString,
  isUndefined,
} from '@freik/typechk';

import { st } from '../../Constants';
import {
  IgnoreItemType,
  IpcCall,
  Keys,
  OpenDialogOptions,
  SocketMsg,
  StrId,
} from '../../Shared/CommonTypes';
import { allAlbumsState } from '../../State/Albums';
import { allArtistsState } from '../../State/Artists';
import { useJotaiAsyncCallback } from '../../State/Helpers';
import {
  neverPlayHatesState,
  onlyPlayLikesState,
} from '../../State/LikesAndHates';
import { rescanInProgressState } from '../../State/Miscellany';
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
} from '../../State/SimpleSettings';
import { ignoreItemsState } from '../../State/SongPlayback';
import { allSongsState } from '../../State/Songs';
import { PostMain, SendMessage } from '../../Tools/Ipc';
import { ShowOpenDialog } from '../../Tools/Utilities';
import { GetHelperText } from '../../Utils';
import { ToggleSwitch } from './Tools';

import './styles/Settings.css';

const btnWidth: CSSProperties = { width: '155px', padding: 0 };

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
          <Button
            icon={<Delete20Regular />}
            onClick={() => void setAllLocs(removeFromSet(allLocs, elem))}
          />
          <Label>{elem}</Label>&nbsp;
          {defLoc === elem ? (
            <Text size={400}>Default "Save" (NYI)</Text>
          ) : (
            <Button
              icon={<SaveImage20Filled />}
              onClick={() => void setDefLoc(elem)}>
              NYI: Set as Default
            </Button>
          )}
        </span>
      ))}
      <div>
        <Button
          onClick={onAddLocation}
          icon={<Add20Regular />}
          title={GetHelperText(Keys.AddFileLocation)}
          style={btnWidth}>
          Add Location
        </Button>
        &nbsp;
        <Tooltip
          relationship="description"
          content="Necessary if you moved files around since launching the app">
          <Button
            icon={<Search20Regular />}
            disabled={rescanInProgress}
            onClick={() => SendMessage(SocketMsg.ManualRescan)}
            style={btnWidth}>
            Rescan Locations
          </Button>
        </Tooltip>
        &nbsp;
        <Button
          /* onClick={() => {}} */
          icon={<ArrowDownload20Regular />}
          title={st(StrId.ImportFiles)}
          style={btnWidth}>
          Import Files (NYI)
        </Button>
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

function IgnoreList(): ReactElement {
  const ignoreItems = useAtomValue(ignoreItemsState);
  const [selectedIgnoreOptions, setSelectedIgnoreOptions] = useState<string[]>(
    [],
  );
  const [ignoreValue, setIgnoreValue] = useState('');
  const onOptionSelect: DropdownProps['onOptionSelect'] = (ev, data) => {
    setSelectedIgnoreOptions(data.selectedOptions);
    setIgnoreValue(data.optionValue ?? '');
  };

  const [newType, setNewType] = useState<IgnoreItemType | ''>('');
  const [newValue, setNewValue] = useState<string>('');
  const ignoreValueChange: InputProps['onChange'] = useCallback((ev, data) => {
    if (isDefined(data.value)) {
      setNewValue(data.value);
    }
  }, []);
  return (
    <div id="ignore-list">
      {ignoreItems.map(({ type, value }, idx) => (
        <div key={idx} style={{ display: 'contents' }}>
          <span style={{ gridRow: idx + 1 }} className="ignore-type">
            {ignoreTypeNameMap.get(type) || 'ERROR!'}:
          </span>
          <span style={{ gridRow: idx + 1 }} className="ignore-value">
            <Input type="text" readOnly value={value} />
          </span>
          <span style={{ gridRow: idx + 1 }} className="ignore-button">
            <Button
              icon={<Delete20Regular />}
              onClick={() => {
                /* TODO RemoveIgnoreItem({ type, value } ); */
              }}
            />
          </span>
        </div>
      ))}
      <span style={{ gridRow: ignoreItems.length + 1 }} className="ignore-type">
        <Dropdown
          selectedOptions={selectedIgnoreOptions}
          onOptionSelect={onOptionSelect}>
          {[...ignoreTypeNameMap.entries()].map(([iit, val]) => (
            <Option key={iit}>{val}</Option>
          ))}
        </Dropdown>
      </span>
      <span
        style={{ gridRow: ignoreItems.length + 1 }}
        className="ignore-value">
        <Input type="text" value={newValue} onChange={ignoreValueChange} />
      </span>
      <span
        style={{ gridRow: ignoreItems.length + 1 }}
        className="ignore-button">
        <Button
          icon={<Add20Regular />}
          onClick={() => {
            if (newType !== '') {
              // TODO: AddIgnoreItem({ type: newType, value: newValue });
            }
            setNewType('');
            setNewValue('');
          }}
          disabled={newValue.length === 0}
        />
      </span>
    </div>
  );
}

function ArticleSorting(): ReactElement {
  const articles = useAtom(ignoreArticlesState);
  return <ToggleSwitch use={articles} label={'Ignore articles when sorting'} />;
}

function ArtistFiltering(): ReactElement {
  const onlyAlbumArtists = useAtom(showArtistsWithFullAlbumsState);
  const [songCount, setSongCount] = useAtom(minSongCountForArtistListState);
  const onChange: SpinButtonProps['onChange'] = useCallback(
    (_ev, newValue) => {
      if (isNumber(newValue)) {
        void setSongCount(Math.min(Math.max(newValue, 1), 100));
      }
    },
    [minSongCountForArtistListState],
  );
  return (
    <>
      <ToggleSwitch
        label="Only show artists with full albums (JODO)"
        use={onlyAlbumArtists}
      />
      <br />
      <Label htmlFor="song-count">
        Only show artists with at least this many songs (JODO)
      </Label>
      <SpinButton
        id="song-count"
        disabled={onlyAlbumArtists[0]}
        value={songCount}
        onChange={onChange}
        style={{ width: '10px' }}
      />
    </>
  );
}

function LikeFiltering(): ReactElement {
  const neverPlayHates = useAtom(neverPlayHatesState);
  const onlyPlayLikes = useAtom(onlyPlayLikesState);
  return (
    <>
      <ToggleSwitch
        label="Never queue up songs you dislike"
        use={neverPlayHates}
      />
      <br />
      <ToggleSwitch label="Only queue up songs you like" use={onlyPlayLikes} />
    </>
  );
}

function ArtworkSettings(): ReactElement {
  const dlAlbumArtwork = useAtom(downloadAlbumArtworkState);
  const dlArtistArtwork = useAtom(downloadArtistArtworkState);
  const saveAlbumArtwork = useAtom(saveAlbumArtworkWithMusicState);
  const [coverArtName, setCoverArtName] = useAtom(albumCoverNameState);
  return (
    <>
      <ToggleSwitch label="Download Album Artwork" use={dlAlbumArtwork} />
      <br />
      <div className="artwork-settings">
        <ToggleSwitch
          disabled={!dlAlbumArtwork[0]}
          label="Try to save Album Artwork with audio files:"
          use={saveAlbumArtwork}
        />
        &nbsp;
        <Label style={{ fontSize: 11, paddingTop: 5 }} htmlFor="cover-art-name">
          Filename to save the artwork as
        </Label>
        <Input
          type="text"
          id="cover-art-name"
          disabled={!saveAlbumArtwork[0] || !dlAlbumArtwork[0]}
          value={coverArtName}
          onChange={(_ev, nv) => nv.value && void setCoverArtName(nv.value)}
        />
      </div>
      <ToggleSwitch label="Download Artist Artwork" use={dlArtistArtwork} />
      <br />
      <Button
        style={{ ...btnWidth, gridRow: 4 }}
        onClick={() => PostMain(IpcCall.FlushImageCache)}>
        Flush Image Cache
      </Button>
    </>
  );
}

export function SettingsView(): ReactElement {
  const [Data, setData] = useState<string>('<uninitialized>');
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
        <br />
        <ArticleSorting />
        <br />
        <ArtistFiltering />
      </Expandable>
      <Expandable separator label="Artwork" defaultShow={true}>
        <ArtworkSettings />
      </Expandable>
      <Expandable separator label="Metadata" defaultShow={true}>
        <>
          <Button
            style={btnWidth}
            onClick={() => PostMain(IpcCall.FlushMetadataCache)}>
            Flush Metadata Cache
          </Button>
          &nbsp;
          <Button
            style={btnWidth}
            onClick={() => PostMain(IpcCall.ClearLocalOverrides)}>
            Clear Local Overrides
          </Button>
          <Button
            style={btnWidth}
            onClick={() => {
              const odo: OpenDialogOptions = {
                folder: true,
                title: 'This is the title',
              };
              ShowOpenDialog(odo).then((val) => {
                if (isUndefined(val)) {
                  setData('Undefined result');
                } else if (isArrayOfString(val)) {
                  setData(val.join('; '));
                } else if (isString(val)) {
                  setData(val);
                } else {
                  setData(
                    'non-string result from OFD:' +
                      JSON.stringify(val).toString(),
                  );
                }
              });
            }}>
            Show Open File Dialog
          </Button>
          <div>{Data}</div>
        </>
      </Expandable>
    </div>
  );
}
