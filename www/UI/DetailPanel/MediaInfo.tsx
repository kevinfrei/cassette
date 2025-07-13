import {
  DetailsList,
  IColumn,
  SelectionMode,
  TextField,
} from '@fluentui/react';
import { SongKey } from '@freik/media-core';
import { isArray, isString } from '@freik/typechk';
import { useAtom, useAtomValue } from 'jotai';
import {
  commonMetadataFromSongKeys,
  mediaInfoStateFamily,
} from '../../Jotai/MediaInfo';
import { SimpleSongsList } from './../Views/MixedSongs';
import { MetadataEditor } from './MetadataEditor';

import { Expandable } from '@freik/fluentui-tools';
import { albumByKey } from 'www/Jotai/Albums';
import { artistStringStateFamily } from 'www/Jotai/Artists';
import { songByKey } from 'www/Jotai/Songs';
import { IpcCall } from 'www/Shared/CommonTypes';
import { SendMain } from 'www/Tools/Ipc';
import { altRowRenderer } from 'www/Tools/SongList';
import { divGrand, fractionalSecondsStrToHMS } from 'www/Utils';

import './styles/MediaInfo.css';

const fileTypeMap = new Map([
  ['FLAC', 'flac'],
  ['MPEG 1 Layer 3', 'mp3'],
  ['MPEG-4/AAC', 'aac'],
]);

function getType(codec: string | void): string {
  if (codec) {
    const res = fileTypeMap.get(codec);
    if (res) return res;
  }
  return '???';
}

function channelDescr(noc: string | void): string {
  if (noc === '1') return 'mono';
  if (noc === '2') return 'stereo';
  if (noc === '4') return 'quadrophonic';
  return `${noc ? noc : '?'} channels`;
}

function getBitRate(br: string | void): string {
  return `${br ? divGrand(br) : '??'}Kbps`;
}

function getSampleRate(sr: string | void): string {
  return `${sr ? divGrand(sr) : '??'}KHz`;
}

// This is the header for single-file editing
function MediaFormatDetails({ forSong }: { forSong: SongKey }): JSX.Element {
  const mediaInfo = useAtomValue(mediaInfoStateFamily(forSong));

  const fileType = getType(mediaInfo.audio.get('format.codec'));
  const duration = fractionalSecondsStrToHMS(
    mediaInfo.audio.get('format.duration') || '0',
  );
  const sampleRate = getSampleRate(mediaInfo.audio.get('format.sampleRate'));
  const bps = mediaInfo.audio.get('format.bitsPerSample') || '16';
  const channels = channelDescr(mediaInfo.audio.get('format.numberOfChannels'));
  const bitrate = getBitRate(mediaInfo.audio.get('format.bitrate'));
  const thePath = mediaInfo.audio.get('File Path') || '';

  return (
    <>
      <TextField
        readOnly
        prefix="File Path"
        value={thePath}
        styles={{ field: { direction: 'rtl' } }}
        onDoubleClick={() => SendMain(IpcCall.ShowFile, thePath)}
      />
      <br />
      <div className="metadata-specs">
        <TextField
          prefix="Duration:"
          readOnly
          value={duration}
          style={{ width: '75px' }}
        />
        <TextField
          prefix={fileType.toLocaleUpperCase() + ':'}
          readOnly
          value={`${bitrate} (${bps} bit ${sampleRate} ${channels})`}
          style={{ width: '280px' }}
        />
      </div>
    </>
  );
}
function RawMetadata({ songKey }: { songKey: SongKey }): JSX.Element {
  const columns: IColumn[] = [
    {
      key: '0',
      name: 'Field',
      fieldName: '0',
      minWidth: 25,
      className: 'md-field',
      isResizable: true,
    },
    {
      key: '1',
      name: 'Value',
      fieldName: '1',
      minWidth: 100,
      isResizable: true,
    },
  ];

  const mediaInfo = useAtomValue(mediaInfoStateFamily(songKey));
  return (
    <DetailsList
      items={[...mediaInfo.audio.entries()]}
      selectionMode={SelectionMode.none}
      columns={columns}
      compact
      onRenderRow={altRowRenderer()}
    />
  );
}

function SingleFileEditor({ songKey }: { songKey: SongKey }): JSX.Element {
  const theSong = useAtomValue(songByKey(songKey));
  const theArtist = useAtomValue(artistStringStateFamily(theSong.artistIds));
  const moreArtists = useAtomValue(
    artistStringStateFamily(theSong.secondaryIds),
  );
  const theAlbum = useAtomValue(albumByKey(theSong.albumId));
  const diskNum = Math.floor(theSong.track / 100);
  const diskName =
    diskNum > 0 &&
    isArray(theAlbum.diskNames) &&
    theAlbum.diskNames.length >= diskNum
      ? theAlbum.diskNames[diskNum - 1]
      : '';
  return (
    <MetadataEditor
      forSong={songKey}
      artist={theArtist}
      album={theAlbum.title}
      track={theSong.track.toString()}
      title={theSong.title}
      year={theAlbum.year > 0 ? theAlbum.year.toString() : ''}
      va={theAlbum.vatype}
      diskName={diskName}
      moreArtists={moreArtists}
      variations={theSong.variations ? theSong.variations.join('; ') : ''}
      albumId={theSong.albumId}
    />
  );
}

function MultiFileEditor({ songKeys }: { songKeys: SongKey[] }): JSX.Element {
  const allTheInfos = useAtomValue(commonMetadataFromSongKeys(songKeys));
  return <MetadataEditor forSongs={songKeys} {...allTheInfos} />;
}

export function MediaInfoTable({
  keyOrKeys,
}: {
  keyOrKeys: SongKey | SongKey[];
}): JSX.Element {
  if (keyOrKeys.length === 0) {
    return <></>;
  }
  // Either Details or a Multi-editing header
  const theHeader = isString(keyOrKeys) ? (
    <MediaFormatDetails forSong={keyOrKeys} />
  ) : (
    <Expandable label="Files Selected">
      <SimpleSongsList forSongs={keyOrKeys} />
    </Expandable>
  );

  // For single-song, the raw metadata in a table
  const theFooter = isString(keyOrKeys) ? (
    <Expandable label="Raw Metadata" separator>
      <RawMetadata songKey={keyOrKeys} />
    </Expandable>
  ) : (
    <></>
  );
  const theEditor = isString(keyOrKeys) ? (
    <SingleFileEditor songKey={keyOrKeys} />
  ) : (
    <MultiFileEditor songKeys={keyOrKeys} />
  );
  return (
    <>
      {theHeader}
      {theEditor}
      {theFooter}
    </>
  );
}
