<template>
    <div class="tab">
        <table>
            <tr>
                <button class="btnThird" @click="clickButton('file')">File</button>
            </tr>
            <tr>
                <button class="btnThird" @click="clickButton('can')">List</button>
            </tr>
        </table>
    </div>
    <div id="fileloader" v-if="state.showDiv.fileLoad">
        <v-row>
            <table class=table>
                <thead class="table-header">
                    <tr>
                        <th><button class="btn-clear" @click="state.fileDialog.show = true">파일불러오기</button></th>
                    </tr>
                </thead>
            </table>
        </v-row>
        <v-row>
            <table class=table>
                <thead class="table-header">
                    <tr>
                        <th>파일명</th>
                        <th>크기</th>
                    </tr>
                </thead>
                <tbody>
                    <tr @click="clickRow(file)" v-for="file in state.registerFiles" :key="file.name">
                        <td>{{ file.name }}</td>
                        <td>{{ file.size }}</td>
                    </tr>
                </tbody>
            </table>
        </v-row>
    </div>
    <div id="canlist" v-if="state.showDiv.canlist">
        <v-row>
            <table class=table>
                <thead class="table-header">
                    <tr>
                        <th>Index</th>
                        <th>Time</th>
                        <th>MessageID</th>
                        <th>Data</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="(row, index) in state.viewData" :key="row.time">
                        <td>{{ index }}</td>
                        <td>{{ row.time }}</td>
                        <td>{{ row.messageID }}</td>
                        <td>{{ row.data }}</td>
                    </tr>
                </tbody>
            </table>
        </v-row>
    </div>
    <v-dialog v-model="state.fileDialog.show">
        <v-card class="dialog-card" style="height: 100%;">
            <v-card-title> File Upload </v-card-title>
            <v-card-text> 원하는 텍스트 파일을 업로드하세요. </v-card-text>
            <v-file-input v-model="state.fileDialog.files" accept=".log, .txt" multiple outlined label="*.log/*txt file"
                style="width:400px">
            </v-file-input>
            <v-card-actions>
                <v-spacer></v-spacer>
                <v-btn color="white" text @click="getFile"> 삽입 </v-btn>
                <v-btn color="white" text @click="state.fileDialog.show = false"> 닫기 </v-btn>
            </v-card-actions>
        </v-card>
    </v-dialog>
    <v-dialog v-model="state.loadingDialog.show">
        <v-card class="dialog-card" style="height: 100%;">
            <v-card-text> {{ state.loadingDialog.title }} </v-card-text>
            <div class="loading-container">
                <div class="loading"></div>
                <div id="loading-text">loading</div>
            </div>
        </v-card>
    </v-dialog>
</template>

