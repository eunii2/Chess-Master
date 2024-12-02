import styled, { keyframes } from 'styled-components';

const fadeIn = keyframes`
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
`;

export const PageContainer = styled.div`
    min-height: 100vh;
    background: linear-gradient(to bottom, #f0f4f8, #e2e8f0);
    padding-bottom: 2rem;
`;

export const NavBar = styled.nav`
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 1.5rem 6rem;
    background: #ffffffdd;
    backdrop-filter: blur(10px);
    box-shadow: 0 1px 4px rgba(0, 0, 0, 0.1);
    position: sticky;
    top: 0;
    z-index: 100;
`;

export const Logo = styled.div`
    display: flex;
    align-items: center;
    gap: 0.75rem;
    font-size: 1.75rem;
    font-weight: 700;
    color: #1e293b;
    cursor: pointer;
    transition: transform 0.2s;

    &:hover {
        transform: scale(1.03);
    }
`;

export const LogoText = styled.span`
    background: linear-gradient(135deg, #1e293b 0%, #64748b 100%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    font-size: 2rem;
`;

export const LogoutButton = styled.button`
    padding: 0.625rem 1.25rem;
    border: 2px solid #e11d48;
    border-radius: 10px;
    color: #e11d48;
    background: transparent;
    font-weight: 500;
    cursor: pointer;
    transition: all 0.2s;
    margin-left: 0.5rem;

    &:hover {
        background: #e11d48;
        color: white;
        transform: translateY(-1px);
        box-shadow: 0 4px 8px rgba(225, 29, 72, 0.15);
    }
`;

export const Content = styled.main`
    margin: 2rem 6rem;
    animation: ${fadeIn} 0.6s ease-out;
`;

export const Header = styled.div`
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 2rem;
`;

export const Title = styled.h1`
    font-size: 2.25rem;
    background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    font-size : 2rem;
`;

export const CreateRoomButton = styled.button`
    padding: 0.875rem 1.75rem;
    background: linear-gradient(135deg, #3b82f6 0%, #2563eb 100%);
    color: white;
    border: none;
    border-radius: 12px;
    font-weight: 600;
    font-size: 1rem;
    cursor: pointer;
    transition: all 0.2s;
    box-shadow: 0 4px 10px rgba(37, 99, 235, 0.15);

    &:hover {
        transform: translateY(-2px);
        box-shadow: 0 6px 12px rgba(37, 99, 235, 0.25);
    }
`;

export const RoomGrid = styled.div`
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
    gap: 2rem;
`;

export const RoomCard = styled.div`
    background: #ffffff;
    border-radius: 16px;
    padding: 1.5rem;
    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.05);
    display: flex;
    flex-direction: column;
    gap: 1.25rem;
    transition: all 0.3s;
    border: 1px solid rgba(0, 0, 0, 0.08);
    opacity: ${props => props.joined ? 0.8 : 1};

    &:hover {
        transform: translateY(-4px);
        box-shadow: 0 8px 16px rgba(0, 0, 0, 0.1);
    }
`;

export const RoomName = styled.h2`
    font-size: 1.3rem;
    color: #1e293b;
    font-weight: 600;
    margin: 0;
`;

export const RoomStatus = styled.span`
    color: ${props => props.joined ? '#e11d48' : '#10b981'};
    font-size: 0.875rem;
    font-weight: 600;
    display: flex;
    align-items: center;
    gap: 0.5rem;

    &:before {
        content: '';
        display: inline-block;
        width: 8px;
        height: 8px;
        border-radius: 50%;
        background: ${props => props.joined ? '#e11d48' : '#10b981'};
    }
`;

export const JoinButton = styled.button`
    padding: 0.75rem;
    background: ${props => props.disabled ? '#94a3b8' : '#059669'};
    color: white;
    border: none;
    border-radius: 8px;
    font-weight: 600;
    cursor: ${props => props.disabled ? 'not-allowed' : 'pointer'};
    transition: all 0.2s;
    font-size: 0.9rem;

    &:hover:not(:disabled) {
        background: #047857;
        transform: translateY(-1px);
    }
`;

export const LoadingMessage = styled.div`
    text-align: center;
    padding: 2rem;
    color: #64748b;
    font-size: 1.125rem;
`;

export const ErrorMessage = styled.div`
    text-align: center;
    padding: 2rem;
    color: #e11d48;
    font-size: 1.125rem;
`;

export const Modal = styled.div`
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
`;

export const ModalContent = styled.div`
  background: white;
  padding: 2rem;
  border-radius: 16px;
  width: 90%;
  max-width: 500px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  animation: ${fadeIn} 0.3s ease-out;
`;

export const ModalTitle = styled.h2`
  font-size: 1.5rem;
  color: #1e293b;
  margin-bottom: 1.5rem;
  text-align: center;
`;

export const ModalInput = styled.input`
  width: 100%;
  padding: 0.75rem;
  border: 2px solid #e2e8f0;
  border-radius: 8px;
  font-size: 1rem;
  margin-bottom: 1.5rem;
  transition: border-color 0.2s;

  &:focus {
    outline: none;
    border-color: #3b82f6;
  }
`;

