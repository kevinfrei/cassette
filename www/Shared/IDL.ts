// My silly IDL is actually just a set of functions that can be interpreted
// by a script to create types for the data structures.

import { isObjectNonNull, isString } from '@freik/typechk';

export enum TypeId {
  Str = 's',
  Char = 'c',
  U8 = '0',
  I8 = '1',
  U16 = '2',
  I16 = '3',
  U32 = '4',
  I32 = '5',
  U64 = '6',
  I64 = '7',
  I = '8', // This is just used for Enum types for generics
  Bool = 'b',
  Flt = 'f',
  Dbl = 'd',
  Obj = 'O',
  Sub = 'C',
  Arr = 'A',
  Set = 'S',
  FastSet = 'H', // Fast Set, used for std::unordered_set in C++
  Map = 'M',
  FastMap = 'F', // Fast Map, used for std::unordered_map in C++
  Tup = 'T',
  Ref = 'R', // Reference type, Points at a type by name
  Enum = '#',
  NEnum = '%',
  SEnum = '$',
}

export type Of<T> = { [key: string]: T };
export type Str = TypeId.Str;
export type Char = TypeId.Char;
export type U8 = TypeId.U8;
export type I8 = TypeId.I8;
export type U16 = TypeId.U16;
export type I16 = TypeId.I16;
export type U32 = TypeId.U32;
export type I32 = TypeId.I32;
export type U64 = TypeId.U64;
export type I64 = TypeId.I64;
export type I = TypeId.I; // This is just used for Enum types for generics
export type Bool = TypeId.Bool;
export type Flt = TypeId.Flt;
export type Dbl = TypeId.Dbl;

export type Int = U8 | I8 | U16 | I16 | U32 | I32 | U64 | I64;
export type Num = Int | Flt | Dbl;
export type Simple = Str | Char | Int | Bool | Flt | Dbl;

export type ObjType = { t: TypeId.Obj; d: Of<Anonymous> };
export type SubType = { t: TypeId.Sub; p: string; d: Of<Anonymous> };
export type ArrType = { t: TypeId.Arr; d: Anonymous };
export type SetType = { t: TypeId.Set; d: Anonymous };
export type FastSetType = { t: TypeId.FastSet; d: Anonymous }; // Fast Set, used for std::unordered_set in C++
export type MapType = { t: TypeId.Map; k: Anonymous; v: Anonymous };
export type FastMapType = { t: TypeId.FastMap; k: Anonymous; v: Anonymous }; // Fast Map, used for std::unordered_map in C++
export type TupType = { t: TypeId.Tup; l: Anonymous[] };
export type RefType = { t: TypeId.Ref; r: string }; // Reference type, Points at a type by name

export type Enum = { t: TypeId.Enum; u: Int | I; v: string[] };
export type NEnum = { t: TypeId.NEnum; u: Int | I; v: Of<number> };
export type SEnum = { t: TypeId.SEnum; v: Of<string> };

export type EnumType = Enum | NEnum | SEnum;
// Objects and Enumeratoins have to be named, so they can't be used in anonymous context
export type Anonymous =
  | Simple
  | RefType
  | ArrType
  | SetType
  | FastSetType
  | MapType
  | FastMapType
  | TupType;
export type Types = Anonymous | ObjType | SubType | EnumType | I;
export type NamedTypes = ObjType | SubType | EnumType;

export const str = (): Str => TypeId.Str;
export const chr = (): Char => TypeId.Char;
export const u8 = (): U8 => TypeId.U8;
export const i8 = (): I8 => TypeId.I8;
export const u16 = (): U16 => TypeId.U16;
export const i16 = (): I16 => TypeId.I16;
export const u32 = (): U32 => TypeId.U32;
export const i32 = (): I32 => TypeId.I32;
export const u64 = (): U64 => TypeId.U64;
export const i64 = (): I64 => TypeId.I64;
export const num = (): I => TypeId.I; // This is just used for Enum types
export const bool = (): Bool => TypeId.Bool;
export const flt = (): Flt => TypeId.Flt;
export const dbl = (): Dbl => TypeId.Dbl;
export const arr = (d: Anonymous): ArrType => ({ t: TypeId.Arr, d });
export const set = (d: Anonymous): SetType => ({ t: TypeId.Set, d });
export const fset = (d: Anonymous): FastSetType => ({ t: TypeId.FastSet, d });
export const map = (k: Anonymous, v: Anonymous): MapType => ({
  t: TypeId.Map,
  k,
  v,
});
export const fmap = (k: Anonymous, v: Anonymous): FastMapType => ({
  t: TypeId.FastMap,
  k,
  v,
});
export const tup = (...l: Anonymous[]): TupType => ({ t: TypeId.Tup, l });
export const obj = (d: Of<Anonymous>): ObjType => ({ t: TypeId.Obj, d });
export const sub = (p: string, d: Of<Anonymous>): SubType => ({
  t: TypeId.Sub,
  p,
  d,
});
export const ref = (r: string): RefType => ({ t: TypeId.Ref, r });
export const enum_lst = (u: Int | I, v: string[]): Enum => ({
  t: TypeId.Enum,
  u,
  v,
});
export const enum_num = (u: Int | I, v: Of<number>): NEnum => ({
  t: TypeId.NEnum,
  u,
  v,
});
export const enum_str = (v: Of<string>): SEnum => ({ t: TypeId.SEnum, v });

