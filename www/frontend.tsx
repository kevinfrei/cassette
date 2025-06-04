/**
 * This file is the entry point for the React app, it sets up the root
 * element and renders the App component to the DOM.
 *
 * It is included in `src/index.html`.
 */

import { createRoot } from 'react-dom/client';
import { StrictMode } from 'react';
import { App } from './App';
import { Provider } from 'jotai';
import { KeepAlive } from './KeepAlive';
import { RealTimeUpdates } from './Tools/ReactWebSocket';

const elem = document.getElementById('root')!;
const theActualApp = <App />;

const app = (
  <StrictMode>
    <Provider>
      <RealTimeUpdates />
      <KeepAlive />
    </Provider>
  </StrictMode>
);

if (import.meta.hot) {
  // With hot module reloading, `import.meta.hot.data` is persisted.
  const root = (import.meta.hot.data.root ??= createRoot(elem));
  root.render(app);
  console.log(theActualApp);
} else {
  // The hot module reloading API is not available in production.
  createRoot(elem).render(app);
}

// This is the thing to tell the server to quit when the page is closed
window.addEventListener('unload', () => {
  fetch('/quit', { method: 'GET' });
});
