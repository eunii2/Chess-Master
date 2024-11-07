import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  PageContainer,
  FormContainer,
  LogoSection,
  Logo,
  Title,
  FormSection,
  FormTitle,
  Form,
  InputGroup,
  Label,
  Input,
  SubmitButton,
  ToggleSection,
  ToggleText,
  ToggleButton,
  ErrorMessage,
  PasswordValidation,
  ValidationItem
} from '../styles/LoginPage.styles';
import { authService } from '../services/authService';

const LoginPage = () => {
  const navigate = useNavigate();
  const [isLogin, setIsLogin] = useState(true);
  const [formData, setFormData] = useState({
    username: '',
    password: ''
  });
  const [error, setError] = useState('');
  const [passwordErrors, setPasswordErrors] = useState({
    length: false,
    number: false,
    special: false
  });

  const validatePassword = (password) => {
    const errors = {
      length: password.length >= 8,
      number: /\d/.test(password),
      special: /[!@#$%^&*(),.?":{}|<>]/.test(password)
    };
    setPasswordErrors(errors);
    return Object.values(errors).every(Boolean);
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    setError('');

    if (!formData.username || !formData.password) {
      setError('아이디와 비밀번호를 모두 입력해주세요.');
      return;
    }

    if (!isLogin && !validatePassword(formData.password)) {
      setError('비밀번호 조건을 모두 만족해주세요.');
      return;
    }

    console.log('Form submitted:', { isLogin, formData });

    try {
      if (isLogin) {
        console.log('Attempting login...');
        const response = await authService.login(
          formData.username,
          formData.password
        );
        console.log('Login success:', response);
        if (response.token) {
          localStorage.setItem('userToken', response.token);
          navigate('/games');
        }
      } else {
        console.log('Attempting join...');
        const response = await authService.join(
          formData.username,
          formData.password
        );
        console.log('Join success:', response);
        
        setIsLogin(true);
        setFormData({ username: '', password: '' });
        alert('회원가입이 완료되었습니다. 로그인해주세요.');
      }
    } catch (error) {
      console.error('Form submission error:', error);
      
      if (error.response?.status === 201 || error.response?.data?.status === 'success') {
        setIsLogin(true);
        setFormData({ username: '', password: '' });
        alert('회원가입이 완료되었습니다. 로그인해주세요.');
        return;
      }

      const errorMessage = error.response?.data?.message 
        || error.message 
        || (isLogin ? '로그인에 실패했습니다.' : '회원가입에 실패했습니다.');
      setError(errorMessage);
    }
  };

  const toggleForm = () => {
    setIsLogin(!isLogin);
    setFormData({ username: '', password: '' });
    setError('');
  };

  return (
    <PageContainer>
      <FormContainer>
        <LogoSection>
          <Logo>♟️</Logo>
          <Title>Chess Master</Title>
        </LogoSection>
        
        <FormSection>
          <FormTitle>{isLogin ? '로그인' : '회원가입'}</FormTitle>
          <Form onSubmit={handleSubmit}>
            <InputGroup>
              <Label>아이디</Label>
              <Input
                type="text"
                placeholder="아이디를 입력하세요"
                value={formData.username}
                onChange={(e) => setFormData({...formData, username: e.target.value})}
              />
            </InputGroup>
            <InputGroup>
              <Label>비밀번호</Label>
              <Input
                type="password"
                placeholder="비밀번호를 입력하세요"
                value={formData.password}
                onChange={(e) => {
                  const newPassword = e.target.value;
                  setFormData({...formData, password: newPassword});
                  if (!isLogin) {
                    validatePassword(newPassword);
                  }
                }}
              />
            </InputGroup>

            {!isLogin && (
              <PasswordValidation>
                <ValidationItem valid={passwordErrors.length}>
                  ✓ 8자 이상
                </ValidationItem>
                <ValidationItem valid={passwordErrors.number}>
                  ✓ 숫자 포함
                </ValidationItem>
                <ValidationItem valid={passwordErrors.special}>
                  ✓ 특수문자 포함
                </ValidationItem>
              </PasswordValidation>
            )}

            {error && <ErrorMessage>{error}</ErrorMessage>}
            <SubmitButton type="submit">
              {isLogin ? '로그인' : '회원가입'}
            </SubmitButton>
          </Form>
          
          <ToggleSection>
            <ToggleText>
              {isLogin ? '아직 계정이 없으신가요?' : '이미 계정이 있으신가요?'}
            </ToggleText>
            <ToggleButton onClick={toggleForm}>
              {isLogin ? '회원가입' : '로그인'}
            </ToggleButton>
          </ToggleSection>
        </FormSection>
      </FormContainer>
    </PageContainer>
  );
};

export default LoginPage; 