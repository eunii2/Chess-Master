import styled from 'styled-components';

export const HistoryContainer = styled.div`
  padding: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
`;

export const ControlsContainer = styled.div`
  display: flex;
  align-items: center;
  gap: 20px;
  margin-bottom: 20px;
`;

export const Button = styled.button`
  padding: 10px 20px;
  font-size: 18px;
  cursor: pointer;
  &:disabled {
    opacity: 0.5;
    cursor: not-allowed;
  }
`;

export const MoveInfo = styled.div`
  min-width: 300px;
  text-align: center;
`;

export const BoardDisplay = styled.div`
  font-family: monospace;
  white-space: pre;
`;

export const BoardRow = styled.div`
  display: flex;
`;

export const BoardCell = styled.span`
  width: 20px;
  height: 20px;
  display: inline-block;
  text-align: center;
`; 