import {
  chkArrayOf,
  chkObjectOfType,
  isString,
  typecheck,
} from '@freik/typechk';

import { IgnoreItemType, StrId } from './Shared/CommonTypes';

// This is mostly just silly boilerplate to make sure that the enums
// are properly typed and can be used in TypeScript.
// I could generate this code from the gencpp.ts script, but I don't want to

export type IgnoreItem = { type: IgnoreItemType; value: string };

export const isIgnoreItemType: typecheck<IgnoreItemType> = (
  val: unknown,
): val is IgnoreItemType =>
  val === 'path-root' || val === 'path-keyword' || val === 'dir-name';

export const isIgnoreItem = chkObjectOfType<IgnoreItem>({
  type: isIgnoreItemType,
  value: isString,
});
export const isIgnoreItemArray = chkArrayOf<IgnoreItem>(isIgnoreItem);

export function st(id: StrId): string {
  return id;
}
