import {
  Aggregate,
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

function forElement(emit: Emitter, adt: Aggregate): EmitItem<any> {
  // Returns the type emitter for the given ADT
  switch (adt.t) {
    case 'O':
      return emit.types.objType;
    case 'A':
      return emit.types.arrType;
    case 'S':
      return emit.types.setType;
    case 'M':
      return emit.types.mapType;
    case 'T':
      return emit.types.tupType;
    case '#':
      return emit.types.enumType;
    case '%':
      return emit.types.numEnumType;
    case '$':
      return emit.types.strEnumType;
    default:
      throw new Error(`Unknown ADT type: ${adt.t}`);
  }
}

async function emitCode(
  emitter: Emitter,
  fileName: string,
  items: Record<string, Aggregate>,
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
  items: Record<string, Aggregate>,
) => Promise<void>;

export function MakeGenerator(emitter: Emitter): FileGenerator {
  return async (fileName: string, items: Record<string, Aggregate>) =>
    await emitCode(emitter, fileName, items);
}
