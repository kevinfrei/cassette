import {
  DefaultButton,
  Dropdown,
  IComboBoxOption,
  IDropdownOption,
  Text,
  TextField,
} from '@fluentui/react';
import { StateToggle } from '@freik/fluentui-tools';
import { useBoolState } from '@freik/react-tools';
import { isArrayOfString, isDefined } from '@freik/typechk';
import { useAtom, useAtomValue } from 'jotai';
import { ReactElement, useState } from 'react';
import {
  chkTranscodeSource,
  IpcCall,
  TranscodeSource,
} from 'www/Shared/CommonTypes';
import { ShowOpenDialog, StringSpinButton } from 'www/Tools/Utilities';
import { useJotaiCallback } from '../../../State/Helpers';
import {
  destLocationState,
  sourceLocationAlbumState,
  sourceLocationArtistState,
  sourceLocationDirState,
  sourceLocationPlaylistState,
  sourceLocationState,
  sourceLocationTypeState,
  validSourceState,
  xcodeBitRateState,
} from '../../../State/Transcode';
import {
  AlbumSelector,
  ArtistSelector,
  PlaylistSelector,
} from './SourceSelectors';
import { TranscodeStatus } from './TranscodeStatus';

import { PostMain, SendMain } from 'www/Tools/Ipc';
import { Setter } from 'www/Types';
import '../styles/Tools.css';

/*
const targetFormats: IDropdownOption[] = [
  { key: 'm4a', text: 'm4a' },
  { key: 'aac', text: 'aac (raw mp4/m4a)' },
  { key: 'mp3', text: 'mp3' },
];
*/

const sourceOptions: IComboBoxOption[] = [
  { key: TranscodeSource.Playlist, text: 'Playlist' },
  { key: TranscodeSource.Artist, text: 'Artist' },
  { key: TranscodeSource.Album, text: 'Album' },
  { key: TranscodeSource.Disk, text: 'Disk location' },
];

function getDir(setter: Setter<string>, setError: Setter<string>) {
  ShowOpenDialog({ folder: true, title: 'Select a directory' })
    .then((val) => {
      if (isArrayOfString(val) && val.length >= 1) {
        setter(val[0]);
      }
    })
    .catch(() => {
      setError('Failed to find a dir');
    });
}

export function TranscoderConfiguration(): ReactElement {
  const copyArtwork = useBoolState(false);
  const mirror = useBoolState(false);
  const [srcLocType, setSrcLocType] = useAtom(sourceLocationTypeState);
  const [srcDirLoc, setSrcDirLoc] = useAtom(sourceLocationDirState);
  const [dstLoc, setDstLoc] = useAtom(destLocationState);
  const [err, setError] = useState('');
  const bitrate = useAtomValue(xcodeBitRateState);
  const validSource = useAtomValue(validSourceState);
  const srcLocDescr = useAtomValue(sourceLocationState);
  // const [targetFormat, setTargetFormat] = useState<IDropdownOption>(targetFormats[0]);
  // const xcodeStatus = <TranscodeSummary />;

  const onChange = useJotaiCallback(
    (get, set, numVal?: number) => {
      if (isDefined(numVal)) {
        set(xcodeBitRateState, numVal);
      }
    },
    [xcodeBitRateState],
  );

  const onSelectSource = (
    event: React.FormEvent<HTMLDivElement>,
    option?: IDropdownOption,
  ): void => {
    if (isDefined(option) && chkTranscodeSource(option.key)) {
      setSrcLocType(option.key);
    }
  };

  // TODO: Create the element for the transcode source type (and populated it, if appropriate)
  let xcodeSrcLocElem;
  switch (srcLocType) {
    case TranscodeSource.Playlist:
      xcodeSrcLocElem = (
        <PlaylistSelector value={sourceLocationPlaylistState} />
      );
      break;
    case TranscodeSource.Artist:
      xcodeSrcLocElem = <ArtistSelector value={sourceLocationArtistState} />;
      break;
    case TranscodeSource.Album:
      xcodeSrcLocElem = <AlbumSelector value={sourceLocationAlbumState} />;
      break;
    case TranscodeSource.Disk:
    default:
      xcodeSrcLocElem = (
        <TextField
          value={srcDirLoc}
          readOnly
          required
          onClick={() => {
            getDir(setSrcDirLoc, setError);
          }}
          iconProps={{ iconName: 'More' }}
        />
      );
      break;
  }
  // To get cover-art, see this page:
  // https://stackoverflow.com/questions/17798709/ffmpeg-how-to-embed-cover-art-image-to-m4a
  return (
    <>
      <Text variant="mediumPlus">
        Transcode (downsample) audio files into a particular directory.
      </Text>
      <br />
      <div id="xcode-source-area">
        <Dropdown
          label="Music Source"
          selectedKey={srcLocType}
          onChange={onSelectSource}
          options={sourceOptions}
        />
        {xcodeSrcLocElem}
      </div>
      <TextField
        label="Destination"
        value={dstLoc}
        readOnly
        required
        onClick={() => getDir(setDstLoc, setError)}
        iconProps={{ iconName: 'More' }}
      />
      <br />
      <div id="xcode-options">
        <StringSpinButton
          label="Target Bit Rate"
          value={bitrate}
          filter={(val: string) => {
            const numericValue = parseInt(val.trim(), 10);
            return isNaN(numericValue) ? undefined : numericValue;
          }}
          format={(val: number) => `${val} Kbps`}
          onChange={onChange}
          min={64}
          max={320}
          step={4}
        />
        <StateToggle label="Copy artwork (NYI) " state={copyArtwork} />
        <StateToggle
          label={'Mirror Source WARNING: May delete files!'}
          state={mirror}
        />
        <DefaultButton
          text="Transcode"
          disabled={!validSource || dstLoc.length === 0}
          onClick={() => {
            SendMain(IpcCall.TranscodingBegin, {
              source: srcLocDescr,
              dest: dstLoc,
              artwork: copyArtwork[0],
              mirror: mirror[0],
              format: 'm4a',
              bitrate: bitrate * 1024,
            });
          }}
        />
      </div>
      <div>{err}</div>
      <TranscodeStatus />
    </>
  );
}
