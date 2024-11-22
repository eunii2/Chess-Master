import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  PageContainer,
  NavBar,
  Logo,
  LogoText,
  LogoutButton,
  UserProfileImage,
  Content,
  Header,
  Title,
  CreateRoomButton,
  RoomGrid,
  RoomCard,
  RoomNameContainer,
  RoomName,
  RoomStatus,
  JoinButton,
  LoadingMessage,
  ErrorMessage,
  Modal,
  ModalContent,
  ModalTitle,
  ModalInput,
  ModalButtonGroup,
  ModalButton,
  UploadArea,
  UploadText,
  UploadInput,
  UploadImage,
  ProfileContainer,
  RoomProfileImage
} from '../styles/GameListPage.styles';
import { gameService } from '../services/gameService';
import { authService } from '../services/authService';
import axios from 'axios';
import { S3Client, PutObjectCommand } from "@aws-sdk/client-s3";
import defaultProfileImage from '../assets/default.png';

const GameListPage = () => {
  const navigate = useNavigate();
  const [rooms, setRooms] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [isCreatingRoom, setIsCreatingRoom] = useState(false);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [newRoomName, setNewRoomName] = useState('');
  const [profileImage, setProfileImage] = useState('');
  const [isProfileModalOpen, setIsProfileModalOpen] = useState(false);
  const [selectedImage, setSelectedImage] = useState(null); 

  const bucketName = 'fpbusket'; 
  const region = 'ap-northeast-2'; 

  const s3 = new S3Client({
    region: region,
    credentials: {
      accessKeyId: 'AKIATCKANK5YUGRICF5W', // 자신의 AWS Access Key ID
      secretAccessKey: '14t1CRMitOidiamIjgM+3HVTuRDAF3Sq5cxsL58U',
    },
  });

  const handleLogout = () => {
    authService.logout();
    navigate('/');
  };

  const handleProfileImageClick = () => {
    setIsProfileModalOpen(true);
  };

  const closeProfileModal = () => {
    setIsProfileModalOpen(false);
    setSelectedImage(null);
  };

  const handleImageChange = (e) => {
    if (e.target.files && e.target.files[0]) {
      setSelectedImage(e.target.files[0]);
    }
  };

  const handleUploadAreaClick = () => {
    document.getElementById('imageUploadInput').click();
  };

  const handleSaveProfileImage = async () => {
    if (!selectedImage) {
      alert('이미지를 선택해주세요.');
      return;
    }

    const fileName = `${Date.now()}-${selectedImage.name}`;

    const params = {
      Bucket: bucketName,
      Key: fileName,
      Body: selectedImage,
      ContentType: selectedImage.type,
    };

    try {
      const command = new PutObjectCommand(params);
      await s3.send(command);

      const imageUrl = `https://${bucketName}.s3.${region}.amazonaws.com/${fileName}`;

      console.log('Uploaded Image URL:', imageUrl);
      alert('이미지가 성공적으로 업로드되었습니다.');

      const token = localStorage.getItem('userToken');
      const postData = {
        token: token,
        image_address: imageUrl,
      };

      await axios.post('http://localhost:8080/image/upload', postData);

      setProfileImage(imageUrl);
      closeProfileModal();
    } catch (error) {
      console.error('이미지 업로드 에러:', error);
      alert('이미지 업로드에 실패했습니다.');
    }
  };

  const handleCreateRoom = async () => {
    if (!newRoomName.trim()) {
      alert('방 이름을 입력해주세요.');
      return;
    }

    try {
      const token = localStorage.getItem('userToken');
      const response = await gameService.createRoom(token, newRoomName);
      if (response.status === 'success') {
        setIsModalOpen(false);
        setNewRoomName('');
        navigate(`/games/${response.room_id}`);
      }
    } catch (err) {
      alert('방 생성에 실패했습니다.');
    }
  };

  const handleJoinRoom = async (roomId) => {
    try {
      const token = localStorage.getItem('userToken');
      const response = await gameService.joinRoom(roomId, token);
      
      if (response.status === 'success') {
        navigate(`/games/${response.room_id}`);
      } else {
        alert(response.message);
      }
    } catch (err) {
      alert('방 입장에 실패했습니다.');
    }
  };

  useEffect(() => {
    const fetchRooms = async () => {
      try {
        const token = localStorage.getItem('userToken');
        const response = await gameService.getRoomList(token);
        console.log('Fetched Rooms:', response);
        setRooms(response.rooms);
        setProfileImage(response.profile_image || defaultProfileImage);
        setLoading(false);
      } catch (err) {
        setError('방 목록을 불러오는데 실패했습니다.');
        setLoading(false);
      }
    };

    fetchRooms();
  }, []);

  if (loading) return <LoadingMessage>로딩 중...</LoadingMessage>;
  if (error) return <ErrorMessage>{error}</ErrorMessage>;

  return (
    <PageContainer>
      <NavBar>
        <Logo>
          ♟️ <LogoText>Chess Master</LogoText>
        </Logo>
        <ProfileContainer>
          <UserProfileImage
            src={profileImage || defaultProfileImage}
            alt="Profile"
            onClick={handleProfileImageClick}
            style={{ cursor: 'pointer' }}
          />
          <LogoutButton onClick={handleLogout}>로그아웃</LogoutButton>
        </ProfileContainer>
      </NavBar>
      <Content>
        <Header>
          <Title>게임 방 목록</Title>
          <CreateRoomButton onClick={() => setIsModalOpen(true)}>
            + 새로운 방 만들기
          </CreateRoomButton>
        </Header>
        <RoomGrid>
          {rooms.map((room) => {
            console.log(`Room ID: ${room.room_id}, Joined: ${room.joined}`);
            return (
              <RoomCard key={room.room_id} joined={room.joined}>
                <RoomNameContainer>
                  <RoomName>{room.room_name}</RoomName>
                  <RoomProfileImage src={room.profile_image || defaultProfileImage} alt="Room Creator" /> {
                    
                  }
                </RoomNameContainer>
                <RoomStatus joined={room.joined}>
                  {room.joined ? '입장 불가' : '참가 가능'}
                </RoomStatus>
                <JoinButton 
                  disabled={room.joined}
                  onClick={() => !room.joined && handleJoinRoom(room.room_id)}
                >
                  {room.joined ? '입장 불가' : '입장하기'}
                </JoinButton>
              </RoomCard>
            );
          })}
        </RoomGrid>
      </Content>

      {isModalOpen && (
        <Modal>
          <ModalContent>
            <ModalTitle>새로운 방 만들기</ModalTitle>
            <ModalInput
              type="text"
              placeholder="방 이름을 입력하세요"
              value={newRoomName}
              onChange={(e) => setNewRoomName(e.target.value)}
              autoFocus
            />
            <ModalButtonGroup>
              <ModalButton onClick={() => {
                setIsModalOpen(false);
                setNewRoomName('');
              }}>
                취소
              </ModalButton>
              <ModalButton primary onClick={handleCreateRoom}>
                만들기
              </ModalButton>
            </ModalButtonGroup>
          </ModalContent>
        </Modal>
      )}

      {isProfileModalOpen && (
        <Modal>
          <ModalContent>
            <ModalTitle>프로필 이미지 변경</ModalTitle>
            <UploadArea onClick={handleUploadAreaClick}>
              {selectedImage ? (
                <UploadImage src={URL.createObjectURL(selectedImage)} alt="Selected" />
              ) : (
                <UploadText>클릭하여 이미지를 업로드하세요.</UploadText>
              )}
              <UploadInput
                id="imageUploadInput"
                type="file"
                accept="image/*"
                onChange={handleImageChange}
              />
            </UploadArea>
            <ModalButtonGroup>
              <ModalButton onClick={closeProfileModal}>닫기</ModalButton>
              <ModalButton primary onClick={handleSaveProfileImage}>저장</ModalButton>
            </ModalButtonGroup>
          </ModalContent>
        </Modal>
      )}
    </PageContainer>
  );
};

export default GameListPage;



