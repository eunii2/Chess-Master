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
  },

  async joinRoom(roomId, token) {
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/room/join`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          token,
          room_id: parseInt(roomId, 10)
        }
      });
      return response.data;
    } catch (error) {
      console.error('Join Room Error:', error);
      throw error;
    }
  },

  async getRoomStatus(roomId, token) {
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/room/status`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          token: token,
          room_id: parseInt(roomId, 10)
        }
      });
      console.log('Room Status Response:', JSON.stringify(response.data, null, 2));
      return response.data;
    } catch (error) {
      console.error('Get Room Status Error:', error);
      throw error;
    }
  },

  async startGame(roomId, token) {
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/room/${roomId}/start`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: { token }
      });
      return response.data;
    } catch (error) {
      console.error('Start Game Error:', error);
      throw error;
    }
  },

  async createRoom(token, roomName) {
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/room/create`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          token: token,
          room_name: roomName
        }
      });
      return response.data;
    } catch (error) {
      console.error('Create Room Error:', error);
      throw error;
    }
  },

  async leaveRoom(roomId, token) {
    try {
      const response = await axios({
        method: 'DELETE',
        url: `${API_URL}/room`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          token: token,
          room_id: parseInt(roomId, 10)
        }
      });
      return response.data;
    } catch (error) {
      console.error('Leave Room Error:', error);
      throw error;
    }
  }
};