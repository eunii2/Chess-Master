import React, { useState, useEffect, useRef } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { gameService } from '../services/gameService';
import Chessboard from '../components/Chessboard';
import {
  GameContainer,
  ChessboardContainer,
  SideContainer,
  PlayerCard,
  PlayerAvatar,
  PlayerName,
  ChatContainer,
  ChatHeader,
  ChatMessages,
  ChatInput, 
  SurrenderButton,
  PlayerInfo,
  MessageContainer,
  Username,
  MessageBubble,
  GameOverlay,
  GameOverModal,
  GameOverTitle,
  GameOverMessage,
  ButtonContainer,
  Button,
  HomeButton,
  RecordButton
} from '../styles/GamePage.styles';

const GamePage = () => {
  const { roomId } = useParams();
  const [isCreator, setIsCreator] = useState(false);
  const [selectedPosition, setSelectedPosition] = useState(null);
  const [gameStatus, setGameStatus] = useState(null);
  const currentUserToken = localStorage.getItem('userToken');
  const [messages, setMessages] = useState([]);
  const [message, setMessage] = useState('');
  const messagesEndRef = useRef(null);
  const [shouldScrollToBottom, setShouldScrollToBottom] = useState(true);
  const chatContainerRef = useRef(null);
  const navigate = useNavigate();
  const [gameResult, setGameResult] = useState(null);

  const handleScroll = (e) => {
    const element = e.target;
    const isScrolledNearBottom = 
      element.scrollHeight - element.scrollTop - element.clientHeight < 100;
    setShouldScrollToBottom(isScrolledNearBottom);
  };

  const scrollToBottom = () => {
    if (shouldScrollToBottom) {
      messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
    }
  };

  useEffect(() => {
    scrollToBottom();
  }, [messages, shouldScrollToBottom]);

  // 방장 여부 확인
  useEffect(() => {
    const checkCreator = async () => {
      try {
        const data = await gameService.getRoomStatus(roomId, currentUserToken);
        setIsCreator(data.is_creator);
      } catch (error) {
        console.error('Failed to check creator status:', error);
      }
    };

    checkCreator();
  }, [roomId, currentUserToken]);

  // 게임 상태 주기적 확인
  useEffect(() => {
    const fetchGameStatus = async () => {
      try {
        const status = await gameService.getGameStatus(roomId, currentUserToken);
        setGameStatus(status);
        
      } catch (error) {
        console.error('Failed to fetch game status:', error);
      }
    };

    // 초기 상태 가져오기
    fetchGameStatus();

    // 1초마다 상태 업데이트
    const interval = setInterval(fetchGameStatus, 1000);

    // 컴포넌트 언마운트 시 인터벌 정리
    return () => clearInterval(interval);
  }, [roomId, currentUserToken]);

  // 기물 이동
  const handleSquareClick = async (fromPosition, toPosition) => {
    try {
      console.log('Moving piece from', fromPosition, 'to', toPosition);
      
      // 이동 결과를 받아옴
      const moveResult = await gameService.movePiece(
        roomId,
        currentUserToken,
        fromPosition,
        toPosition
      );
      
      // 새로운 게임 상태를 가져옴
      const newStatus = await gameService.getGameStatus(roomId, currentUserToken);
      setGameStatus(newStatus);
      
    } catch (error) {
      if (error.response) {
        if (!error.response.data?.game_over) {
          alert(error.response.data?.message || '이동할 수 없습니다.');
        }
      } else {
        console.error('Move failed:', error);
      }
    }
  };

  // 기권
  const handleSurrender = async () => {
    try {
      await gameService.forfeitGame(roomId, currentUserToken);
      alert(isCreator ? "백이 기권했습니다." : "흑이 기권했습니다.");
    } catch (error) {
      console.error('Surrender failed:', error);
      alert('기권에 실패했습니다.');
    }
  };

  // 디버깅을 위한 로그 추가
  useEffect(() => {
    console.log('Current game status:', gameStatus);
    console.log('Current player token:', gameStatus?.current_player_token);
    console.log('Player1 token:', gameStatus?.player1_token);
    console.log('Player2 token:', gameStatus?.player2_token);
  }, [gameStatus]);

  useEffect(() => {
    const fetchMessages = async () => {
      try {
        const messages = await gameService.getMessages(roomId, currentUserToken);
        setMessages(messages);
      } catch (error) {
        console.error('Failed to fetch messages:', error);
      }
    };

    // 초기 메시지 로드
    fetchMessages();

    // 1초마다 메시지 업데이트
    const interval = setInterval(fetchMessages, 1000);

    return () => clearInterval(interval);
  }, [roomId, currentUserToken]);

  const handleSendMessage = async () => {
    if (!message.trim()) return;

    try {
      await gameService.sendMessage(roomId, currentUserToken, message);
      setMessage('');
    } catch (error) {
      console.error('Failed to send message:', error);
    }
  };

  const handleKeyPress = (e) => {
    if (e.key === 'Enter' && !e.shiftKey) {
      e.preventDefault();
      handleSendMessage();
    }
  };

  useEffect(() => {
    if (gameStatus?.game_over) {
      console.log('Game Status:', gameStatus); // 디버깅을 위한 로그 추가
      
      // 현재 플레이어가 승자인지 확인
      const amIWinner = gameStatus.winner_token === currentUserToken;
      
      setGameResult({
        isWin: amIWinner,
        reason: gameStatus.game_over_reason,
        winner: gameStatus.winner_username
      });
    }
  }, [gameStatus, currentUserToken]);

  const getGameOverMessage = () => {
    console.log('Game Result:', gameResult); // 디버깅을 위한 로그 추가
    
    if (gameResult.reason === 'king_captured') {
      return gameResult.isWin ? 
        "상대방의 왕을 잡았습니다" : 
        "당신의 왕이 잡혔습니다";
    } else if (gameResult.reason === 'forfeit') {
      return gameResult.isWin ? 
        "상대방이 기권했습니다" : 
        "당신이 기권했습니다";
    }
    return "게임이 종료되었습니다"; // 기본 메시지
  };

  return (
    <GameContainer>
      <ChessboardContainer>
        <Chessboard 
          isCreator={isCreator} 
          onSquareClick={handleSquareClick}
          gameStatus={gameStatus}
        />
      </ChessboardContainer>
      
      <SideContainer>
        <PlayerCard $isCurrentPlayer={gameStatus?.current_player_token === gameStatus?.player1_token}>
          <PlayerInfo>
            <PlayerAvatar />
            <PlayerName>
              {gameStatus?.player1_username} (White)
            </PlayerName>
          </PlayerInfo>
          {isCreator && <SurrenderButton onClick={handleSurrender}>기권하기</SurrenderButton>}
        </PlayerCard>
        
        <PlayerCard $isCurrentPlayer={gameStatus?.current_player_token === gameStatus?.player2_token}>
          <PlayerInfo>
            <PlayerAvatar />
            <PlayerName>
              {gameStatus?.player2_username} (Black)
            </PlayerName>
          </PlayerInfo>
          {!isCreator && <SurrenderButton onClick={handleSurrender}>기권하기</SurrenderButton>}
        </PlayerCard>
        
        <ChatContainer>
          <ChatHeader>Chat</ChatHeader>
          <ChatMessages 
            ref={chatContainerRef}
            onScroll={handleScroll}
          >
            {messages?.map((msg, index) => (
              <MessageContainer 
                key={index} 
                $isMe={msg.username === (isCreator ? gameStatus?.player1_username : gameStatus?.player2_username)}
              >
                <Username 
                  $isMe={msg.username === (isCreator ? gameStatus?.player1_username : gameStatus?.player2_username)}
                >
                  {msg.username}
                </Username>
                <MessageBubble 
                  $isMe={msg.username === (isCreator ? gameStatus?.player1_username : gameStatus?.player2_username)}
                >
                  {msg.content}
                </MessageBubble>
              </MessageContainer>
            ))}
            <div ref={messagesEndRef} />
          </ChatMessages>
          <ChatInput>
            <input
              type="text"
              value={message}
              onChange={(e) => setMessage(e.target.value)}
              onKeyPress={handleKeyPress}
              placeholder="메시지를 입력하세요..."
            />
            <button type="button" onClick={handleSendMessage}>전송</button>
          </ChatInput>
        </ChatContainer>
      </SideContainer>
      
      {gameResult && (
        <GameOverlay>
          <GameOverModal>
            <GameOverTitle $isWin={gameResult.isWin}>
              {gameResult.isWin ? 'You win!' : 'You lose'}
            </GameOverTitle>
            <GameOverMessage>
              {getGameOverMessage()}
            </GameOverMessage>
            <ButtonContainer>
              <RecordButton onClick={() => navigate(`/games/${roomId}/record`)}>
                기록 보기
              </RecordButton>
              <HomeButton onClick={() => navigate('/games')}>
                홈으로
              </HomeButton>
            </ButtonContainer>
          </GameOverModal>
        </GameOverlay>
      )}
    </GameContainer>
  );
};
export default GamePage;