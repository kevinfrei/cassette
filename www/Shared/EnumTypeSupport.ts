import { typecheck, isNumber } from '@freik/typechk';
import { CurrentView } from './CommonTypes';

export const isCurrentView: typecheck<CurrentView> = (
  val: unknown,
): val is CurrentView =>
  isNumber(val) &&
  Number.isInteger(val) &&
  val >= CurrentView.disabled &&
  val <= CurrentView.tools;
