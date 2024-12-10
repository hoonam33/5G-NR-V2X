const bcrypt = require("bcryptjs");
const { verifyToken, findRoleById } = require('../middlewares/authJwt.middleware')
const dbAdapter = require('../db/dbAdapter');

module.exports = function (app) {
    const db = dbAdapter.GetDB();

    app.post(
        "/admin/register",
        [verifyToken, findRoleById, isAdmin],
        async (req, res, next) => {
            try {
                let findUserById = await checkExistUserId(db, req.body.user_id);
                if (findUserById) {
                    return res.status(400).send({
                        message: "Failed! User id is already in use!"
                    });
                }

                console.log(req.body)
                await db.query('insert into users(user_role, user_id, user_pw) values ($1, $2, $3)',
                    [req.body.user_role, req.body.user_id, bcrypt.hashSync(req.body.user_pw, 8)]);

                findUserById = await checkExistUserId(db, req.body.user_id);
                if (findUserById) {
                    return res.status(200).send({
                        message: "User registration Success!"
                    });
                } else {
                    return res.status(400).send({
                        message: "error: User registration Fail!"
                    });
                }

            } catch (err) {
                console.log(err);
                res.status(500).send({ message: 'server error' });
            }
        });

        app.post('/admin/get_users', [verifyToken, findRoleById, isAdmin], async(req, res, next) => {
            try {
                let userList = await db.query(`select * from users`);
                res.status(200).send({
                    userList: userList.rows
                }); 
            } catch (err) {
                console.log(err);
                res.status(500).send({ message: 'server error' });
            }
        });

        app.post('/admin/get_user_log', [verifyToken, findRoleById, isAdmin], async(req, res, next) => {
            try {
                if(req.body.user_id) {
                    db.query(`SELECT user_id, user_ip, user_status, created_time
                        FROM public.users_log where user_id=$1 order by log_id desc limit 100`,
                    [req.body.user_id]).then(
                        response => {
                            res.status(200).send({
                                userLogList: response.rows
                            }); 
                        },error => {
                            console.log(error)
                            res.status(400).send({
                                message: 'error: db'
                            }); 
                        }
                    )
                }
                if(!req.body.user_id) {
                    db.query(`SELECT user_id, user_ip, user_status, created_time
                        FROM public.users_log order by log_id desc limit 100`).then(
                        response => {
                            res.status(200).send({
                                userLogList: response.rows
                            }); 
                        },error => {
                            console.log(error)
                            res.status(400).send({
                                message: 'error: db'
                            }); 
                        }
                    )
                }
            } catch (err) {
                console.log(err);
                res.status(500).send({ message: 'error: server' });
            }
        });
}

const checkExistUserId = async (db, user_id) => {
    // Username
    let findUserById = (await db.query('SELECT user_id, user_role FROM users WHERE user_id = $1 LIMIT 1', [user_id])).rows[0];
    if (findUserById) {
        return true;
    } else {
        return false;
    }
};