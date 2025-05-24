import './index.css';

import { APITester } from './APITester';

import logo from './logo.svg';
import reactLogo from './react.svg';
import { AppShell, Burger, Button, Grid } from '@mantine/core';
import { useDisclosure } from '@mantine/hooks';

export function App(): React.JSX.Element {
  const [opened, { toggle }] = useDisclosure();
  return (
    <AppShell layout="alt" padding="md">
      <AppShell.Header>Header</AppShell.Header>
      <AppShell.Navbar p="md">NavBar</AppShell.Navbar>
      <AppShell.Main>Main</AppShell.Main>
    </AppShell>
  );
}

/*
       <AppShell.Header>
        <Burger opened={opened} onClick={toggle} hiddenFrom="sm" size="sm" />
        <div>Logo</div>
      </AppShell.Header>

      <AppShell.Navbar p="md">Navbar</AppShell.Navbar>

      <AppShell.Main>
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
      </AppShell.Main>
*/

export default App;
