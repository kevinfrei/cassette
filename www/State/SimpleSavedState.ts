import { atom } from 'jotai';
import { CurrentView, CurrentViewEnum } from 'www/Constants';

export const mutedState = atom<boolean>(false);
export const volumeState = atom<number>(0.5); // Volume as a percentage (0-100)

export type RepeatType = 'off' | 'one' | 'all';

export const curViewState = atom<CurrentViewEnum>(CurrentView.albums);
export const shuffleState = atom<boolean>(false);
export const repeatState = atom<RepeatType>('off');
export function NextRepeat(current: RepeatType): RepeatType {
  switch (current) {
    case 'off':
      return 'one';
    case 'one':
      return 'all';
    case 'all':
    default:
      return 'off'; // Fallback to 'off' if something goes wrong
  }
}
