import {
  ArrType,
  Enum,
  isArrayType,
  isMapType,
  isNumericEnumType,
  isObjectType,
  isPlainEnumType,
  isSetType,
  isStringEnumType,
  isTupleType,
  MapType,
  NEnum,
  ObjType,
  SEnum,
  SetType,
  TupType,
  Types,
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
};

function forElement(emit: Emitter, adt: Types): EmitItem<any> {
  // Returns the type emitter for the given ADT
  if (isObjectType(adt)) {
    return emit.types.objType;
  } else if (isArrayType(adt)) {
    return emit.types.arrType;
  } else if (isSetType(adt)) {
    return emit.types.setType;
  } else if (isMapType(adt)) {
    return emit.types.mapType;
  } else if (isTupleType(adt)) {
    return emit.types.tupType;
  } else if (isPlainEnumType(adt)) {
    return emit.types.enumType;
  } else if (isNumericEnumType(adt)) {
    return emit.types.numEnumType;
  } else if (isStringEnumType(adt)) {
    return emit.types.strEnumType;
  }
  throw new Error(`Unknown ADT type: ${adt.t}`);
}

async function emitCode(
  emitter: Emitter,
  fileName: string,
  items: Record<string, Types>,
): Promise<void> {
  const file = Bun.file(fileName);
  if (await file.exists()) {
    await file.delete();
  }
  const writer = file.writer();
  await emitter.header(writer);
  for (const [name, item] of Object.entries(items)) {
    // Emit the C++ code for each SharedConstants item, either numeric or string type
    const itemEmitter = forElement(emitter, item);
    await itemEmitter(writer, name, item);
  }
  await emitter.footer(writer);
  await writer.end();
}

export type FileGenerator = (
  fileName: string,
  items: Record<string, Types>,
) => Promise<void>;

export function MakeGenerator(emitter: Emitter): FileGenerator {
  return async (fileName: string, items: Record<string, Types>) =>
    await emitCode(emitter, fileName, items);
}
