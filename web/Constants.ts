import {
  chkArrayOf,
  chkObjectOfType,
  isString,
  typecheck,
} from '@freik/typechk';

import { IgnoreItemType, StrId } from './Shared/CommonTypes';

export type IgnoreItem = { type: IgnoreItemType; value: string };

// TODO: Move this function into the IDL transpiler
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
