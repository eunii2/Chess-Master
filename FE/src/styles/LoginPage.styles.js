import styled, { keyframes } from 'styled-components';

const fadeIn = keyframes`
  from { opacity: 0; transform: translateY(-20px); }
  to { opacity: 1; transform: translateY(0); }
`;

export const PageContainer = styled.div`
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  width: 100%;
  background: #f5f7fa;
  padding: 20px;
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
`;

export const FormContainer = styled.div`
  width: 100%;
  max-width: 420px;
  background: white;
  border-radius: 16px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
  overflow: hidden;
  animation: ${fadeIn} 0.6s ease-out;
  margin: 0 auto;

  @media (max-width: 768px) {
    max-width: 100%;
    margin: 20px;
  }
`;

export const LogoSection = styled.div`
  text-align: center;
  padding: 35px 0;
  background: white;
  color: #2c3e50;
`;

export const Logo = styled.div`
  font-size: 42px;
  margin-bottom: 12px;
  animation: ${fadeIn} 0.8s ease-out;
`;

export const Title = styled.h1`
  font-size: 22px;
  font-weight: 500;
  color: #2c3e50;
  margin: 0;
  letter-spacing: 0.5px;
`;

export const FormSection = styled.div`
  padding: 32px 40px;
  background: white;

  @media (max-width: 768px) {
    padding: 24px 20px;
  }
`;

export const FormTitle = styled.h2`
  text-align: center;
  color: #2c3e50;
  margin-bottom: 32px;
  font-size: 20px;
  font-weight: 500;
`;

export const Form = styled.form`
  display: flex;
  flex-direction: column;
  gap: 20px;
`;

export const InputGroup = styled.div`
  display: flex;
  flex-direction: column;
  gap: 6px;
`;

export const Label = styled.label`
  font-size: 13px;
  color: #7f8c8d;
  font-weight: 500;
  margin-left: 4px;
`;

export const Input = styled.input`
  padding: 12px 16px;
  border: 1.5px solid #e6e9ec;
  border-radius: 8px;
  font-size: 15px;
  transition: all 0.2s ease;
  background: #f8fafc;
  color: #2c3e50;

  &:focus {
    outline: none;
    border-color: #3498db;
    background: white;
    box-shadow: 0 0 0 3px rgba(52, 152, 219, 0.1);
  }

  &::placeholder {
    color: #95a5a6;
  }
`;

export const SubmitButton = styled.button`
  padding: 14px;
  background: #3498db;
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 15px;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s ease;
  margin-top: 10px;

  &:hover {
    background: #2980b9;
    transform: translateY(-1px);
    box-shadow: 0 4px 12px rgba(52, 152, 219, 0.15);
  }

  &:active {
    transform: translateY(0);
  }
`;

export const ToggleSection = styled.div`
  margin-top: 28px;
  text-align: center;
  padding-top: 20px;
  border-top: 1px solid #f0f2f4;
`;

export const ToggleText = styled.span`
  color: #7f8c8d;
  font-size: 14px;
  margin-right: 8px;
`;

export const ToggleButton = styled.button`
  background: none;
  border: none;
  color: #3498db;
  font-weight: 500;
  cursor: pointer;
  padding: 4px 8px;
  font-size: 14px;
  transition: all 0.2s ease;

  &:hover {
    color: #2980b9;
    text-decoration: underline;
  }
`;

export const ErrorMessage = styled.div`
  color: #e74c3c;
  font-size: 14px;
  text-align: center;
  margin-top: 8px;
  padding: 8px;
  background: #fdeaea;
  border-radius: 4px;
`;

export const PasswordValidation = styled.div`
  margin-top: 8px;
  padding: 12px 16px;
  background: #f8fafc;
  border-radius: 8px;
  border: 1px solid #e6e9ec;
`;

export const ValidationItem = styled.div`
  display: flex;
  align-items: center;
  gap: 8px;
  color: ${props => props.valid ? '#2ecc71' : '#95a5a6'};
  font-size: 13px;
  margin: 6px 0;
  transition: color 0.2s ease;

  &:before {
    content: '${props => props.valid ? '✓' : '○'}';
    font-size: 14px;
  }
`; 