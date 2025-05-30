import {
  chkArrayOf,
  chkObjectOfType,
  isNumber,
  isString,
  typecheck,
} from '@freik/typechk';

import {
  CurrentView_source,
  IgnoreItemType_source,
  IpcId_source,
  Keys_source,
  StrId_source,
} from './SharedConstants';

export const CurrentView = Object.freeze(CurrentView_source.values);
export type CurrentViewEnum = (typeof CurrentView)[keyof typeof CurrentView];

export const Keys = Object.freeze(Keys_source.values);
export type KeysEnum = (typeof Keys)[keyof typeof Keys];

export const IpcId = Object.freeze(IpcId_source.values);
export type IpcIdEnum = (typeof IpcId)[keyof typeof IpcId];

export const IgnoreItemType = Object.freeze(IgnoreItemType_source.values);
export type IgnoreItemTypeEnum =
  (typeof IgnoreItemType)[keyof typeof IgnoreItemType];

export const isIgnoreItemType: typecheck<IgnoreItemTypeEnum> = (
  val: unknown,
): val is IgnoreItemTypeEnum =>
  val === 'path-root' || val === 'path-keyword' || val === 'dir-name';
export type IgnoreItem = { type: IgnoreItemTypeEnum; value: string };
export const isIgnoreItem = chkObjectOfType<IgnoreItem>({
  type: isIgnoreItemType,
  value: isString,
});
export const isIgnoreItemArray = chkArrayOf<IgnoreItem>(isIgnoreItem);

// TODO: This stuff let's me localize my strings eventually
export const StrId = Object.freeze(StrId_source.values);
export type StrIdEnum = (typeof StrId)[keyof typeof StrId];

export function st(id: StrIdEnum): string {
  return id;
}

export const isCurrentView: typecheck<CurrentViewEnum> = (
  val: unknown,
): val is CurrentViewEnum =>
  isNumber(val) &&
  Number.isInteger(val) &&
  val >= CurrentView.disabled &&
  val <= CurrentView.tools;
