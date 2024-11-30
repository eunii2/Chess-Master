import React, { useState, useEffect } from 'react';
import { useParams } from 'react-router-dom';
import {
  GameStartContainer,
  PlayerContainer,
  ChessBoardContainer,
  ChatContainer,
  ChatBox,
  MessageInput,
  SendButton
} from '../styles/GameStart.styles';
import { gameService } from '../services/gameService';

const GameStart = () => {
  const { roomId } = useParams();
  const [isPlayerOneTurn, setIsPlayerOneTurn] = useState(true);
  const [messages, setMessages] = useState([]);
  const [newMessage, setNewMessage] = useState('');
  const [players, setPlayers] = useState([]);

  useEffect(() => {
    const fetchRoomStatus = async () => {
      try {
        const token = localStorage.getItem('userToken');
        const response = await gameService.getRoomStatus(roomId, token);
        
        if (response) {
          setPlayers([
            {
              username: 'Player 1',
              isCreator: response.is_creator,
              isActive: response.is_creator ? isPlayerOneTurn : !isPlayerOneTurn
            },
            {
              username: 'Player 2',
              isCreator: !response.is_creator,
              isActive: !response.is_creator ? isPlayerOneTurn : !isPlayerOneTurn
            }
          ]);
        }
      } catch (err) {
        console.error('Error fetching room status:', err);
      }
    };
    fetchRoomStatus();
  }, [roomId, isPlayerOneTurn]);

  const handleSendMessage = () => {
    if (newMessage.trim()) {
      setMessages([...messages, { sender: isPlayerOneTurn ? 'Player 1' : 'Player 2', text: newMessage }]);
      setNewMessage('');
    }
  };

  return (
    <GameStartContainer>
      {players.map((player, index) => (
        <PlayerContainer key={index} isActive={player.isActive}>
          <h2>{player.username}</h2>
        </PlayerContainer>
      ))}
      <ChessBoardContainer>
        <h2>체스 보드 영역 (추후 구현)</h2>
      </ChessBoardContainer>
      <ChatContainer>
        <ChatBox>
          {messages.map((msg, index) => (
            <p key={index}><strong>{msg.sender}:</strong> {msg.text}</p>
          ))}
        </ChatBox>
        <MessageInput
          type="text"
          value={newMessage}
          onChange={(e) => setNewMessage(e.target.value)}
          placeholder="메시지를 입력하세요..."
        />
        <SendButton onClick={handleSendMessage}>전송</SendButton>
      </ChatContainer>
    </GameStartContainer>
  );
};

export default GameStart;
