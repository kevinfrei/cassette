import { ISearchBox } from '@fluentui/react';

interface AppWindow extends Window {
  searchBox?: ISearchBox | null;
  // db: FlatAudioDatabase;
}
declare let window: AppWindow;

export function SetSearch(searchBox: ISearchBox | null): void {
  window.searchBox = searchBox;
}

export function FocusSearch(): boolean {
  if (window.searchBox) {
    window.searchBox.focus();
    return true;
  }
  return false;
}
