import { Expandable } from '@freik/fluentui-tools';
import { ReactElement, Suspense, useState } from 'react';

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
