import http from 'k6/http';
import { sleep, check } from 'k6';

export const options = {
    stages: [
        { duration: '30s', target: 10 },  // 30초 동안 10명으로 시작
        { duration: '30s', target: 200 }, // 30초 동안 200명으로 급증
        { duration: '1m', target: 200 },  // 1분 동안 200명 유지
        { duration: '30s', target: 10 },  // 30초 동안 10명으로 감소
    ],
};

export default function () {
    const tokenPayload = JSON.stringify({
        token: 'HbLm48IlQ1Cq8XAuze6YdHdtbIIbenLC',
    });

    const params = {
        headers: {
            'Content-Type': 'application/json',
        },
    };

    const res = http.post('http://localhost:8080/room', tokenPayload, params);

    // 응답 검증
    check(res, {
        'status is 200': (r) => r.status === 200,
        'response time is < 200ms': (r) => r.timings.duration < 200,
    });

    sleep(1); // 부하를 조정하기 위해 1초 대기
}