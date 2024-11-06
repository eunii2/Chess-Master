import axios from 'axios';

const API_URL = 'http://localhost:8080';

export const authService = {
  async join(username, password) {
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/join`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          username: username,
          password: password
        },
        validateStatus: function (status) {
          return status >= 200 && status < 300 || status === 201;
        }
      });
      
      return response.data;
    } catch (error) {
      throw error;
    }
  },

  async login(username, password) {
    try {
      const response = await axios({
        method: 'POST',
        url: `${API_URL}/login`,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          username,
          password
        }
      });
      
      if (response.data.token) {
        localStorage.setItem('userToken', response.data.token);
      }
      return response.data;
    } catch (error) {
      console.error('Login Error:', error);
      throw error;
    }
  },

  logout() {
    localStorage.removeItem('userToken');
  },

  getToken() {
    return localStorage.getItem('userToken');
  },

  isLoggedIn() {
    return !!localStorage.getItem('userToken');
  }
};
