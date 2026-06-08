import { Expandable } from '@freik/fluentui-tools';
import { ReactElement, Suspense, useCallback, useMemo, useState } from 'react';

import { Switch } from '@fluentui/react-components';
import { ErrorBoundary } from '../../Tools/Utilities';
import { FolderChooser } from '../Dialogs/FolderChooser';
import './styles/Tools.css';
import { TranscoderConfiguration } from './Tools/Transcoder';

export function ToolsView(): ReactElement {
  const [selectedFolder, setSelectedFolder] = useState<string | null>(null);
  return (
    <div className="tools-view">
      <Expandable separator label="Transcoder" defaultShow>
        <TranscoderConfiguration />
      </Expandable>
    </div>
  );
}

export type ToggleSwitchProps = {
  label: string;
  use: [boolean, (v: boolean) => void];
  disabled?: boolean;
};
export function ToggleSwitch({
  label,
  use,
  disabled,
}: ToggleSwitchProps): ReactElement {
  const [checked, setChecked] = use;
  const fn = useCallback(() => setChecked(!checked), [checked, setChecked]);
  return (
    <Switch
      disabled={!!disabled}
      checked={checked}
      onClick={fn}
      label={label}
    />
  );
}
