import axios from 'axios';

const API_URL = 'http://localhost:8080';

// 공통 axios 설정
const axiosConfig = {
  headers: {
      'Content-Type': 'application/json'
  },
  withCredentials: false  // CORS 설정
};


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

  // async startGame(roomId, token) {
  //   try {
  //     const response = await axios({
  //       method: 'POST',
  //       url: `${API_URL}/room/${roomId}/start`,
  //       headers: {
  //         'Content-Type': 'application/json'
  //       },
  //       data: { token }
  //     });
  //     return response.data;
  //   } catch (error) {
  //     console.error('Start Game Error:', error);
  //     throw error;
  //   }
  // },

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
  },

  async startGame(roomId, token) {
    try {
        const response = await axios.post(
            `${API_URL}/game/start_game`,
            {
                room_id: Number(roomId),
                token
            },
            axiosConfig
        );
        return response.data;
    } catch (error) {
        console.error('Start Game Error:', error);
        throw error;
    }
},

async getGameStatus(roomId, token) {
    try {
        const response = await axios.post(
            `${API_URL}/game/get_game_status`,
            {
                room_id: parseInt(roomId, 10),
                token: token
            },
            axiosConfig
        );
        console.log('Raw game status response:', response.data.board);
        console.log('Game status response:', response.data);
        return response.data;
    } catch (error) {
        console.error('Get Game Status Error:', error.response?.data || error);
        throw error;
    }
},

async movePiece(roomId, token, fromPosition, toPosition) {
    try {
        // 먼저 게임 상태를 확인
        const gameStatus = await this.getGameStatus(roomId, token);
        
        // 게임이 이미 종료된 상태라면 바로 종료
        if (gameStatus.game_over) {
            return gameStatus;
        }

        const response = await axios.post(
            `${API_URL}/game/move_piece`,
            {
                room_id: parseInt(roomId, 10),
                token,
                from_position: fromPosition,
                to_position: toPosition
            },
            axiosConfig
        );
        return response.data;
    } catch (error) {
        if (error.response?.data?.message) {
            throw new Error(error.response.data.message);
        }
        console.error('Move Piece Error:', error);
        throw error;
    }
},

async forfeitGame(roomId, token) {
    try {
        const response = await axios.post(
            `${API_URL}/game/forfeit`,
            {
                room_id: parseInt(roomId, 10),
                token
            },
            axiosConfig
        );
        return response.data;
    } catch (error) {
        if (error.response?.data?.message) {
            throw new Error(error.response.data.message);
        }
        console.error('Forfeit Game Error:', error);
        throw error;
    }
},

async sendMessage(roomId, token, message) {
    try {
        const response = await axios.post(`${API_URL}/chat/send_message`, {
            room_id: parseInt(roomId),
            token: token,
            message: message
        });
        return response.data;
    } catch (error) {
        console.error('Send message error:', error.response?.data || error);
        throw error;
    }
},

getMessages: async (roomId, token) => {
    try {
      console.log('Requesting messages with:', {
        room_id: roomId,
        token: token
      });

      const response = await axios.post(`${API_URL}/chat/get_messages`, {
        room_id: parseInt(roomId),
        token: token
      });
      
      console.log('Messages response:', response.data);
      
      return response.data.messages;
    } catch (error) {
      console.error('Get messages error:', error.response?.data || error);
      throw error;
    }
  },

  async getGameHistory(roomId) {
    try {
        const response = await axios.post(
            `${API_URL}/game/get_history`,
            {
                room_id: parseInt(roomId, 10)
            },
            axiosConfig
        );
        return response.data;
    } catch (error) {
        console.error('Get Game History Error:', error.response?.data || error);
        throw error;
    }
  },

  getUserGameHistory: async () => {
    try {
      const token = localStorage.getItem('userToken');
      const response = await axios.post('http://localhost:8080/game/user_history', {
        token: token
      });
      
      console.log('Game history response:', response.data);
      return response.data.games;
    } catch (error) {
      console.error('Error fetching user game history:', error);
      throw error;
    }
  }
};