export const ModalButtonGroup = styled.div`
  display: flex;
  gap: 1rem;
  justify-content: flex-end;
`;

export const ModalButton = styled.button`
  padding: 0.75rem 1.5rem;
  border-radius: 8px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;

  ${props => props.primary ? `
    background: #3b82f6;
    color: white;
    border: none;

    &:hover {
      background: #2563eb;
    }
  ` : `
    background: white;
    color: #64748b;
    border: 2px solid #e2e8f0;

    &:hover {
      background: #f1f5f9;
    }
  `}
`;

export const ProfileImage = styled.img`
  width: 40px;
  height: 40px;
  border-radius: 50%;
  margin-right: 10px;
`;


export const UserProfileImage = styled.img`
  width: 50px;
  height: 50px;
  border-radius: 50%;
  margin-left: 5px;  // 간격을 조금 줄임
`;

export const CreatorProfileImage = styled.img`
  width: 20px;
  height: 20px;
  border-radius: 50%;
  margin-left: 10px;
`;

export const ProfileContainer = styled.div`
    display: flex;
    align-items: center;
    gap: 0.5rem;
`;

export const RoomNameContainer = styled.div`
  display: flex;
  align-items: center;
  justify-content: space-between;
`;

export const RoomProfileImage = styled.img`
  width: 40px;
  height: 40px;
  border-radius: 50%;
  margin-right: 10px;
`;

export const UploadArea = styled.div`
  border: 2px dashed #94a3b8;
  border-radius: 12px;
  padding: 2rem;
  text-align: center;
  cursor: pointer;
  color: #64748b;
  position: relative;

  &:hover {
    background-color: #f1f5f9;
  }
`;

export const UploadInput = styled.input`
  display: none;
`;

export const UploadText = styled.p`
  font-size: 1rem;
  color: #64748b;
`;

export const UploadImage = styled.img`
  max-width: 100%;
  max-height: 300px;
  object-fit: contain;
`;

export const HistoryButton = styled.button`
  position: fixed;
  bottom: 6rem;
  right: 6rem;
  width: 60px;
  height: 60px;
  border-radius: 50%;
  border: none;
  cursor: pointer;
  background: linear-gradient(135deg, #4e80ee 0%, #3b5998 100%);
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
  
  outline: none;
  &:focus {
    outline: none;
  }
  /* 페이드인 + 슬라이드 애니메이션 추가 */
  animation: slideIn 0.5s ease-out;
  
  @keyframes slideIn {
    0% {
      opacity: 0;
      transform: translateY(20px);
    }
    100% {
      opacity: 1;
      transform: translateY(0);
    }
  }

  /* 호버 효과는 별도로 관리 */
  transition: transform 0.2s, box-shadow 0.2s;
  &:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.15);
  }
`;

export const HistoryIcon = styled.img`
  width: 30px;
  height: 30px;
  filter: brightness(0) invert(1); // PNG 아이콘을 흰색으로 변경
`;

export const HistoryPopup = styled.div`
  position: fixed;
  bottom: 12rem;
  right: 6rem;
  background: white;
  border-radius: 12px;
  padding: 1.5rem;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.15);
  min-width: 300px;
  max-width: 400px;
  max-height: calc(100vh - 100px);
  overflow-y: auto;
  z-index: 1000;
  
  /* 페이드인 애니메이션 추가 */
  animation: fadeIn 0.3s ease-out;
  
  @keyframes fadeIn {
    from {
      opacity: 0;
      transform: translateY(10px);
    }
    to {
      opacity: 1;
      transform: translateY(0);
    }
  }

  /* 기존의 스크롤바 스타일 유지 */
  scrollbar-width: none;
  -ms-overflow-style: none;
  &::-webkit-scrollbar {
    display: none;
  }

  &:hover {
    &::-webkit-scrollbar {
      display: block;
      width: 4px;
    }

    &::-webkit-scrollbar-track {
      background: transparent;
    }

    &::-webkit-scrollbar-thumb {
      background: rgba(0, 0, 0, 0.2);
      border-radius: 4px;
    }

    &::-webkit-scrollbar-thumb:hover {
      background: rgba(0, 0, 0, 0.3);
    }
  }

  &::after {
    content: '';
    position: absolute;
    bottom: -10px;
    right: 25px;
    width: 0;
    height: 0;
    border-left: 10px solid transparent;
    border-right: 10px solid transparent;
    border-top: 10px solid white;
  }
`;

export const HistoryList = styled.div`
  display: flex;
  flex-direction: column;
  gap: 1rem;
  width: 100%;
`;

export const HistoryItem = styled.div`
  padding: 1rem;
  background: #f8f9fa;
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s ease;
  width: 100%;

  &:hover {
    background: #eef1f5;
    transform: translateY(-2px);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
  }
`;

export const HistoryTitle = styled.h3`
  margin: 0 0 0.5rem 0;
  font-size: 1rem;
  color: #1e293b;
  font-weight: 600;
`;

export const HistoryDate = styled.p`
  margin: 0;
  font-size: 0.875rem;
  color: #64748b;
`;
