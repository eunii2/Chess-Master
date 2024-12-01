import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import LoginPage from './pages/LoginPage';
import GameListPage from './pages/GameListPage';
import GameRoom from './pages/GameRoom';
import { createGlobalStyle } from 'styled-components';
import GamePage from './pages/GamePage';
import GameHistory from './pages/GameHistory';

const GlobalStyle = createGlobalStyle`
  * {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
  }

  html, body, #root {
    width: 100%;
    height: 100%;
    margin: 0;
    padding: 0;
  }

  body {
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen,
      Ubuntu, Cantarell, 'Fira Sans', 'Droid Sans', 'Helvetica Neue', sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    background: #f5f7fa;
  }

  #root {
    display: flex;
    flex-direction: column;
  }
`;

function App() {
  return (
    <>
      <GlobalStyle />
      <Router>
        <Routes>
          <Route path="/" element={<LoginPage />} />
          <Route path="/games" element={<GameListPage />} />
          <Route path="/games/:roomId" element={<GameRoom />} />
          <Route path="/game/:roomId" element={<GamePage />} />
          <Route path="/games/:roomId/record" element={<GameHistory />} />
        </Routes>
      </Router>
    </>
  );
}

export default App;
