import React, { useState } from 'react';
import { Board, Square, Piece } from '../styles/Chessboard.styles';
import styled from 'styled-components';

const Chessboard = ({ isCreator, onSquareClick, gameStatus }) => {
  const [selectedPiece, setSelectedPiece] = useState(null);

  const toChessNotation = (row, col) => {
    if (isCreator) {
      return `${String.fromCharCode(97 + col)}${8 - row}`;
    } else {
      return `${String.fromCharCode(97 + (7 - col))}${row + 1}`;
    }
  };

  const handleSquareClick = (position, row, col) => {
    if (!selectedPiece) {
      const piece = gameStatus?.board?.[row]?.[col];
      if (piece && piece !== ' ') {
        setSelectedPiece({ position, row, col });
      }
    } else {
      onSquareClick(selectedPiece.position, position);
      setSelectedPiece(null);
    }
  };

  const renderSquare = (row, col) => {
    const isLight = (row + col) % 2 === 0;
    const actualRow = isCreator ? row : 7 - row;
    const actualCol = isCreator ? col : 7 - col;
    const piece = gameStatus?.board?.[actualRow]?.[actualCol] || ' ';
    const position = toChessNotation(row, col);
    const isSelected = selectedPiece?.row === actualRow && selectedPiece?.col === actualCol;

    return (
      <Square 
        key={position} 
        $isLight={isLight}
        $isSelected={isSelected}
        onClick={() => handleSquareClick(position, actualRow, actualCol)}
      >
        {piece !== ' ' && (
          <Piece 
            $pieceType={piece}
            $isWhitePiece={piece === piece.toUpperCase()}
          />
        )}
      </Square>
    );
  };

  const renderBoard = () => {
    const squares = [];
    for (let row = 0; row < 8; row++) {
      for (let col = 0; col < 8; col++) {
        squares.push(renderSquare(row, col));
      }
    }
    return squares;
  };

  return (
    <Board>
      {renderBoard()}
    </Board>
  );
};

export default Chessboard;