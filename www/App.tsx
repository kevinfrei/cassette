import './index.css';
import { APITester } from './APITester';

import logo from './logo.svg';
import reactLogo from './react.svg';
import { Button } from '@mantine/core';

export function App(): React.JSX.Element {
  return (
    <div className="app">
      <div className="logo-container">
        <img src={logo} alt="Bun Logo" className="logo bun-logo" />
        <img src={reactLogo} alt="React Logo" className="logo react-logo" />
      </div>
      <Button>Test Button</Button>
      <h1>Bun + React</h1>
      <p>
        Edit <code>src/App.tsx</code> and save to test HMR
        <audio autoPlay={true} src="/tune/file.m4a" controls={true} />
      </p>
      <APITester />
    </div>
  );
}

export default App;
