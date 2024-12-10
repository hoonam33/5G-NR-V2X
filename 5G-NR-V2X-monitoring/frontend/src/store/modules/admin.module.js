import AdminService from "../../services/admin.service";

export const admin = {
    state: {
    },
    actions: {
        //회원가입
        register(context, user) {
            return AdminService.register(user)
        },
        //회원삭제
        deleteUser(context, user_id) {
            return AdminService.deleteUser(user_id)
        }
    },
    getters: {
        //모든 유저 가져오기
        getUsers: (state) => {
            return AdminService.getUsers()
        },
        //특정 회원에 대한 로그 가져오기
        getUserLog: (state) => (user_id) => {
            return AdminService.getUserLog(user_id)
        }
    }
}
