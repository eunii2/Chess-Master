import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import {
  RoomContainer,
  RoomContent,
  RoomHeader,
  RoomTitle,
  RoomSubtitle,
  PlayerSection,
  Player,
  PlayerName,
  PlayerStatus,
  GameControls,
  StartButton,
  LeaveButton
} from '../styles/GameRoom.styles';
import { gameService } from '../services/gameService';

const GameRoom = () => {
  const { roomId } = useParams();
  const navigate = useNavigate();
  const [players, setPlayers] = useState([]);
  const [isReady, setIsReady] = useState(false);
  const [error, setError] = useState(null);
  const [roomName, setRoomName] = useState('');
  const [isCreator, setIsCreator] = useState(false);

  useEffect(() => {
    const token = localStorage.getItem('userToken');
    if (!token) {
      navigate('/');
      return;
    }

    const fetchRoomStatus = async () => {
      try {
        const response = await gameService.getRoomStatus(roomId, token);
        console.log('Room Status Response:', response);
        
        setIsReady(response.has_joined_members);
        setRoomName(response.room_name);
        setIsCreator(response.is_creator);
        
        if (response.is_creator) {
          setPlayers([
            {
              username: '방장',
              isCreator: true
            },
            ...(response.has_joined_members ? [{
              username: '참가자',
              isCreator: false
            }] : [])
          ]);
        } else {
          setPlayers([
            {
              username: '방장',
              isCreator: true
            },
            {
              username: '참가자',
              isCreator: false
            }
          ]);
        }
      } catch (err) {
        console.error('Room Status Error:', err);
        setError('방 정보를 불러오는데 실패했습니다.');
      }
    };

    fetchRoomStatus();
    const interval = setInterval(fetchRoomStatus, 3000);
    return () => clearInterval(interval);
  }, [roomId, navigate]);

  const handleStartGame = async () => {
    try {
      const token = localStorage.getItem('userToken');
      await gameService.startGame(roomId, token);
      navigate(`/game-start/${roomId}`); 
    } catch (err) {
      setError('게임 시작에 실패했습니다.');
    }
  };

  const handleLeaveRoom = async () => {
    try {
      const token = localStorage.getItem('userToken');
      await gameService.leaveRoom(roomId, token);
      navigate('/game-list');
    } catch (err) {
      setError('방을 나가는데 실패했습니다.');
    }
  };

  return (
    <RoomContainer>
      <RoomContent>
        <LeaveButton onClick={handleLeaveRoom}>
          나가기
        </LeaveButton>
        <RoomHeader>
          <RoomTitle>{roomName}</RoomTitle>
          <RoomSubtitle>체스 게임 대기실</RoomSubtitle>
        </RoomHeader>
        <PlayerSection>
          {players.map((player, index) => (
            <Player key={index}>
              <PlayerName>{player.username}</PlayerName>
              <PlayerStatus isCreator={player.isCreator}>
                {player.isCreator ? '방장' : '준비완료'}
              </PlayerStatus>
            </Player>
          ))}
          {players.length < 2 && (
            <Player waiting>
              <PlayerName waiting>대기 중...</PlayerName>
            </Player>
          )}
        </PlayerSection>
        <GameControls>
          {isCreator ? (
            <StartButton 
              disabled={!isReady}
              onClick={handleStartGame}
            >
              게임 시작
            </StartButton>
          ) : (
            <StartButton disabled>
              게임 시작 기다리는 중...
            </StartButton>
          )}
        </GameControls>
      </RoomContent>
    </RoomContainer>
  );
};

export default GameRoom;
