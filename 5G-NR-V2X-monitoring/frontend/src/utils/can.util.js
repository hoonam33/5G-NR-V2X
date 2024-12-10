// 존재하는 메시지 아이디 확인
const messageID = [0x156, 0x157, 0x710, 0x711, 0x712, 0x713]
//            DEC [ 343,   343,   1808,  1809,  1810,  1811]

class message0x156 {
    constructor() {

    }
}

export function makeUseCanArray(text) {
    var strs = text.split('\n')
    var returnArray = []

    strs.forEach(str => {
        if (!str || str.trim() === "") return
        var addObject = {}
        var strParts = str.split(' ')

        if (strParts.length !== 3) return
        if (
            (!strParts[0] || strParts[0].trim() === "") ||
            (!strParts[1] || strParts[1].trim() === "") ||
            (!strParts[2] || strParts[2].trim() === "")
        ) return

        addObject.time = strParts[0].replace(/[()]/g, '').trim()

        var dataParts = strParts[2].split('#')
        if (dataParts.length !== 2) return
        if (
            (!dataParts[0] || dataParts[0].trim() === "") ||
            (!dataParts[1] || dataParts[1].trim() === "")
        ) return
        addObject.messageID = parseInt(dataParts[0], 16)
        addObject.data = dataParts[1].trim()

        if (messageID.includes(addObject.messageID)) {
            returnArray.push(addObject)
        }
    });

    return returnArray
}