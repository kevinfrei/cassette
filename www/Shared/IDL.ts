// My silly IDL is actually just a set of functions that can be interpreted
// by a script to create types for the data structures.

import { isObjectNonNull, isString } from '@freik/typechk';

export type Of<T> = { [key: string]: T };
export type Str = 's';
export type Char = 'c';
export type U8 = '0';
export type I8 = '1';
export type U16 = '2';
export type I16 = '3';
export type U32 = '4';
export type I32 = '5';
export type U64 = '6';
export type I64 = '7';
export type I = '8'; // This is just used for Enum types for generics
export type Bool = 'b';
export type Flt = 'f';
export type Dbl = 'd';

export type Int = U8 | I8 | U16 | I16 | U32 | I32 | U64 | I64;
export type Num = Int | Flt | Dbl;
export type Simple = Str | Char | Int | Bool | Flt | Dbl;

export type ObjType = { t: 'O'; d: Of<Anonymous> };
export type ArrType = { t: 'A'; d: Anonymous };
export type SetType = { t: 'S'; d: Anonymous };
export type MapType = { t: 'M'; k: Anonymous; v: Anonymous };
export type TupType = { t: 'T'; l: Anonymous[] };
export type RefType = { t: 'R'; r: string }; // Reference type, Points at a type by name

export type Enum = { t: '#'; u: Int | I; v: string[] };
export type NEnum = { t: '%'; u: Int | I; v: Of<number> };
export type SEnum = { t: '$'; v: Of<string> };

export type EnumType = Enum | NEnum | SEnum;
// Objects and Enumeratoins have to be named, so they can't be used in anonymous context
export type Anonymous =
  | Simple
  | RefType
  | ArrType
  | SetType
  | MapType
  | TupType;
export type Types = Anonymous | ObjType | EnumType | I;
export type NamedTypes = ObjType | EnumType;

export const str = (): Str => 's';
export const chr = (): Char => 'c';
export const u8 = (): U8 => '0';
export const i8 = (): I8 => '1';
export const u16 = (): U16 => '2';
export const i16 = (): I16 => '3';
export const u32 = (): U32 => '4';
export const i32 = (): I32 => '5';
export const u64 = (): U64 => '6';
export const i64 = (): I64 => '7';
export const num = (): I => '8'; // This is just used for Enum types
export const bool = (): Bool => 'b';
export const flt = (): Flt => 'f';
export const dbl = (): Dbl => 'd';
export const arr = (d: Anonymous): ArrType => ({ t: 'A', d });
export const set = (d: Anonymous): SetType => ({ t: 'S', d });
export const map = (k: Anonymous, v: Anonymous): MapType => ({
  t: 'M',
  k,
  v,
});
export const tup = (...l: Anonymous[]): TupType => ({ t: 'T', l });
export const obj = (d: Of<Anonymous>): ObjType => ({ t: 'O', d });
export const ref = (r: string): RefType => ({ t: 'R', r });
export const enum_lst = (u: Int | I, v: string[]): Enum => ({ t: '#', u, v });
export const enum_num = (u: Int | I, v: Of<number>): NEnum => ({
  t: '%',
  u,
  v,
});
export const enum_str = (v: Of<string>): SEnum => ({ t: '$', v });

// Type guards for the IDL types
export function isStringType(x: Types): x is Str {
  return x === 's';
}
export function isCharType(x: Types): x is Char {
  return x === 'c';
}
export function isU8Type(x: Types): x is U8 {
  return x === '0';
}
export function isI8Type(x: Types): x is I8 {
  return x === '1';
}
export function isU16Type(x: Types): x is U16 {
  return x === '2';
}
export function isI16Type(x: Types): x is I16 {
  return x === '3';
}
export function isU32Type(x: Types): x is U32 {
  return x === '4';
}
export function isI32Type(x: Types): x is I32 {
  return x === '5';
}
export function isU64Type(x: Types): x is U64 {
  return x === '6';
}
export function isI64Type(x: Types): x is I64 {
  return x === '7';
}
export function isPlainIntEnumType(x: Types): x is I {
  return x === '8';
}
export function isBoolType(x: Types): x is Bool {
  return x === 'b';
}
export function isFloatType(x: Types): x is Flt {
  return x === 'f';
}
export function isDoubleType(x: Types): x is Dbl {
  return x === 'd';
}
export function isSimpleType(x: Types): x is Simple {
  return isString(x) && x.length === 1 && 'sc01234567bfd'.indexOf(x) >= 0;
}
export function isRefType(x: Types): x is RefType {
  return isObjectNonNull(x) && x.t === 'R';
}
export function isArrayType(x: Types): x is ArrType {
  return isObjectNonNull(x) && x.t === 'A';
}
export function isSetType(x: Types): x is SetType {
  return isObjectNonNull(x) && x.t === 'S';
}
export function isMapType(x: Types): x is MapType {
  return isObjectNonNull(x) && x.t === 'M';
}
export function isTupleType(x: Types): x is TupType {
  return isObjectNonNull(x) && x.t === 'T';
}
export function isObjectType(x: Types): x is ObjType {
  return isObjectNonNull(x) && x.t === 'O';
}
export function isPlainEnumType(x: Types): x is Enum {
  return isObjectNonNull(x) && x.t === '#';
}
export function isNumericEnumType(x: Types): x is NEnum {
  return isObjectNonNull(x) && x.t === '%';
}
export function isStringEnumType(x: Types): x is SEnum {
  return isObjectNonNull(x) && x.t === '$';
}
export function isEnumType(x: Types): x is EnumType {
  return isObjectNonNull(x) && (x.t === '#' || x.t === '%' || x.t === '$');
}
export function isNamedType(x: Types): x is ObjType | EnumType {
  return (
    isObjectNonNull(x) &&
    (x.t === 'O' || x.t === '#' || x.t === '%' || x.t === '$')
  );
}
export function isAnonymousType(x: Types): x is Anonymous {
  return !isNamedType(x);
}

export type SymbolList = Record<string, Types>;

// An example of how to define types using the IDL:

const ExampleEnum1 = enum_lst(u8(), ['one', 'two', 'three']);
const ExampleEnum2 = enum_num(num(), { one: 1, two: 2, tre: 3 });
const ExampleEnum3 = enum_str({
  Red: 'red',
  Green: 'green',
  Blue: 'blue',
});
const ExampleObjectType = obj({
  name: str(),
  age: i32(),
  isActive: bool(),
  scores: arr(i32()),
  metadata: ref('Metadata'),
  friends: set(str()),
  info: map(str(), ref('ExampleTupleType')),
  id: ref('ExampleEnum1'),
});

const ExampleTupleType_otherName = tup(str(), i32(), bool());
const ExampleArrayType = arr(ref('ExampleObjectType'));
const ExampleNestedObjectType = obj({
  id: str(),
  data: ref('ExampleTupleType'),
});
const ExampleObjectTupleType = tup(
  ref('ExampleNestedObjectType1'),
  ref('ExampleNestedObjectType2'),
);
const ExampleNestedObjectType1 = obj({ id: str(), value: i32() });
const ExampleNestedObjectType2 = obj({ id: str(), value: dbl() });

export const TypesToGenerate: SymbolList = {
  ExampleEnum1,
  ExampleEnum2,
  ExampleEnum3,
  // Inlined type for Metadata:
  Metadata: obj({ author: str(), angle: dbl() }),
  ExampleTupleType: ExampleTupleType_otherName,
  ExampleObjectType,
  ExampleArrayType,
  ExampleNestedObjectType,
  ExampleNestedObjectType1,
  ExampleNestedObjectType2,
  ExampleObjectTupleType,
};
