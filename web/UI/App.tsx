import { Provider } from 'jotai';
import { ReactElement } from 'react';
import { KeepAlive } from '../KeepAlive';
import { getStore } from '../State/Storage';
import { WebSocketRegistrar } from '../Tools/ReactWebSocket';
import { Sidebar } from './Sidebar';
import { ViewSelector } from './Views/Selector';

import '../styles/App.css';

function TheActualApp(): ReactElement {
  return (
    <div id="app">
      <span id="left-column" />
      <span id="top-row" />
      <Sidebar />
      <ViewSelector />
    </div>
  );
}

export function App(): ReactElement {
  const store = getStore();
  return (
    <Provider store={store}>
      <WebSocketRegistrar />
      <KeepAlive />
      <TheActualApp />
    </Provider>
  );
}
