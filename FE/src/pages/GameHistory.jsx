import React, { useState, useEffect } from 'react';
import { useParams } from 'react-router-dom';
import { gameService } from '../services/gameService';

const GameHistory = () => {
    const { roomId } = useParams();
    const [history, setHistory] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        const loadHistory = async () => {
            try {
                setLoading(true);
                const response = await gameService.getGameHistory(roomId);
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

    if (loading) return <div>로딩 중...</div>;
    if (error) return <div>{error}</div>;
    if (!history) return <div>기록이 없습니다.</div>;

    // 히스토리 문자열을 줄바꿈으로 분리
    const moves = history.history.split('\n').filter(line => line.trim() !== '');

    return (
        <div>
            <h2>게임 기록</h2>
            <div className="moves-list" style={{ whiteSpace: 'pre-wrap', fontFamily: 'monospace' }}>
                {moves.map((move, index) => (
                    <div key={index} className="move-item">
                        {move}
                    </div>
                ))}
            </div>
        </div>
    );
};

export default GameHistory;
