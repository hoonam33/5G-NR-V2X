const { Pool } = require('pg');

var DataBase = function () { };

module.exports = DataBase;

DataBase.GetDB = function () {
    if (typeof DataBase.db === 'undefined') {
        DataBase.InitDB();
    }
    return DataBase.db;
};

DataBase.InitDB = function () {
    let db = {};
    db.options = require("../config/db.config").database['test_db'];

    db.poolTable = {};
    db.getPool = (user, password) => {
        const key = '' + user + '|' + password;
        if (db.poolTable.hasOwnProperty(key))
            return db.poolTable[key];
        return db.poolTable[key] = new Pool({
            user: user,
            password: password,
            host: db.options.host,
            port: db.options.port,
            database: db.options.database,
        });
    };

    db.getQuery = (user, password, text, params) => {
        const start = Date.now()
        let result = db.getPool(user, password).query(text, params);
        return result;
    }

    db.query = (text, params,) => db.getQuery(db.options.user, db.options.password, text, params)
    db.pool = db.getPool(db.options.user, db.options.password);
    db.poolAs = db.getPool;
    db.queryAs = db.getQuery;

    db.query('SELECT * FROM users_role').then(response => {
        db.roles = {};
        for (let role of response.rows) {
            db.roles[role.role_num] = role;
        }
    })

    DataBase.db = db;
}

DataBase.Disconnect = function () {
    if (DataBase.db) {
        DataBase.db.close();
    }
}