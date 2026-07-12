import { ReactElement, useState } from 'react';
import { useAtom, useAtomValue } from 'jotai';

import { IComboBoxOption, IDropdownOption } from '@fluentui/react';
import {
  Button,
  Dropdown,
  Input,
  Label,
  SpinButton,
  SpinButtonProps,
  Text,
} from '@fluentui/react-components';
import { isArrayOfString, isDefined } from '@freik/typechk';

import {
  chkTranscodeSource,
  IpcCall,
  TranscodeSource,
} from '../../../Shared/CommonTypes';
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
import { SendMain } from '../../../Tools/Ipc';
import { ShowOpenDialog, StringSpinButton } from '../../../Tools/Utilities';
import { Setter } from '../../../Types';
import { ToggleSwitch } from '../Tools';
import {
  AlbumSelector,
  ArtistSelector,
  PlaylistSelector,
} from './SourceSelectors';
import { TranscodeStatus } from './TranscodeStatus';

import '../styles/Tools.css';

import {
  MoreCircle20Filled,
  MoreHorizontal20Regular,
} from '@fluentui/react-icons';

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

const BITRATE_LOW = 64;
const BITRATE_HIGH = 320;
export function TranscoderConfiguration(): ReactElement {
  const copyArtwork = useState(false);
  const mirror = useState(false);
  const [srcLocType, setSrcLocType] = useAtom(sourceLocationTypeState);
  const [srcDirLoc, setSrcDirLoc] = useAtom(sourceLocationDirState);
  const [dstLoc, setDstLoc] = useAtom(destLocationState);
  const [err, setError] = useState('');
  const [bitrate, setBitrate] = useAtom(xcodeBitRateState);
  const [displayBitrate, setDisplayBitrate] = useState(`${bitrate} Kbps`);
  const validSource = useAtomValue(validSourceState);
  const srcLocDescr = useAtomValue(sourceLocationState);
  // const [targetFormat, setTargetFormat] = useState<IDropdownOption>(targetFormats[0]);
  // const xcodeStatus = <TranscodeSummary />;

  const onBitrateChange: SpinButtonProps['onChange'] = useJotaiCallback(
    (get, set, _ev, data) => {
      if (data.value !== undefined && data.value !== null) {
        const val = Math.max(BITRATE_LOW, Math.min(data.value, BITRATE_HIGH));
        setBitrate(val);
        setDisplayBitrate(`${val} Kbps`);
      } else if (data.displayValue !== undefined) {
        const newValue = Number.parseInt(data.displayValue, 10);
        if (!Number.isNaN(newValue)) {
          const val = Math.max(BITRATE_LOW, Math.min(newValue, BITRATE_HIGH));
          setBitrate(val);
          setDisplayBitrate(`${val} Kbps`);
        } else {
          // Display a "special" value when user types something
          // that's not parsable as a number.
          setBitrate(128);
          setDisplayBitrate('128 Kbps');
        }
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
        <Input
          value={srcDirLoc}
          readOnly
          required
          contentAfter={
            <Button
              size="small"
              appearance="transparent"
              onClick={() => {
                getDir(setSrcDirLoc, setError);
              }}>
              <MoreHorizontal20Regular />
            </Button>
          }
        />
      );
      break;
  }
  // To get cover-art, see this page:
  // https://stackoverflow.com/questions/17798709/ffmpeg-how-to-embed-cover-art-image-to-m4a
  return (
    <>
      <Text size={400}>
        Transcode (downsample) audio files into a particular directory.
      </Text>
      <div id="xcode-source-area">
        <Dropdown
          label="Music Source"
          selectedKey={srcLocType}
          onChange={onSelectSource}
          options={sourceOptions}
        />
        {xcodeSrcLocElem}
      </div>
      <Label weight="semibold" htmlFor="dstLoc">
        Destination
      </Label>
      <Input
        id="dstLoc"
        value={dstLoc}
        readOnly
        required
        contentAfter={
          <Button
            size="small"
            appearance="transparent"
            onClick={() => getDir(setDstLoc, setError)}>
            <MoreHorizontal20Regular />
          </Button>
        }
      />
      <div id="xcode-options">
        <div>
          <Label weight="semibold" htmlFor="bitrate-spinbutton">
            Target Bit Rate
          </Label>
          <SpinButton
            id="bitrate-spinbutton"
            value={bitrate}
            displayValue={displayBitrate}
            onChange={onBitrateChange}
            min={BITRATE_LOW}
            max={BITRATE_HIGH}
            step={4}
            stepPage={16}
          />
        </div>
        <ToggleSwitch label="Copy artwork (NYI) " use={copyArtwork} />
        <ToggleSwitch
          label={'Mirror Source WARNING: May delete files!'}
          use={mirror}
        />
        <Button
          appearance="primary"
          id="transcode-button"
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
          }}>
          Transcode
        </Button>
      </div>
      <div>{err}</div>
      <TranscodeStatus />
    </>
  );
}
