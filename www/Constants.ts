import { isNumber, typecheck } from '@freik/typechk';

import {
  CurrentView_source,
  Keys_source,
  StrId_source,
} from './SharedConstants';

export const CurrentView = Object.freeze(CurrentView_source.values);
export type CurrentViewEnum = (typeof CurrentView)[keyof typeof CurrentView];

// TODO: This stuff let's me localize my strings eventually
export const StrId = Object.freeze(StrId_source.values);
export type StrIdEnum = (typeof StrId)[keyof typeof StrId];

export const Keys = Object.freeze(Keys_source.values);
export type KeysEnum = (typeof Keys)[keyof typeof Keys];

export const isCurrentView: typecheck<CurrentViewEnum> = (
  val: unknown,
): val is CurrentViewEnum =>
  isNumber(val) &&
  Number.isInteger(val) &&
  val >= CurrentView.disabled &&
  val <= CurrentView.tools;

export function st(id: StrIdEnum): string {
  return id;
}
