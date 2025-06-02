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

export const isCurrentView: typecheck<CurrentViewEnum> = (
  val: unknown,
): val is CurrentViewEnum =>
  isNumber(val) &&
  Number.isInteger(val) &&
  val >= CurrentView.disabled &&
  val <= CurrentView.tools;
