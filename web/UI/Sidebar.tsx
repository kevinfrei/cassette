import { hasStrField, isObjectNonNull } from '@freik/typechk';
import { useAtom } from 'jotai';
import { ReactElement } from 'react';
import { st } from '../Constants';
import { CurrentView, Keys, StrId } from '../Shared/CommonTypes';
import { curViewState } from '../State/CurrentView';
import { GetHelperText } from '../Utils';

import '../styles/Sidebar.css';

type ViewEntry = {
  name: CurrentView;
  title: StrId;
  accelerator: Keys;
};

const mkEntry = (name: CurrentView, title: StrId, accelerator: Keys) => ({
  name,
  title,
  accelerator,
});

const views: (ViewEntry | null)[] = [
  mkEntry(CurrentView.tools, StrId.ViewTools, Keys.Tools),
  null,
  mkEntry(CurrentView.settings, StrId.ViewSettings, Keys.Settings),
];

function getEntry(
  curView: CurrentView,
  setCurView: (newView: CurrentView) => Promise<void> | void,
  view: ViewEntry | null,
  index: number,
) {
  if (!view) {
    return <hr key={index} />;
  }
  const extra = curView === view.name ? ' sidebar-selected' : '';
  return (
    <div
      key={index}
      className={`sidebar-container${extra}`}
      onClick={() => void setCurView(view.name)}
      title={GetHelperText(view.accelerator)}>
      <span className="sidebar-icon" id={st(view.title).replace(/ /g, '-')}>
        &nbsp;
      </span>
      <p className={`sidebar-text${extra}`}>{st(view.title)}</p>
    </div>
  );
}

// This is used to prevent responding to global keypresses when the input box
// is active
export function isSearchBox(target: EventTarget | null): boolean {
  return (
    isObjectNonNull(target) &&
    hasStrField(target, 'type') &&
    hasStrField(target, 'tagName') &&
    hasStrField(target, 'placeholder') &&
    target.type === 'text' &&
    target.tagName === 'INPUT' &&
    target.placeholder === 'Search'
  );
}

export function Sidebar(): ReactElement {
  const [curView, setCurView] = useAtom(curViewState);
  return (
    <div id="sidebar">
      {views.map((ve, index) => getEntry(curView, setCurView, ve, index))}
    </div>
  );
}
