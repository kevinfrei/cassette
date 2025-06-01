import {
  chkArrayOf,
  chkObjectOfType,
  isString,
  typecheck,
} from '@freik/typechk';

import {
  CurrentView_source,
  IgnoreItemType_source,
  IpcId_source,
  Keys_source,
  StrId_source,
} from './Shared/Enums';

// This is mostly just silly boilerplate to make sure that the enums
// are properly typed and can be used in TypeScript.
// I could generate this code from the gencpp.ts script, but I don't want to
/*****
 * Begin Boilerplate Code
 *****/
export const CurrentView = Object.freeze(CurrentView_source.values);
export type CurrentViewEnum = (typeof CurrentView)[keyof typeof CurrentView];

export const Keys = Object.freeze(Keys_source.values);
export type KeysEnum = (typeof Keys)[keyof typeof Keys];

export const IpcId = Object.freeze(IpcId_source.values);
export type IpcIdEnum = (typeof IpcId)[keyof typeof IpcId];

export const IgnoreItemType = Object.freeze(IgnoreItemType_source.values);
export type IgnoreItemTypeEnum =
  (typeof IgnoreItemType)[keyof typeof IgnoreItemType];

// TODO: This stuff let's me localize my strings eventually
export const StrId = Object.freeze(StrId_source.values);
export type StrIdEnum = (typeof StrId)[keyof typeof StrId];

/*****
 * End Boilerplate Code
 *****/

export type IgnoreItem = { type: IgnoreItemTypeEnum; value: string };

export const isIgnoreItemType: typecheck<IgnoreItemTypeEnum> = (
  val: unknown,
): val is IgnoreItemTypeEnum =>
  val === 'path-root' || val === 'path-keyword' || val === 'dir-name';

export const isIgnoreItem = chkObjectOfType<IgnoreItem>({
  type: isIgnoreItemType,
  value: isString,
});
export const isIgnoreItemArray = chkArrayOf<IgnoreItem>(isIgnoreItem);

export function st(id: StrIdEnum): string {
  return id;
}
