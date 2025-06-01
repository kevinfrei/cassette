import { Expandable } from '@freik/fluentui-tools';
import { TranscoderConfiguration } from './Tools/Transcoder';

import './styles/Tools.css';
import { ReactElement } from 'react';

export function ToolsView(): ReactElement {
  return (
    <div className="tools-view">
      <Expandable separator label="Transcoder" defaultShow>
        <TranscoderConfiguration />
      </Expandable>
    </div>
  );
}
