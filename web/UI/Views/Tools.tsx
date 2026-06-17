import { Switch } from '@fluentui/react-components';
import { Expandable } from '@freik/fluent9-tools';
import { ReactElement, useCallback, useState } from 'react';
import { TranscoderConfiguration } from './Tools/Transcoder';
import './styles/Tools.css';

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
  const fn = useCallback(() => setChecked(!checked), [use]);
  return (
    <Switch
      disabled={!!disabled}
      checked={checked}
      onClick={fn}
      label={label}
    />
  );
}
