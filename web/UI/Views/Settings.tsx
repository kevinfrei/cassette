import { DefaultButton } from '@fluentui/react';
import { Expandable, StateToggle } from '@freik/fluentui-tools';
import { isBoolean } from '@freik/typechk';
import { CSSProperties, ReactElement } from 'react';
import { IpcCall, StorageId } from '../../Shared/CommonTypes';
import { useJotaiBoolState } from '../../State/Hooks';
import { atomWithMainStorage } from '../../State/Storage';
import { PostMain } from '../../Tools/Ipc';

import './styles/Settings.css';

const btnWidth: CSSProperties = { width: '155px', padding: 0 };

const setting = atomWithMainStorage(StorageId.SettingValue, true, isBoolean);

function ArticleSorting(): ReactElement {
  const articles = useJotaiBoolState(setting);
  return <StateToggle label="Ignore articles when sorting" state={articles} />;
}

export function SettingsView(): ReactElement {
  return (
    <div className="settings-view">
      <Expandable separator label="Some Things" defaultShow={true}>
        <Expandable
          indent={30}
          separator
          label="Ignore filters"
          defaultShow={false}>
          <div>Something here</div>
        </Expandable>
      </Expandable>
      <Expandable separator label="Sorting & Filtering" defaultShow={true}>
        <ArticleSorting />
      </Expandable>
      <Expandable separator label="Some Things" defaultShow={true}>
        <>
          <DefaultButton
            text="Call a Thing"
            style={btnWidth}
            onClick={() => PostMain(IpcCall.RestoreWindow)}
          />
          &nbsp;
          <DefaultButton
            text="Clear another Thing"
            style={btnWidth}
            onClick={() => PostMain(IpcCall.MaximizeWindow)}
          />
        </>
      </Expandable>
    </div>
  );
}
