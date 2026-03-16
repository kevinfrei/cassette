import { Expandable } from '@freik/fluentui-tools';
import { TranscoderConfiguration } from './Tools/Transcoder';

import { ReactElement } from 'react';
import './styles/Tools.css';

export function ToolsView(): ReactElement {
  return (
    <div className="tools-view">
      <Expandable separator label="Transcoder" defaultShow>
        <TranscoderConfiguration />
      </Expandable>
    </div>
  );
}
