import { BoolState } from '@freik/react-tools';
import { PrimitiveAtom, WritableAtom, atom, useAtom } from 'jotai';
import { atomFamily } from 'jotai-family';
import { SetAtomFamily, WritableAtomType } from './StateTypes';

export function useJotaiBoolState(atm: WritableAtomType<boolean>): BoolState {
  const [val, setter] = useAtom(atm);
  return [val, () => setter(false), () => setter(true)];
}

export function MakeSetAtomFamily<T>(): SetAtomFamily<T> {
  const theSetState = atom(new Set<T>());
  const theFamily = atomFamily((key: T) =>
    atom(
      (get) => get(theSetState).has(key),
      (get, set, newValue: boolean) => {
        const s = get(theSetState);
        const newS = new Set(s);
        if (newValue) {
          newS.delete(key);
        } else {
          newS.add(key);
        }
        set(theSetState, newS);
      },
    ),
  );
  return [theSetState, theFamily];
}
