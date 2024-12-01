import { API_BASE_URL } from '../config';

export const fetchGameHistory = async (roomId) => {
  try {
    const response = await fetch(`${API_BASE_URL}/game/history/${roomId}`);
    if (!response.ok) {
      throw new Error('Failed to fetch game history');
    }
    const data = await response.text();
    return data;
  } catch (error) {
    console.error('Error fetching game history:', error);
    throw error;
  }
};
