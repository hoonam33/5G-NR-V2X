export function contestChange (obj, oldObj) {
    if(!obj) 
      obj = {
        contest_id: parseInt(oldObj.contest_id), 
        contest_name:oldObj.contest_name, 
        contest_organization: oldObj.contest_organization, 
        contest_location: oldObj.contest_location,
        contest_url: oldObj.contest_url,
        sponsor: oldObj.sponsor,
        supervise: oldObj.supervise,
        contest_type: parseInt(oldObj.contest_type),
        contest_status: parseInt(oldObj.contest_status),
        contest_start_time: oldObj.contest_start_time,
        contest_end_time: oldObj.contest_end_time,
        created_time: oldObj.created_time,
        created_date: oldObj.created_date,
        contest_teams: oldObj.contest_teams,
        selected_team: parseInt(oldObj.selected_team)
      }
    else {
      obj.contest_id = parseInt(oldObj.contest_id);
      obj.contest_name = oldObj.contest_name;
      obj.contest_organization = oldObj.contest_organization;
      obj.contest_location = oldObj.contest_location;
      obj.contest_url = oldObj.contest_url;
      obj.sponsor = oldObj.sponsor;
      obj.supervise = oldObj.supervise;
      obj.contest_type = parseInt(oldObj.contest_type);
      obj.contest_status = parseInt(oldObj.contest_status);
      obj.contest_start_time = oldObj.contest_start_time;
      obj.contest_end_time = oldObj.contest_end_time;
      obj.created_time = oldObj.created_time;
      obj.created_date = oldObj.created_date;
      obj.contest_teams = oldObj.contest_teams;
      obj.selected_team = parseInt(oldObj.selected_team);  
    }
}
//기존 Dictionary와 새롭게 들어온 array를 비교하여

//new, replace, delete를 수행함
export function compareArrayAndDictionary (array, key, oldDic, addedFunc, replacedFunc = null, deleteFunc = null) {
    let newDic = {}
    for(let item of array) {
        newDic[item[key]] = item;
    }
    
    let oldIds = Object.keys(oldDic); 
    let newIds = Object.keys(newDic);
    
    let added = newIds.filter(id => oldIds.indexOf(id) < 0); 
    for(let item of added) {
        addedFunc(oldDic, newDic[item], key);        
    }

    if(!replacedFunc)
        return;
    
    let replaced = newIds.filter(id => oldIds.indexOf(id) >= 0);
    for(let item of replaced) {
        replacedFunc(oldDic, newDic[item], key);
    }

    if(!deleteFunc)
        return;
    let removed = oldIds.filter(id => newIds.indexOf(id) < 0);
    for(let item of removed) {
        deleteFunc(oldDic, item);
    }
}

//키가 2개인 경우
export function compareArrayAndDictionaryTwoKey (array, keyOne, keyTwo,  dic, addedFunc, replacedFunc = null) {
    for(let item of array) {
        let valueOne = item[keyOne];
        let valueTwo = item[keyTwo];

        let dicItem = dic[valueOne]?.[valueTwo];
        if(dicItem) {
            if(replacedFunc)
                replacedFunc(dic, valueOne, valueTwo, item)
            continue;
        }
        addedFunc(dic, valueOne, valueTwo, item);
    }
}

export function compareArrayAndDictionaryThreeKey (array, keyOne, keyTwo, keyThree, dic, addedFunc, replacedFunc = null, deleteFunc = null) {
    if(!array) return;
    let newIds = [];
    for(let item of array) {
        let valueOne = item[keyOne];
        let valueTwo = item[keyTwo];
        let valueThree = item[keyThree];

        if(newIds.indexOf(valueOne) < 0) newIds.push(valueOne.toString());

        let dicItem = dic[valueOne]?.[valueTwo];
        if(dicItem) {
            let dicItemTwo = dic[valueOne][valueTwo]?.[valueThree];
            if(dicItemTwo){
                if(replacedFunc)
                replacedFunc(dic, valueOne, valueTwo, valueThree, item)
                continue;
            }
        }
        addedFunc(dic, valueOne, valueTwo, valueThree, item);
    }
    if(!deleteFunc) return;
    let oldIds = Object.keys(dic); 
    let removed = oldIds.filter(id => newIds.indexOf(id) < 0);
    for(let item of removed) {
        deleteFunc(dic, item);
    }
}

//특정 Property들만 바꿈
export function replaceProperties (oldItem, newItem, properties) {
    for(let property of properties) {
        oldItem[property] = newItem[property]
    }
}

export function checkOld (itemTime, timeout) {
    if((new Date()-new Date(itemTime))>timeout)
        return true;
    else
        return false;
}

export function MissionStatusName(type, mission_status) {
    const MissionStatusText = {
        1: {
            name: '-'
        },
        2: {
            name: '진행'
        },
        4: {
            name: 'O'
        },
        5: {
            name: 'X'
        }
    }

    const MissionStatus = {
        0: {
            name: '연결 안됨'
        },
        1: {
            name: '대기'
        },
        2: {
            name: '준비'
        },
        3: {
            name: '시작'
        },
        4: {
            name: '종료'
        },
        5: {
            name: '실격'
        },
        6: {
            name: '정비'
        },
        7: {
            name: '정비 종료'
        },
        8: {
            name: '중지'
        }
    }

    const MissionStatusShow = {
        0: {
            name: '선택'
        },
        1: {
            name: '대기'
        },
        2: {
            name: '준비'
        },
        3: {
            name: '정상' // 시작
        },
        4: {
            name: '종료'
        },
        5: {
            name: '실격'
        },
        6: {
            name: '정비'
        },
        7: {
            name: '정상' // 정비 종료
        },
        8: {
            name: '중지'
        }
    }

    const RoboTaxi = {
        4: {
            name: '출발지 도착 승인'
        },
        5: {
            name: '도착지 도착 승인'
        }
    }

    const RoboTaxi_Result = {
        4: {
            name: '성공'
        },
        5: {
            name: '실패'
        }
    }

    const ItemGet = {
        1: {
            name: '가점'
        },
        2: {
            name: '감점'
        },
        3: {
            name: '가속 주행'
        }
    }
    
    if(type == 'MissionStatus') return MissionStatus[mission_status].name + '';
    else if(type == 'MissionStatusShow') return MissionStatusShow[mission_status].name + '';
    else if(type == 'RoboTaxi') return RoboTaxi[mission_status].name + '';
    else if(type == 'RoboTaxi_Result') return RoboTaxi_Result[mission_status].name + '';
    else if(type == 'ItemGet') return ItemGet[mission_status].name + '';
    else if(type == 'MissionStatusText') return MissionStatusText[mission_status].name + '';
}