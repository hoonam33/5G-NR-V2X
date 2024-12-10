const jwt = require("jsonwebtoken");
var bcrypt = require("bcryptjs");
const dbAdapter = require('../db/dbAdapter');

module.exports = function(app) {
    const db = dbAdapter.GetDB();
    const config = require("../config/auth.config");

    app.post('/auth/login', async(req, res, next) => {
        let user = {user_id: null, user_ip: null};
        try {
            user = req.body;
            const findUser = (await db.query('SELECT * FROM users WHERE user_id = $1 LIMIT 1', [user.user_id])).rows[0]; 
            
            if (!findUser) {   
                res.status(404).send({
                    accessToken: null,
                    message: "Login Failed: information is not correct."
                });
                return writeUserLog(db, user, "Failed: wrong id");
            }

            let isValid = bcrypt.compareSync(user.user_pw, findUser.user_pw);
            if(!isValid) {
                res.status(401).send({
                    accessToken: null,
                    message: "Login Failed: information is not correct."
                });
                return writeUserLog(db, user, "Failed: wrong pw");
            } else {
                var token = jwt.sign({ user_id: findUser.user_id }, config.secret, { 
                    expiresIn: 86400 // 24 hours 
                });
                
                writeUserLog(db, user, "Login Success")
                return res.status(200).send({
                    user_id: findUser.user_id, 
                    user_role: findRoleByNum(db, findUser.user_role),
                    accessToken: token
                });
            }
        } catch (err) {
            console.log(err);
            res.status(500).send({ message: 'server error' });
            return writeUserLog(db, user, "Failed: server problem");
        }
    })
}

const writeUserLog = (db, user, logMsg) => {
    db.query('INSERT INTO users_log ' +
                    '(user_id, user_ip, user_status) VALUES ' +
                    '($1, $2, $3)',
                    [user['user_id'], user['user_ip'], logMsg]);
}

const findRoleByNum = (db, user_role) => {
    if(user_role in db.roles) {
        return db.roles[user_role].role_kor;
    }
    return null;
}