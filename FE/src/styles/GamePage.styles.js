import styled from 'styled-components';

export const GameContainer = styled.div`
  min-height: 100vh;
  background: linear-gradient(to bottom, #f0f4f8, #e2e8f0);
  padding: 2rem;
  display: flex;
  gap: 2rem;
  justify-content: center;
  align-items: center;
`;

export const ChessboardContainer = styled.div`
  flex: 0 0 auto;
  width: 800px;
  aspect-ratio: 1;
  display: flex;
  flex-direction: column;
  gap: 1rem;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
`;

export const SideContainer = styled.div`
  flex: 0 0 400px;
  display: flex;
  flex-direction: column;
  gap: 1.5rem;
  height: 800px;
`;

export const PlayerCard = styled.div`
  background: white;
  border-radius: 16px;
  padding: 1.25rem;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
  display: grid;
  grid-template-columns: auto 1fr auto;
  align-items: center;
  gap: 1rem;
  border: 2px solid ${props => props.$isCurrentPlayer ? '#3b82f6' : '#e2e8f0'};

  &::before {
    content: '';
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: ${props => props.$isCurrentPlayer ? '#22c55e' : '#94a3b8'};
    transition: background-color 0.2s ease;
  }
`;

export const PlayerAvatar = styled.div`
  width: 40px;
  height: 40px;
  border-radius: 50%;
  background: #f1f5f9;
`;

export const PlayerName = styled.span`
  font-size: 1.1rem;
  font-weight: 600;
  color: #1e293b;
`;

export const ChatContainer = styled.div`
  flex: 1;
  background: white;
  border-radius: 16px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
  display: flex;
  flex-direction: column;
  overflow: hidden;
`;

export const ChatHeader = styled.div`
  padding: 0.8rem;
  font-size: 1.4rem;
  border-bottom: 2px solid #e2e8f0;
  font-weight: 600;
  color: #1e293b;
  text-align: center;
`;

export const ChatMessages = styled.div`
  flex: 1;
  padding: 1.25rem;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 0.5rem;

  &::-webkit-scrollbar {
    width: 6px;
  }

  &::-webkit-scrollbar-track {
    background: transparent;
  }

  &::-webkit-scrollbar-thumb {
    background-color: rgba(0, 0, 0, 0.1);
    border-radius: 3px;
  }

  scrollbar-width: thin;
  scrollbar-color: rgba(0, 0, 0, 0.1) transparent;
`;

export const MessageContainer = styled.div`
  display: flex;
  flex-direction: column;
  align-items: ${props => props.$isMe ? 'flex-end' : 'flex-start'};
  gap: 0.15rem;
`;

export const Username = styled.span`
  font-size: 0.75rem;
  color: #666;
  margin: ${props => props.$isMe ? '0 0.5rem 0 0' : '0 0 0 0.5rem'};
`;

export const MessageBubble = styled.div`
  background: ${props => props.$isMe ? '#3b82f6' : '#f1f5f9'};
  color: ${props => props.$isMe ? 'white' : 'black'};
  padding: 0.75rem 1rem;
  border-radius: 16px;
  max-width: 70%;
  word-break: break-word;
`;

export const ChatInput = styled.div`
  padding: 1rem;
  border-top: 2px solid #e2e8f0;
  display: flex;
  gap: 0.75rem;

  input {
    flex: 1;
    padding: 0.75rem 1rem;
    border: 2px solid #e2e8f0;
    border-radius: 8px;
    font-size: 0.95rem;
    transition: all 0.2s;

    &:focus {
      outline: none;
      border-color: #3b82f6;
      box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
    }
  }

  button {
    padding: 0.75rem 1.5rem;
    background: #3b82f6;
    color: white;
    border: none;
    border-radius: 8px;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s;

    &:hover {
      background: #2563eb;
      transform: translateY(-1px);
      box-shadow: 0 4px 12px rgba(37, 99, 235, 0.15);
    }
  }
`;

export const SurrenderButton = styled.button`
  padding: 0.5rem 1rem;
  background: #e11d48;
  color: white;
  border: none;
  border-radius: 8px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;
  font-size: 0.9rem;

  &:hover {
    background: #be123c;
    transform: translateY(-1px);
    box-shadow: 0 4px 12px rgba(225, 29, 72, 0.15);
  }
`;

export const PlayerInfo = styled.div`
  display: flex;
  align-items: center;
  gap: 1rem;
  min-width: 200px;
`;

export const GameOverlay = styled.div`
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.6);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
`;

export const GameOverModal = styled.div`
  background: white;
  padding: 2rem;
  border-radius: 10px;
  text-align: center;
  width: 400px;
`;

export const GameOverTitle = styled.h1`
  font-size: 2.5rem;
  margin-bottom: 0.5rem;
  color: #000000;
`;

export const GameOverMessage = styled.p`
  font-size: 1.2rem;
  margin-bottom: 2rem;
  color: #666;
`;

export const ButtonContainer = styled.div`
  display: flex;
  justify-content: center;
  gap: 1rem;
`;

export const Button = styled.button`
  padding: 0.8rem 1.5rem;
  border: none;
  border-radius: 5px;
  font-size: 1rem;
  cursor: pointer;
  transition: opacity 0.2s;

  &:hover {
    opacity: 0.8;
  }
`;

export const HomeButton = styled(Button)`
  background-color: #f44336;
  color: white;
`;

export const RecordButton = styled(Button)`
  background-color: #2196F3;
  color: white;
`;