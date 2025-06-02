import {
  ArrType,
  Enum,
  MapType,
  NEnum,
  ObjType,
  SEnum,
  SetType,
  TupType,
} from '../../www/Shared/IDL';

export type EmitItem<T> = (
  writer: Bun.FileSink,
  name: string,
  item: T,
) => Promise<void>;

export type Emitter = {
  header: (writer: Bun.FileSink) => Promise<void>;
  footer: (writer: Bun.FileSink) => Promise<void>;
  types: {
    objType: EmitItem<ObjType>;
    arrType: EmitItem<ArrType>;
    setType: EmitItem<SetType>;
    mapType: EmitItem<MapType>;
    tupType: EmitItem<TupType>;
    enumType: EmitItem<Enum>;
    numEnumType: EmitItem<NEnum>;
    strEnumType: EmitItem<SEnum>;
  };
  fields: {};
};
