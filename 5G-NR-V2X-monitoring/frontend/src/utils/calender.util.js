function toDate (date) {
    if(!date) {
        return undefined
    } if (date === void 0) {
        return new Date(0);
    } if (date instanceof Date) {
        return date;
    } else {
        return new Date(date);
    }
}

export function serverTime() {
    var xmlHttp;
    // XMLHttpRequest는 대부분의 브라우저 지원가능: https://caniuse.com/#search=XMLHttpRequest
    if (window.XMLHttpRequest) {
        xmlHttp = new XMLHttpRequest();            
    // InternetExplorer ActiveXObject 지원하는 경우
    }
    /*
        xmlHttp.open : 헤더 정보만 받기 위해 HEAD방식으로 요청.
            - HEAD 정보만 요청, async (비동기) false 옵션을 줘서 동기로 요청.
            - Synchronous XMLHttpRequest는 deprecated되었기 때문에 사용을 권장하지 않음.
        
        xmlHttp.setRequestHeader : reqeust header를 설정
            - 안보내도 되고, xmlHttp.setRequestHeader("Authorization", "ABCDEFG"); 이런식으로 보내도 된다.
        
        * xmlHttp.send : HTTP 요청
    */
    xmlHttp.open('HEAD', window.location.href.toString(), false);
    xmlHttp.setRequestHeader("Content-Type", "text/html");
    xmlHttp.send('');

    // xmlHttp.getAllResponseHeaders() : 모든 헤더 정보 조회
    return xmlHttp.getResponseHeader("Date"); // type: string
}

export function dateFormat (date, format_) {
    var d = toDate(date);
    if(!d)
        return '-';
    return format_
    .replace(/Y/gm, ('000000' + (d.getFullYear().toString()%100000)).substr(-4))
    .replace(/m/gm, ('0' + (d.getMonth() + 1)).substr(-2))
    .replace(/d/gm, ('0' + (d.getDate() + 0)).substr(-2))
    .replace(/H/gm, ('0' + (d.getHours())).substr(-2))
    .replace(/i/gm, ('0' + (d.getMinutes() + 0)).substr(-2))
    .replace(/s/gm, ('0' + (d.getSeconds() + 0)).substr(-2))
    .replace(/v/gm, ('0000' + (d.getMilliseconds() % 1000)).substr(-3));
}

export function dateDiff (t0, t1) {
    let d = (new Date(t1)) - (new Date(t0));
    let weekdays     = Math.floor(d/1000/60/60/24/7);
    let days         = Math.floor(d/1000/60/60/24 - weekdays*7);
    let hours        = Math.floor(d/1000/60/60    - weekdays*7*24            - days*24);
    let minutes      = Math.floor(d/1000/60       - weekdays*7*24*60         - days*24*60         - hours*60);
    let seconds      = Math.floor(d/1000          - weekdays*7*24*60*60      - days*24*60*60      - hours*60*60      - minutes*60);
    let milliseconds = Math.floor(d               - weekdays*7*24*60*60*1000 - days*24*60*60*1000 - hours*60*60*1000 - minutes*60*1000 - seconds*1000);
    let t = String(hours).padStart(2,0) + ':' + 
            String(minutes).padStart(2,0) + ':' + 
            String(seconds).padStart(2,0) + '.' + String(milliseconds).padEnd(2,0).slice(0,2);
    return t;
}

export function millisecondsToDigitalFormat(d) {
    let weekdays     = Math.floor(d/1000/60/60/24/7);
    let days         = Math.floor(d/1000/60/60/24 - weekdays*7);
    let hours        = Math.floor(d/1000/60/60    - weekdays*7*24            - days*24);
    let minutes      = Math.floor(d/1000/60       - weekdays*7*24*60         - days*24*60         - hours*60);
    let seconds      = Math.floor(d/1000          - weekdays*7*24*60*60      - days*24*60*60      - hours*60*60      - minutes*60);
    let milliseconds = Math.floor(d               - weekdays*7*24*60*60*1000 - days*24*60*60*1000 - hours*60*60*1000 - minutes*60*1000 - seconds*1000);
    let t = String(hours).padStart(2,0) + ':' + 
            String(minutes).padStart(2,0) + ':' + 
            String(seconds).padStart(2,0) + '.' + String(milliseconds).padEnd(2,0).slice(0,2);
    return t;
}

export function millisecondsToMinutesDigitalFormat(d) {
    if(d < 0) return '00:00.00';
    let weekdays     = Math.floor(d/1000/60/60/24/7);
    let days         = Math.floor(d/1000/60/60/24 - weekdays*7);
    let hours        = Math.floor(d/1000/60/60    - weekdays*7*24            - days*24);
    let minutes      = Math.floor(d/1000/60       - weekdays*7*24*60         - days*24*60         - hours*60);
    let seconds      = Math.floor(d/1000          - weekdays*7*24*60*60      - days*24*60*60      - hours*60*60      - minutes*60);
    let milliseconds = Math.floor(d               - weekdays*7*24*60*60*1000 - days*24*60*60*1000 - hours*60*60*1000 - minutes*60*1000 - seconds*1000);
    let t = String(minutes).padStart(2,0) + ':' + 
            String(seconds).padStart(2,0) + '.' + String(milliseconds).padEnd(2,0).slice(0,2);
    return t;
}