<script>
import { reactive, onMounted } from 'vue'
import { useStore } from 'vuex'
import { makeUseCanArray } from '../../utils/can.util.js'
export default {
    components: {},
    emits: ['UpdateViewData'],

    setup(props, { emit }) {
        const store = useStore()
        const state = reactive({
            showDiv: {
                fileLoad: false,
                canlist: false
            },
            loadingDialog: {
                show: false,
                title: undefined
            },
            fileDialog: {
                show: false,
                files: undefined,
                filename: undefined,
                path: undefined
            },
            registerFiles: [],
            viewData: undefined
        })

        onMounted(() => {
            //testProcess()
        })

        const testProcess = async () => {
            const fetchStart = new Date()

            // TODO TEST

            const fetchEnd = new Date().getTime() - fetchStart.getTime()
            if (fetchEnd < 3000) {
                setTimeout(await testProcess, 3000 - fetchEnd)
            } else {
                setTimeout(await testProcess, 0)
            }
        }

        const getFile = () => {
            const allowedExtensions = ['log', 'txt'];

            const validFiles = state.fileDialog.files.filter(file => {
                const fileExtension = file.name.split('.').pop().toLowerCase();
                return allowedExtensions.includes(fileExtension);
            });

            validFiles.forEach(file => {
                var fileobj = {}
                fileobj.file = file
                fileobj.name = file.name
                fileobj.size = file.size
                state.registerFiles.push(fileobj)
                getFileText(file)
            });

            state.fileDialog.show = false
        }

        const MAX_FILE_SIZE = 500 * 1024 * 1024;
        var chunkString = ""
        var chunkParts = []
        const getFileText = (file) => {
            var reader = new FileReader()
            state.loadingDialog.title = file.name + ' File Load'
            state.loadingDialog.show = true

            if (file.size < MAX_FILE_SIZE) {
                reader.onload = () => {
                    saveFileText(file.name, reader.result, 1)
                }

                reader.readAsText(file)
            }
            else {
                const chunkSize = 1024 * 1024
                let offset = 0
                let chunkIndex = 0

                reader.onload = (event) => {
                    chunkString += event.target.result

                    offset += chunkSize;
                    if (offset < file.size && chunkIndex <= 500) {
                        readNextChunk()
                        if (chunkIndex === 500) {
                            chunkIndex = 0
                            chunkParts.push(chunkString)
                            chunkString = ""
                        }
                        else chunkIndex++
                    } else {
                        if (chunkIndex > 0) {
                            chunkParts.push(chunkString)
                        }


                        saveFileText(file.name, chunkParts, 2)

                        chunkIndex = 0
                        chunkString = ""
                        chunkParts = []
                    }
                }

                const readNextChunk = () => {
                    const chunk = file.slice(offset, offset + chunkSize)
                    reader.readAsText(chunk)
                };
                readNextChunk()
            }
        }

        const saveFileText = (filename, result, type) => {
            var fileobj = state.registerFiles.find(rfile => rfile.name == filename)
            fileobj.data = []

            if (type === 1) {
                fileobj.data = makeUseCanArray(result)
            }
            else if (type === 2) {
                var inputData = []
                result.forEach(str => {
                    var resultArray = makeUseCanArray(str)
                    inputData = inputData.concat(resultArray)
                })
                fileobj.data = inputData
            }

            state.viewData = fileobj.data
            state.loadingDialog.show = false
        }

        const clickRow = (file) => {
            var fileobj = state.registerFiles.find(rfile => rfile.name == file.name)
            state.viewData = fileobj.data

            clickButton('can')
        }

        const clickButton = (typeStr) => {
            if (typeStr === 'file') {
                if (!state.showDiv.fileLoad) {
                    state.showDiv.canlist = false
                    state.showDiv.fileLoad = true
                }
                else state.showDiv.fileLoad = false
            }
            else if (typeStr === 'can') {
                if (!state.showDiv.canlist) {
                    state.loadingDialog.title = 'Load Can Data List'
                    state.loadingDialog.show = true

                    setTimeout(() => {
                        state.showDiv.fileLoad = false;
                        state.showDiv.canlist = true;

                        state.loadingDialog.show = false;
                    }, 1000);
                }
                else state.showDiv.canlist = false
            }
        }

        return { state, store, getFile, clickRow, clickButton }
    },
    methods: {},
}
</script>

<style lang="scss" scoped>
#fileloader {
    position: absolute;
    top: 20px;
    left: 70px;
    width: 500px;
    height: 800px;
    padding: 0;
}

#canlist {
    position: absolute;
    top: 20px;
    left: 70px;
    width: 610px;
    max-height: 800px;
    overflow-y: auto;
    overflow-x: hidden;
    padding: 5px 0px 0 15px;
}

.v-toolbar-title {
    font-weight: bold;
}

#link {
    color: #E45635;
    display: block;
    font: 12px "Helvetica Neue", Helvetica, Arial, sans-serif;
    text-align: center;
    text-decoration: none;
}

#link:hover {
    color: #CCCCCC
}

#link,
#link:hover {
    -webkit-transition: color 0.5s ease-out;
    -moz-transition: color 0.5s ease-out;
    -ms-transition: color 0.5s ease-out;
    -o-transition: color 0.5s ease-out;
    transition: color 0.5s ease-out;
}

