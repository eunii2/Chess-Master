import styled from 'styled-components';

export const GameStartContainer = styled.div`
  display: flex;
  justify-content: space-between;
  padding: 2rem;
  height: 100vh;
  background: #f0f4f8;
`;

export const PlayerContainer = styled.div`
  width: 20%;
  padding: 1.5rem;
  border-radius: 1rem;
  background: ${props => (props.isActive ? '#3b82f6' : 'white')};
  color: ${props => (props.isActive ? 'white' : '#1e293b')};
  box-shadow: 0 8px 16px rgba(0, 0, 0, 0.1);
  transition: background 0.3s ease;
`;

export const ChessBoardContainer = styled.div`
  width: 50%;
  display: flex;
  justify-content: center;
  align-items: center;
  background: #e2e8f0;
  padding: 1.5rem;
  border-radius: 1rem;
  box-shadow: 0 8px 16px rgba(0, 0, 0, 0.1);
`;

export const ChatContainer = styled.div`
  width: 20%;
  padding: 1.5rem;
  background: white;
  border-radius: 1rem;
  box-shadow: 0 8px 16px rgba(0, 0, 0, 0.1);
`;

export const ChatBox = styled.div`
  height: 70%;
  overflow-y: auto;
  padding: 1rem;
  border: 1px solid #e2e8f0;
  margin-bottom: 1rem;
  border-radius: 0.5rem;
`;

export const MessageInput = styled.input`
  width: calc(100% - 2rem);
  padding: 0.75rem;
  border: 1px solid #e2e8f0;
  border-radius: 0.5rem;
  margin-bottom: 1rem;
`;

export const SendButton = styled.button`
  width: 100%;
  padding: 0.75rem;
  background: #3498db;
  color: white;
  border: none;
  border-radius: 0.5rem;
  font-weight: 600;
  cursor: pointer;
  transition: background 0.3s ease;

  &:hover {
    background: #2980b9;
  }
`;
