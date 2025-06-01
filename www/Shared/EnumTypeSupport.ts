import {
  typecheck,
  isObjectOfExactType,
  isString,
  isBoolean,
  chk2TupleOf,
  chkRecordOf,
  chkObjectOfExactType,
  isNonNullable,
  isNumber,
} from '@freik/typechk';
import { CurrentViewEnum, CurrentView } from 'www/Constants';

export type SharedEnum<T> = {
  description: string;
  typechecker: boolean;
  type: [string, string];
  values: Record<string, T>;
};

export function isSharedEnumOf<T>(
  val: unknown,
  typechecker: typecheck<T>,
): val is SharedEnum<T> {
  return isObjectOfExactType(val, {
    description: isString,
    typechecker: isBoolean,
    type: chk2TupleOf(isString, isString),
    values: chkRecordOf(isString, typechecker),
  });
}

export function chkSharedEnumOf<T>(
  typechecker: typecheck<T>,
): typecheck<SharedEnum<T>> {
  return (val): val is SharedEnum<T> => isSharedEnumOf(val, typechecker);
}

export const chkSharedEnum: typecheck<SharedEnum<unknown>> =
  chkObjectOfExactType({
    description: isString,
    typechecker: isBoolean,
    type: chk2TupleOf(isString, isString),
    values: chkRecordOf(isString, isNonNullable),
  });

export function isSharedEnum(val: unknown): val is SharedEnum<unknown> {
  return chkSharedEnum(val);
}

export const isCurrentView: typecheck<CurrentViewEnum> = (
  val: unknown,
): val is CurrentViewEnum =>
  isNumber(val) &&
  Number.isInteger(val) &&
  val >= CurrentView.disabled &&
  val <= CurrentView.tools;
