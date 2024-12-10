export const api = {
    state: () => ({
        fetchTime: new Date(),
    }),

    mutations: {
        setFetchTime(state, fetchTime) {
            state.fetchTime = fetchTime;
        },
    },

    actions: {
        
    },

    getters: {
        getFetchTime: (state) => {
            return state.fetchTime;
        }
    }
}