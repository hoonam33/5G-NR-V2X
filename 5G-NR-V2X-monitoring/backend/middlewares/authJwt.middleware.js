const jwt = require("jsonwebtoken");
const config = require("../config/auth.config.js");
const dbAdapter = require('../db/dbAdapter');

verifyToken = (req, res, next) => {
    let token = req.headers["x-access-token"];
    
    if (!token) {
        return res.status(403).send({
            message: "No token provided!"
        });
    }
    jwt.verify(token, config.secret, (err, decoded) => {
        if (err) {
            return res.status(401).send({
                message: "Unauthorized!"
            });
        }
        req.user_id = decoded.user_id;
        next();
        return;
    });
};

findRoleById = (req, res, next) => {
    const db = dbAdapter.GetDB();
    try {
        db.query('SELECT user_role FROM users WHERE user_id =$1',  [req.user_id]).then(response => {
            req.role_kor = db.roles[response.rows[0].user_role].role_kor.trim();    
            next();
            return;
        })
    } catch (err) {
        console.log(err);
        return res.status(500).send({
            error: 'server error',
        });
    }
}

isAdmin = (req, res, next) => {
    if(req.role_kor == '관리자') {
        next()
    } else {
        res.status(403).send({
            message: "Require admin"
        });
    }
}

isUser = (req, res, next) => {
    if(req.role_kor == '사용자') {
        next()
    } else {
        res.status(403).send({
            message: "Require user"
        });
    }
}

const authJwt = {
    verifyToken: verifyToken,
    findRoleById: findRoleById,
    isAdmin: isAdmin,
    isUser: isUser
};

module.exports = authJwt;