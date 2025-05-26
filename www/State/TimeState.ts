import { atom } from 'jotai';

export type MediaTime = {
  duration: number;
  position: number;
};

export const mediaTimeState = atom<MediaTime>({ duration: 0, position: 0 }); // Current time in seconds
export const mediaTimePercentState = atom<number>(0); // Current time as a percentage of total duration
export const mediaTimePositionState = atom<string>('0:00');
export const mediaTimeRemainingState = atom<string>('0:00');
