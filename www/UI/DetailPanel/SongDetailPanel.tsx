import { ContextualMenu, Dialog, DialogType } from '@fluentui/react';
import { Spinner } from '@freik/fluentui-tools';
import { useAtom, useAtomValue } from 'jotai';
import { RESET } from 'jotai/utils';
import { ReactElement } from 'react';
import { maybeDataForSongFunc } from 'www/State/DisplayData';
import { songDetailState } from 'www/State/SongPlayback';
import { MediaInfoTable } from './MediaInfo';

export function SongDetailPanel(): ReactElement {
  const [detailSongs, setSongDetail] = useAtom(songDetailState);
  const songInfo = useAtomValue(maybeDataForSongFunc([...detailSongs]));
  const dismissClick = () => setSongDetail(RESET);
  let elem;
  let header = '';
  if (detailSongs.size === 0) {
    elem = <div />;
    header = 'No file selected';
  } else if (detailSongs.size === 1) {
    elem = <MediaInfoTable keyOrKeys={[...detailSongs][0]} />;
    header = `Details for ${songInfo!.title}`;
  } else {
    elem = <MediaInfoTable keyOrKeys={[...detailSongs]} />;
    header = `Details for ${detailSongs.size} songs`;
  }

  return detailSongs.size === 0 ? (
    <></> // Doing it this way makes it disappear, instead of 2-phase mess
  ) : (
    <Dialog
      hidden={detailSongs.size === 0}
      dialogContentProps={{
        type: DialogType.normal,
        title: header,
      }}
      modalProps={{
        isBlocking: false,
        dragOptions: {
          moveMenuItemText: 'Move',
          closeMenuItemText: 'Close',
          menu: ContextualMenu,
        },
      }}
      minWidth={600}
      onDismiss={dismissClick}
      closeButtonAriaLabel="Close">
      <Spinner label="Loading...">{elem}</Spinner>
    </Dialog>
  );
}
