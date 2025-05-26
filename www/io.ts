import { typecheck } from '@freik/typechk';

export function Post(name: string): void {
  fetch('/' + name, { method: 'PUT' }).catch(console.error);
}

export async function RawGet(
  endpoint: string,
  ...args: string[]
): Promise<string | undefined> {
  try {
    const response = await fetch(endpoint, {
      method: 'GET',
    });
    if (response.ok) {
      return await response.text();
    }
  } catch (err) {
    console.error(
      `Failed to fetch ${endpoint} with args: ${args.join(', ')}`,
      err,
    );
    throw err;
  }
  return undefined;
}

export async function Get(
  endpoint: string,
  ...args: string[]
): Promise<unknown> {
  try {
    const response = await fetch(['/api', endpoint, ...args].join('/'), {
      method: 'GET',
    });
    if (response.ok) {
      return await response.json();
    }
  } catch (err) {
    return err;
  }
  return { error: `Failed to fetch ${endpoint} with args: ${args.join(', ')}` };
}

export async function GetAs<T>(
  validator: typecheck<T>,
  endpoint: string,
  ...args: string[]
): Promise<T | undefined> {
  const res = await Get(endpoint, ...args);
  return validator(res) ? res : undefined;
}
