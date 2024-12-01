import React, { useState, useEffect } from 'react';
import { fetchGameHistory } from '../services/gameApi';
import {
  HistoryContainer,
  ControlsContainer,
  Button,
  MoveInfo,
  BoardDisplay,
  BoardRow,
  BoardCell
} from '../styles/GameHistory.styles';

const GameHistory = ({ roomId }) => {
  const [moves, setMoves] = useState([]);
  const [currentMoveIndex, setCurrentMoveIndex] = useState(-1);
  const [currentBoard, setCurrentBoard] = useState(null);

  useEffect(() => {
    const loadHistory = async () => {
      const history = await fetchGameHistory(roomId);
      const parsedMoves = parseHistoryData(history);
      setMoves(parsedMoves);
      setCurrentMoveIndex(-1); // 시작 상태
    };
    loadHistory();
  }, [roomId]);

  const parseHistoryData = (historyText) => {
    const moves = [];
    const lines = historyText.split('\n');
    let currentMove = null;

    lines.forEach(line => {
      if (line.startsWith('Player:')) {
        // 이동 정보 파싱
        const moveMatch = line.match(/Player: (\w+) moved (\w+) from (\w+\d) to (\w+\d)/);
        if (moveMatch) {
          currentMove = {
            player: moveMatch[1],
            piece: moveMatch[2],
            from: moveMatch[3],
            to: moveMatch[4],
            board: null
          };
        }
      } else if (line.includes('Board state after move:')) {
        // 다음 8줄이 보드 상태
        const boardState = lines.slice(
          lines.indexOf(line) + 1, 
          lines.indexOf(line) + 9
        ).join('\n');
        if (currentMove) {
          currentMove.board = boardState;
          moves.push(currentMove);
          currentMove = null;
        }
      }
    });
    return moves;
  };

  const handlePrevMove = () => {
    if (currentMoveIndex > -1) {
      setCurrentMoveIndex(currentMoveIndex - 1);
      setCurrentBoard(currentMoveIndex - 1 >= 0 
        ? moves[currentMoveIndex - 1].board 
        : getInitialBoard());
    }
  };

  const handleNextMove = () => {
    if (currentMoveIndex < moves.length - 1) {
      setCurrentMoveIndex(currentMoveIndex + 1);
      setCurrentBoard(moves[currentMoveIndex + 1].board);
    }
  };

  return (
    <HistoryContainer>
      <ControlsContainer>
        <Button onClick={handlePrevMove} disabled={currentMoveIndex === -1}>
          &lt;
        </Button>
        <MoveInfo>
          {currentMoveIndex >= 0 && moves[currentMoveIndex] ? (
            `${moves[currentMoveIndex].player} moved ${moves[currentMoveIndex].piece} 
             from ${moves[currentMoveIndex].from} to ${moves[currentMoveIndex].to}`
          ) : (
            "게임 시작"
          )}
        </MoveInfo>
        <Button onClick={handleNextMove} disabled={currentMoveIndex === moves.length - 1}>
          &gt;
        </Button>
      </ControlsContainer>
      <BoardDisplay>
        {currentBoard && currentBoard.split('\n').map((row, i) => (
          <BoardRow key={i}>
            {row.split('').map((cell, j) => (
              <BoardCell key={j}>{cell === ' ' ? '\u00A0' : cell}</BoardCell>
            ))}
          </BoardRow>
        ))}
      </BoardDisplay>
    </HistoryContainer>
  );
};

export default GameHistory;
