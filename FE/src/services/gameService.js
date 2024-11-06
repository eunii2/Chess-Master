import axios from 'axios';

const API_URL = 'http://localhost:8080';

export const gameService = {
  async getRoomList(token) {
    console.log("Sending token:", token);
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/room`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          token: token
        }
      });
      console.log("Received response:", response.data);
      return response.data;
    } catch (error) {
      console.error('Get Room List Error:', error);
      throw error;
    }
  }
};