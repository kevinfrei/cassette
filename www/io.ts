export function Post(name: string): void {
  fetch('/' + name, { method: 'PUT' }).catch(console.error);
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
}

export async function GetAs<T>(
  validator: (obj: unknown) => obj is T,
  endpoint: string,
  ...args: string[]
): Promise<T | undefined> {
  const res = await Get(endpoint, ...args);
  return validator(res) ? res : undefined;
}
