import styled from 'styled-components';

export const HistoryContainer = styled.div`
  min-height: 100vh;
  background: linear-gradient(to bottom, #f0f4f8, #e2e8f0);
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 2rem;
  padding: 4rem;
`;

export const ChessboardSection = styled.div`
  flex: 0 0 auto;
  width: 800px;  // 체스판 크기 더 증가
  height: 800px;  // 정사각형 유지
  display: flex;
  justify-content: center;
  align-items: center;
`;

export const SidebarSection = styled.div`
  background: white;
  border-radius: 16px;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.08);
  padding: 2rem;
  width: 400px;  // 사이드바 크기 더 증가
  height: 800px;  // 체스보드와 동일한 높이
  display: flex;
  flex-direction: column;
  gap: 1.5rem;

  @media (max-width: 1024px) {
    width: 100%;
    height: auto;
  }
`;

export const ResultSection = styled.div`
  margin-bottom: 1rem; 

  h2 {
    font-size: 1.5rem;
    color: #1e293b;
    margin-bottom: 1rem;
    font-weight: 700;
  }
  
  p {
    color: #64748b;
    font-size: 1.1rem;
  }
`;

export const MovesSection = styled.div`
  flex: 1;  // 남은 공간을 모두 차지하도록
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 1rem;

  &::-webkit-scrollbar {
    width: 8px;
  }

  &::-webkit-scrollbar-track {
    background: #f1f5f9;
    border-radius: 4px;
  }

  &::-webkit-scrollbar-thumb {
    background: #94a3b8;
    border-radius: 4px;
  }
`;

export const MoveItem = styled.div`
  padding: 1rem;
  background: ${props => props.$isActive ? '#f1f5f9' : 'white'};
  border: 1px solid #e2e8f0;
  border-radius: 8px;
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
  transition: all 0.2s;

  &:hover {
    background: #f8fafc;
  }

  > div {
    padding: 0.5rem;
    border-radius: 4px;
    cursor: pointer;
    
    &:hover {
      background: #e2e8f0;
    }
  }
`;

export const ControlSection = styled.div`
  display: flex;
  justify-content: center;
  gap: 6rem;
  padding-top: 1rem;
  border-top: 2px solid #e2e8f0;
`;

export const ControlButton = styled.button`
  padding: 0.75rem 1.5rem;
  background: #3b82f6;
  color: white;
  border: none;
  border-radius: 8px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;

  &:hover:not(:disabled) {
    background: #2563eb;
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(59, 130, 246, 0.3);
  }

  &:disabled {
    background: #94a3b8;
    cursor: not-allowed;
  }
`;

export const ChessBoard = styled.div`
  display: grid;
  grid-template-rows: repeat(8, 1fr);
  gap: 1px;
  background-color: #ddd;
  padding: 10px;
  border-radius: 8px;
  
  > div {
    display: grid;
    grid-template-columns: repeat(8, 1fr);
    gap: 1px;
    
    > span {
      aspect-ratio: 1;
      display: flex;
      align-items: center;
      justify-content: center;
      background-color: white;
      font-size: 24px;
    }
  }
`; 

export const ResultHeader = styled.div`
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 1rem;
`;

export const TitleGroup = styled.div`
  h2 {
    font-size: 1.5rem;
    font-weight: 600;
    color: #1e293b;
    margin-bottom: 0.5rem;
  }
`;

export const ResultText = styled.p`
  color: #64748b;
  font-size: 1rem;
`;

export const HomeButton = styled.button`
  background: none;
  border: none;
  cursor: pointer;
  padding: 0.5rem;
  display: flex;
  align-items: center;
  justify-content: center;
`;

export const HomeIcon = styled.img`
  width: 24px;
  height: 24px;
  transition: transform 0.2s;

  &:hover {
    transform: scale(1.1);
  }
`;

