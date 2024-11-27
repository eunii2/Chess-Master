# 체스 서버 프로젝트

## 프로젝트 구조
```
BE
├── config                      
│   └── config.h               # 설정 파일
├── data 
│   ├── game                   # 게임 데이터
│   ├── image                  # 이미지 링크 
│   ├── rooms                  # 게임 방 데이터
│   └── user                   # 유저 데이터
│       └── user_image.txt
├── libs                      # cJSON을 포함한 라이브러리
├── src
│   ├── admin                 # 관리자 기능
│   ├── auth                  # 유저 관리(인증) 기능
│   ├── chat                  # 채팅 기능
│   ├── game                  # 게임 기능
│   ├── image                 # 이미지 관련 기능
│   ├── room                  # 게임 방 관련 기능
│   ├── server                # 서버 로직
│   └── utils                 # 유틸리티 함수(ex. 토큰 생성)
├── main.c                    # 메인 함수 (프로그램 시작점)
├── CMakeLists.txt            # CMake 설정 파일
└── README.md                 # 프로젝트 설명
```


## 시작하기

1. 필요한 패키지 설치:
    ```sh
    sudo apt update
    sudo apt install cmake build-essential
    ```

2. 프로젝트 디렉토리에서 `build` 폴더 생성:
    ```sh
    mkdir build
    cd build
    ```

3. CMake를 사용하여 빌드 시스템 생성:
    ```sh
    cmake ..
    ```

4. 프로젝트 빌드:
    ```sh
    make
    ```

5. 서버 실행:
    ```sh
    ./chess_server
    ```

