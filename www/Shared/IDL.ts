// My silly IDL is actually just a set of functions that can be interpreted
// by a script to create types for the data structures.

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

export type Int = I | U8 | I8 | U16 | I16 | U32 | I32 | U64 | I64;
export type Num = Int | Flt | Dbl;
export type Atoms = Str | Char | Int | Bool | Flt | Dbl;

export type ObjType = { t: 'O'; d: Of<Types> };
export type ArrType = { t: 'A'; d: Types };
export type SetType = { t: 'S'; d: Types };
export type MapType = { t: 'M'; k: Types; v: Types };
export type TupType = { t: 'T'; l: Types[] };

export type Enum = { t: '#'; u: Int; v: string[] };
export type NEnum = { t: '%'; u: Int; v: Of<number> };
export type SEnum = { t: '$'; v: Of<string> };

export type Enums = Enum | NEnum | SEnum;

export type Types =
  | Atoms
  | Enums
  | ObjType
  | ArrType
  | SetType
  | MapType
  | TupType;

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
export const arr = (d: Types): ArrType => ({ t: 'A', d });
export const set = (d: Types): SetType => ({ t: 'S', d });
export const map = (k: Types, v: Types): MapType => ({ t: 'M', k, v });
export const tup = (...l: Types[]): TupType => ({ t: 'T', l });
export const obj = (d: Of<Types>): ObjType => ({ t: 'O', d });
export const enum_lst = (u: Int, v: string[]): Enum => ({ t: '#', u, v });
export const enum_num = (u: Int, v: Of<number>): NEnum => ({ t: '%', u, v });
export const enum_str = (v: Of<string>): SEnum => ({ t: '$', v });

/*

const ExampleEnum1_desc = enum_lst(u8(), ['one', 'two', 'three']);
const ExampleEnum2_desc = enum_num(num(), { one: 1, two: 2, tre: 3 });
const ExampleEnum3_desc = enum_str({ Red: 'red', Green: 'green', Blue: 'blue' });
const ExampleObjectType_desc = obj({
  name: str(),
  age: i32(),
  isActive: bool(),
  scores: arr(i32()),
  metadata: obj({
    author: str(),
    angle: dbl(),
  }),
  friends: set(str()),
  info: map(str(), tup(str(), str())),
  id: ExampleEnum1_desc,
});

const ExampleTupleType_desc = tup(str(), i32(), bool());
const ExampleArrayType_desc = arr(ExampleObjectType_desc);
const ExampleObjectTupleType_desc = tup(
  obj({ id: str(), data: ExampleTupleType_desc }),
  obj({ name: str(), value: i32() }),
);
*/