/** BEGIN CSS **/
body {
    background: #333333;
}

@keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@-moz-keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@-webkit-keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@-o-keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@-moz-keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@-webkit-keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@-o-keyframes rotate-loading {
    0% {
        transform: rotate(0deg);
        -ms-transform: rotate(0deg);
        -webkit-transform: rotate(0deg);
        -o-transform: rotate(0deg);
        -moz-transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
        -ms-transform: rotate(360deg);
        -webkit-transform: rotate(360deg);
        -o-transform: rotate(360deg);
        -moz-transform: rotate(360deg);
    }
}

@keyframes loading-text-opacity {
    0% {
        opacity: 0
    }

    20% {
        opacity: 0
    }

    50% {
        opacity: 1
    }

    100% {
        opacity: 0
    }
}

@-moz-keyframes loading-text-opacity {
    0% {
        opacity: 0
    }

    20% {
        opacity: 0
    }

    50% {
        opacity: 1
    }

    100% {
        opacity: 0
    }
}

@-webkit-keyframes loading-text-opacity {
    0% {
        opacity: 0
    }

    20% {
        opacity: 0
    }

    50% {
        opacity: 1
    }

    100% {
        opacity: 0
    }
}

@-o-keyframes loading-text-opacity {
    0% {
        opacity: 0
    }

    20% {
        opacity: 0
    }

    50% {
        opacity: 1
    }

    100% {
        opacity: 0
    }
}

.loading-container,
.loading {
    height: 100px;
    position: relative;
    width: 100px;
    border-radius: 100%;
}


.loading-container {
    margin: 40px auto
}

.loading {
    border: 2px solid transparent;
    border-color: transparent #fff transparent #FFF;
    -moz-animation: rotate-loading 1.5s linear 0s infinite normal;
    -moz-transform-origin: 50% 50%;
    -o-animation: rotate-loading 1.5s linear 0s infinite normal;
    -o-transform-origin: 50% 50%;
    -webkit-animation: rotate-loading 1.5s linear 0s infinite normal;
    -webkit-transform-origin: 50% 50%;
    animation: rotate-loading 1.5s linear 0s infinite normal;
    transform-origin: 50% 50%;
}

.loading-container:hover .loading {
    border-color: transparent #E45635 transparent #E45635;
}

.loading-container:hover .loading,
.loading-container .loading {
    -webkit-transition: all 0.5s ease-in-out;
    -moz-transition: all 0.5s ease-in-out;
    -ms-transition: all 0.5s ease-in-out;
    -o-transition: all 0.5s ease-in-out;
    transition: all 0.5s ease-in-out;
}

#loading-text {
    -moz-animation: loading-text-opacity 2s linear 0s infinite normal;
    -o-animation: loading-text-opacity 2s linear 0s infinite normal;
    -webkit-animation: loading-text-opacity 2s linear 0s infinite normal;
    animation: loading-text-opacity 2s linear 0s infinite normal;
    color: #ffffff;
    font-family: "Helvetica Neue, " Helvetica", " "arial";
    font-size: 10px;
    font-weight: bold;
    margin-top: 45px;
    opacity: 0;
    position: absolute;
    text-align: center;
    text-transform: uppercase;
    top: 0;
    width: 100px;
}

.tab {
    position: absolute;
    left: 2px;
    top: 5px;
    width: 50px;
    height: 100%;
}

$blue: #8e44ad;

.btnThird {
    width: 50px;
    height: 50px;
    border-radius: 5px;
    border-color: $blue;
    color: #fff;
    box-shadow: 0 0 40px 40px $blue inset, 0 0 0 0 $blue;
    transition: all 150ms ease-in-out;

    &:hover {
        box-shadow: 0 0 10px 0 $blue inset, 0 0 10px 4px $blue;
    }
}
</style>