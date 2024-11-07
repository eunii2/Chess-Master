import styled from 'styled-components';

export const RoomContainer = styled.div`
  min-height: 100vh;
  background: linear-gradient(to bottom, #f0f4f8, #e2e8f0);
  padding: 2rem 1rem;
`;

export const RoomContent = styled.div`
  max-width: 1200px;
  margin: 0 auto;
  background: white;
  border-radius: 2rem;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.08);
  padding: 2rem;
  position: relative;
  
  @media (min-width: 768px) {
    padding: 3rem;
  }
`;

export const RoomHeader = styled.div`
  text-align: center;
  margin: 1rem 0 3rem;
  position: relative;
  padding-bottom: 2rem;
  border-bottom: 2px solid #e2e8f0;
`;

export const RoomTitle = styled.h1`
  font-size: 2.5rem;
  font-weight: 800;
  color: #1e293b;
  margin-bottom: 0.5rem;
  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
`;

export const RoomSubtitle = styled.p`
  color: #64748b;
  font-size: 1.1rem;
  font-weight: 500;
`;

export const PlayerSection = styled.div`
  display: grid;
  grid-template-columns: 1fr;
  gap: 1.5rem;
  margin: 2rem 0;
  
  @media (min-width: 768px) {
    grid-template-columns: repeat(2, 1fr);
    gap: 2rem;
  }
`;

export const Player = styled.div`
  background: ${props => props.waiting ? '#f8fafc' : 'white'};
  padding: 2rem;
  border-radius: 1.5rem;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.05);
  border: 2px solid ${props => props.waiting ? '#e2e8f0' : '#3b82f6'};
  position: relative;
  overflow: hidden;
  cursor: pointer;

  &::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    height: 4px;
    background: ${props => props.waiting ? '#94a3b8' : '#3b82f6'};
  }

  &:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.1);
  }
`;

export const PlayerName = styled.h2`
  font-size: 1.5rem;
  color: #1e293b;
  margin-bottom: 1rem;
  font-weight: 700;
  
  ${props => props.waiting && `
    color: #94a3b8;
  `}
`;

export const PlayerStatus = styled.div`
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  font-size: 0.875rem;
  font-weight: 600;
  padding: 0.5rem 1rem;
  border-radius: 2rem;
  color: ${props => props.isCreator ? '#2563eb' : '#059669'};
  background: ${props => props.isCreator ? '#eff6ff' : '#ecfdf5'};
  border: 1px solid ${props => props.isCreator ? '#60a5fa' : '#34d399'};

  &::before {
    content: '';
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: currentColor;
  }
`;

export const GameControls = styled.div`
  display: flex;
  justify-content: center;
  margin-top: 3rem;
  padding-top: 2rem;
  border-top: 2px solid #e2e8f0;
`;

export const StartButton = styled.button`
  width: 100%;
  max-width: 300px;
  padding: 1.25rem;
  background: ${props => props.disabled ? '#94a3b8' : '#3b82f6'};
  color: white;
  border: none;
  border-radius: 1rem;
  font-size: 1.25rem;
  font-weight: 700;
  cursor: ${props => props.disabled ? 'not-allowed' : 'pointer'};
  transition: all 0.2s;
  
  &:hover:not(:disabled) {
    background: #2563eb;
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(59, 130, 246, 0.3);
  }
`;

export const LeaveButton = styled.button`
  position: absolute;
  top: 2rem;
  left: 2rem;
  padding: 0.75rem 1.5rem;
  background: white;
  color: #e11d48;
  border: 2px solid #e11d48;
  border-radius: 0.75rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;

  &:hover {
    background: #e11d48;
    color: white;
    transform: translateY(-2px);
    box-shadow: 0 4px 8px rgba(225, 29, 72, 0.2);
  }
`;