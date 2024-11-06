import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  PageContainer,
  NavBar,
  Logo,
  LogoText,
  LogoutButton,
  Content,
  Header,
  Title,
  CreateRoomButton,
  RoomGrid,
  RoomCard,
  RoomName,
  RoomStatus,
  JoinButton,
  LoadingMessage,
  ErrorMessage
} from '../styles/GameListPage.styles';
import { gameService } from '../services/gameService';
import { authService } from '../services/authService';

const GameListPage = () => {
  const navigate = useNavigate();
  const [rooms, setRooms] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  const handleLogout = () => {
    authService.logout();
    navigate('/');
  };

  useEffect(() => {
    const fetchRooms = async () => {
      try {
        const token = localStorage.getItem('userToken');
        const response = await gameService.getRoomList(token);
        setRooms(response.rooms);
        setLoading(false);
      } catch (err) {
        setError('방 목록을 불러오는데 실패했습니다.');
        setLoading(false);
      }
    };

    fetchRooms();
  }, []);

  if (loading) return <LoadingMessage>로딩 중...</LoadingMessage>;
  if (error) return <ErrorMessage>{error}</ErrorMessage>;

  return (
    <PageContainer>
      <NavBar>
        <Logo>
          ♟️ <LogoText>Chess Master</LogoText>
        </Logo>
        <LogoutButton onClick={handleLogout}>로그아웃</LogoutButton>
      </NavBar>
      <Content>
        <Header>
          <Title>게임 방 목록</Title>
          <CreateRoomButton>+ 새로운 방 만들기</CreateRoomButton>
        </Header>
        <RoomGrid>
          {rooms.map((room) => {
            console.log(`Room ID: ${room.room_id}, Joined: ${room.joined}`);
            return (
              <RoomCard key={room.room_id} joined={room.joined}>
                <RoomName>{room.room_name}</RoomName>
                <RoomStatus joined={room.joined}>
                  {room.joined ? '입장 불가' : '참가 가능'}
                </RoomStatus>
                <JoinButton disabled={room.joined}>
                  {room.joined ? '입장 불가' : '입장하기'}
                </JoinButton>
              </RoomCard>
            );
          })}
        </RoomGrid>
      </Content>
    </PageContainer>
  );
};

export default GameListPage;