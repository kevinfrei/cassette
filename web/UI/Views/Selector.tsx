import { useAtomValue } from 'jotai';
import { CSSProperties, ReactElement, useState } from 'react';
import { CurrentView } from '../../Shared/CommonTypes';
import { curViewState } from '../../State/CurrentView';
import { SettingsView } from './Settings';
import { ToolsView } from './Tools';

import './styles/Selector.css';

function ignore(view: CurrentView): boolean {
  return view === CurrentView.settings || view === CurrentView.tools;
}

function include(
  val: CurrentView,
  theSet: Set<CurrentView>,
  which: CurrentView,
): boolean {
  return theSet.has(val) || which === val;
}

export function ViewSelector(): ReactElement {
  const which = useAtomValue(curViewState);
  const [rendered, setRendered] = useState(new Set<CurrentView>([which]));
  // Let's see if I can speed this up a bit by not trying to render everything
  // the first time
  const vis = (v: CurrentView): CSSProperties =>
    which === v ? {} : { visibility: 'hidden' };
  const contents: [CurrentView, ReactElement][] = [];
  if (!rendered.has(which) && !ignore(which)) {
    const newrendered = new Set<CurrentView>([which, ...rendered]);
    setRendered(newrendered);
    // We still need to do a full render, because otherwise elements get
    // deleted and recreated, and that's probably bad, right?
  }
  if (include(CurrentView.settings, rendered, which)) {
    contents.push([CurrentView.settings, <SettingsView />]);
  }
  if (include(CurrentView.tools, rendered, which)) {
    contents.push([CurrentView.tools, <ToolsView />]);
  }
  return (
    <>
      {contents.map(([view, elem]) => (
        <div key={view} className="current-view" style={vis(view)}>
          {elem}
        </div>
      ))}
    </>
  );
}
