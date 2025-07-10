import { useAtomValue } from 'jotai';
import { ReactElement } from 'react';
import { useJotaiCallback } from 'www/Jotai/Helpers';
import {
  isSongHatedFam,
  isSongLikedFam,
  songListLikeNumberFromStringFam,
} from 'www/Jotai/LikesAndHates';
import { Song, SongKey } from 'www/Shared/CommonTypes';

function Liker({ songId }: { songId: SongKey }): ReactElement {
  const likeNum = useAtomValue(songListLikeNumberFromStringFam(songId));
  const strings = ['⋯', '👍', '👎', '⋮'];
  const onClick = useJotaiCallback(
    (get, set) => {
      switch (likeNum) {
        case 0: // neutral
          set(isSongLikedFam(songId), true);
          break;
        case 1: // like
          set(isSongHatedFam(songId), true);
          break;
        case 2: // hate
          set(isSongHatedFam(songId), false);
          break;
        case 3: // mixed
          set(isSongLikedFam(songId), false);
          set(isSongHatedFam(songId), false);
          break;
      }
    },
    [songId, likeNum],
  );
  return <div onClick={onClick}>{strings[likeNum]}</div>;
}

// This is a function, and not a React Function Component, so you can't
// have state in it: Gotta wrap it. Kinda weird, but whatever...
export function LikeOrHate(song: Song): ReactElement {
  return <Liker songId={song.key} />;
}
