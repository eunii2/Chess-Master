import styled from 'styled-components';

export const Board = styled.div`
  display: grid;
  grid-template-columns: repeat(8, 1fr);
  width: 100%;
  aspect-ratio: 1;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
  border-radius: 8px;
  overflow: hidden;
  border: none;
`;

export const Square = styled.div`
  aspect-ratio: 1;
  background-color: ${props => (props.$isLight ? '#fff' : '#999')};
  display: flex;
  justify-content: center;
  align-items: center;
  cursor: pointer;
  &:hover {
    background-color: ${props => (props.$isLight ? '#e6e6e6' : '#666')};
  }
`;

export const Piece = styled.div`
  width: 80%;
  height: 80%;
  font-size: 4.5em;
  display: flex;
  justify-content: center;
  align-items: center;
  color: ${props => props.$isWhitePiece ? '#fff' : '#000'};
  text-shadow: ${props => props.$isWhitePiece ? 
    '-1px -1px 0 #000, 1px -1px 0 #000, -1px 1px 0 #000, 1px 1px 0 #000' : 
    'none'};

  &:hover {
    transform: scale(1.1);
  }
  
  &::after {
    content: '${props => {
      switch(props.$pieceType.toLowerCase()) {
        case 'p': return '♟';
        case 'r': return '♜';
        case 'n': return '♞';
        case 'b': return '♝';
        case 'q': return '♛';
        case 'k': return '♚';
        default: return '';
      }
    }}';
  }
`;
