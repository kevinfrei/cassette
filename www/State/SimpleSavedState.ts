import { atom } from 'jotai';
import { CurrentView, CurrentViewEnum } from 'www/Constants';

export const curViewState = atom<CurrentViewEnum>(CurrentView.albums);
