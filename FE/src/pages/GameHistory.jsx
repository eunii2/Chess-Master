import React, { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { gameService } from '../services/gameService';
import Chessboard from '../components/Chessboard';
import homeIcon from '../assets/homeButton.png';
import {
  HistoryContainer,
  ChessboardSection,
  SidebarSection,
  ResultSection,
  MovesSection,
  ControlSection,
  MoveItem,
  ControlButton,
  ResultHeader,
  TitleGroup,
  ResultText,
  HomeButton,
  HomeIcon
} from '../styles/GameHistory.styles';

const GameHistory = () => {
    const { roomId } = useParams();
    const navigate = useNavigate();
    const [history, setHistory] = useState(null);
    const [currentMove, setCurrentMove] = useState(-1);
    const [boardStates, setBoardStates] = useState([]);
    const [moves, setMoves] = useState([]);
    const [highlightSquares, setHighlightSquares] = useState({ from: null, to: null });
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    // 초기 체스판 상태 추가
    const initialBoard = [
        ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
        ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'],
        [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
        ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']
    ];

    const applyMove = (board, from, to) => {
        const newBoard = board.map(row => [...row]);
        
        const fromCol = from.charCodeAt(0) - 'a'.charCodeAt(0);
        const fromRow = 8 - parseInt(from[1]);
        const toCol = to.charCodeAt(0) - 'a'.charCodeAt(0);
        const toRow = 8 - parseInt(to[1]);
        
        newBoard[toRow][toCol] = newBoard[fromRow][fromCol];
        newBoard[fromRow][fromCol] = ' ';
        
        return newBoard;
    };

    useEffect(() => {
        const loadHistory = async () => {
            try {
                setLoading(true);
                const response = await gameService.getGameHistory(roomId);
                const historyLines = response.history.split('\n').filter(line => line.trim() !== '');
                
                const states = [initialBoard];
                const movesList = [];
                let currentBoard = initialBoard.map(row => [...row]);
                let movesPair = [];

                historyLines.forEach(line => {
                    if (line.startsWith('Player:')) {
                        const move = parseMoveNotation(line);
                        movesPair.push(move);
                        
                        currentBoard = applyMove(currentBoard, move.from, move.to);
                        states.push(currentBoard.map(row => [...row]));
                        
                        if (movesPair.length === 2) {
                            movesList.push(movesPair);
                            movesPair = [];
                        }
                    }
                });

                if (movesPair.length > 0) {
                    movesList.push(movesPair);
                }

                setBoardStates(states);
                setMoves(movesList);
                setHistory(response);
            } catch (error) {
                console.error('Error loading game history:', error);
                setError('게임 기록을 불러오는데 실패했습니다.');
            } finally {
                setLoading(false);
            }
        };
        
        loadHistory();
    }, [roomId]);

    const parseMoveNotation = (moveLine) => {
        const parts = moveLine.split(' ');
        return {
            player: parts[1],
            piece: parts[3],
            from: parts[5],
            to: parts[7]
        };
    };

    const updateHighlight = (moveIndex) => {
        if (moveIndex < 0) {
            setHighlightSquares({ from: null, to: null });
            return;
        }

        const moveNumber = Math.floor(moveIndex / 2);
        const isWhiteMove = moveIndex % 2 === 0;
        const currentMoves = moves[moveNumber];
        
        if (currentMoves) {
            const move = currentMoves[isWhiteMove ? 0 : 1];
            if (move) {
                setHighlightSquares({
                    from: move.from,
                    to: move.to
                });
            }
        }
    };

    const handlePrevMove = () => {
        if (currentMove >= 0) {
            setCurrentMove(prev => prev - 1);
            updateHighlight(currentMove - 1);
        }
    };

    const handleNextMove = () => {
        if (currentMove < boardStates.length - 2) {
            setCurrentMove(prev => prev + 1);
            updateHighlight(currentMove + 1);
        }
    };

    const handleHomeClick = () => {
        navigate('/games');
    };

    if (loading) return <div>로딩 중...</div>;
    if (error) return <div>{error}</div>;
    if (!history) return <div>기록이 없습니다.</div>;

    return (
        <HistoryContainer>
            <ChessboardSection>
                <Chessboard 
                    isCreator={true} 
                    gameStatus={{ board: currentMove < 0 ? initialBoard : boardStates[currentMove + 1] }}
                    highlightSquares={highlightSquares}
                />
            </ChessboardSection>
            
            <SidebarSection>
                <ResultHeader>
                    <TitleGroup>
                        <h2>Game Result :</h2>
                        <ResultText>
                            {history.history
                                .split('\n')
                                .filter(line => line.trim() !== '')
                                .map(line => line.replace("Game Over!", "").trim())
                                .pop() || '게임 결과 없음'
                            }
                        </ResultText>
                    </TitleGroup>
                    <HomeButton onClick={handleHomeClick}>
                        <HomeIcon src={homeIcon} alt="Home" />
                    </HomeButton>
                </ResultHeader>

                <MovesSection>
                    {moves.map((movePair, index) => (
                        <MoveItem 
                            key={index} 
                            $isActive={Math.floor(currentMove/2) === index}
                        >
                            <div>{`${index + 1}.`}</div>
                            <div 
                                onClick={() => {
                                    setCurrentMove(index * 2);
                                    updateHighlight(index * 2);
                                }}
                                style={{
                                    backgroundColor: currentMove === index * 2 ? '#e6e6e6' : 'transparent',
                                    padding: '4px',
                                    cursor: 'pointer'
                                }}
                            >
                                {`${movePair[0]?.from} → ${movePair[0]?.to}`}
                            </div>
                            {movePair[1] && (
                                <div 
                                    onClick={() => {
                                        setCurrentMove(index * 2 + 1);
                                        updateHighlight(index * 2 + 1);
                                    }}
                                    style={{
                                        backgroundColor: currentMove === index * 2 + 1 ? '#e6e6e6' : 'transparent',
                                        padding: '4px',
                                        cursor: 'pointer'
                                    }}
                                >
                                    {`${movePair[1]?.from} → ${movePair[1]?.to}`}
                                </div>
                            )}
                        </MoveItem>
                    ))}
                </MovesSection>

                <ControlSection>
                    <ControlButton 
                        onClick={handlePrevMove} 
                        disabled={currentMove < 0}
                    >
                        ←
                    </ControlButton>
                    <ControlButton 
                        onClick={handleNextMove} 
                        disabled={currentMove >= boardStates.length - 2}
                    >
                        →
                    </ControlButton>
                </ControlSection>
            </SidebarSection>
        </HistoryContainer>
    );
};

export default GameHistory;
