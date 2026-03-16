import { Expandable } from '@freik/fluentui-tools';
import { ReactElement } from 'react';
import './styles/Tools.css';
import { TranscoderConfiguration } from './Tools/Transcoder';

export function ToolsView(): ReactElement {
  return (
    <div className="tools-view">
      <Expandable separator label="Transcoder" defaultShow>
        <TranscoderConfiguration />
      </Expandable>
    </div>
  );
}