// Type guards for the IDL types
export function isStringType(x: Types): x is Str {
  return x === TypeId.Str;
}
export function isCharType(x: Types): x is Char {
  return x === TypeId.Char;
}
export function isU8Type(x: Types): x is U8 {
  return x === TypeId.U8;
}
export function isI8Type(x: Types): x is I8 {
  return x === TypeId.I8;
}
export function isU16Type(x: Types): x is U16 {
  return x === TypeId.U16;
}
export function isI16Type(x: Types): x is I16 {
  return x === TypeId.I16;
}
export function isU32Type(x: Types): x is U32 {
  return x === TypeId.U32;
}
export function isI32Type(x: Types): x is I32 {
  return x === TypeId.I32;
}
export function isU64Type(x: Types): x is U64 {
  return x === TypeId.U64;
}
export function isI64Type(x: Types): x is I64 {
  return x === TypeId.I64;
}
export function isPlainIntEnumType(x: Types): x is I {
  return x === TypeId.I;
}
export function isBoolType(x: Types): x is Bool {
  return x === TypeId.Bool;
}
export function isFloatType(x: Types): x is Flt {
  return x === TypeId.Flt;
}
export function isDoubleType(x: Types): x is Dbl {
  return x === TypeId.Dbl;
}
export function isSimpleType(x: Types): x is Simple {
  return isString(x) && x.length === 1 && 'sc01234567bfd'.indexOf(x) >= 0;
}
export function isRefType(x: Types): x is RefType {
  return isObjectNonNull(x) && x.t === TypeId.Ref;
}
export function isArrayType(x: Types): x is ArrType {
  return isObjectNonNull(x) && x.t === TypeId.Arr;
}
export function isSetType(x: Types): x is SetType {
  return isObjectNonNull(x) && x.t === TypeId.Set;
}
export function isFastSetType(x: Types): x is FastSetType {
  return isObjectNonNull(x) && x.t === TypeId.FastSet;
}
export function isMapType(x: Types): x is MapType {
  return isObjectNonNull(x) && x.t === TypeId.Map;
}
export function isFastMapType(x: Types): x is FastMapType {
  return isObjectNonNull(x) && x.t === TypeId.FastMap;
}
export function isTupleType(x: Types): x is TupType {
  return isObjectNonNull(x) && x.t === TypeId.Tup;
}
export function isObjectType(x: Types): x is ObjType {
  return isObjectNonNull(x) && x.t === TypeId.Obj;
}
export function isSubType(x: Types): x is SubType {
  return isObjectNonNull(x) && x.t === TypeId.Sub;
}
export function isPlainEnumType(x: Types): x is Enum {
  return isObjectNonNull(x) && x.t === TypeId.Enum;
}
export function isNumericEnumType(x: Types): x is NEnum {
  return isObjectNonNull(x) && x.t === TypeId.NEnum;
}
export function isStringEnumType(x: Types): x is SEnum {
  return isObjectNonNull(x) && x.t === TypeId.SEnum;
}
export function isEnumType(x: Types): x is EnumType {
  return (
    isObjectNonNull(x) &&
    (x.t === TypeId.NEnum || x.t === TypeId.SEnum || x.t === TypeId.Enum)
  );
}
export function isNamedType(x: Types): x is ObjType | SubType | EnumType {
  return isEnumType(x) || isSubType(x) || isObjectType(x);
}
export function isAnonymousType(x: Types): x is Anonymous {
  return !isNamedType(x);
}

export type SymbolList = Record<string, Types>;

// An example of how to define types using the IDL:
/*
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
*/
