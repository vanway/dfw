#coding=utf-8
import os
import sys
import gevent.monkey
gevent.monkey.patch_all()

from collections import deque
from flask import Flask, render_template, session, redirect, url_for, escape, request, make_response
from flask.ext.uwsgi_websocket import GeventWebSocket

import thread
import random
import time
import ujson
from ctypes import *

FILE_PATH = os.path.split(os.path.realpath(__file__))[0]
sys.path.append('{0}/../data_access'.format(FILE_PATH))
sys.path.append('{0}/../logic'.format(FILE_PATH))

import user_manager
from mysql_manager import MysqlManager

app = Flask(__name__)
ws = GeventWebSocket(app)

#行情指数队列
price_deque = deque(maxlen=2*60*5)

#ws 链接的用户
users = {}

# 开启状态
g_is_open = False

#### 交易变量
# 当前价格
cur_price = 0.0
#单价
g_unit_price = 0.0
#行情地址
g_server_ip = ''
#帐号
g_big_account_name = ''
#密码
g_big_account_pw = ''
#期货代码
g_future_code = ''

#有权限的用户
pass_users = {}

#管理员帐号
admin = []

#pass_users 和 ws连接用户关联
user_match_dict = {}

#用户交易信息
user_trade_info = {}

# set the secret key.  keep this really secret:
app.secret_key = 'B0yrkdl/3yX R~XHH!jmN]7yh/,?RT'

def update_users_privilege():
    while True:
        global admin
        global pass_users
        admin = user_manager.get_admin()
        pass_users = user_manager.get_users()
        time.sleep(30)

def send_random_msg():
    while True:
        global cur_price
        cur_price = float('%0.2f'%(random.randint(3500, 4200) + random.random()))
        price_deque.append(cur_price)
        msg_dict = {"trade_info": [], "cur_price": cur_price}
        msg = ujson.dumps(msg_dict, ensure_ascii=False)
        for user_id in users:
            users[user_id].send(msg)
        for user_id in user_trade_info:
            item = user_trade_info[user_id]
            item[-2] = cur_price
            item[-1] = (cur_price - item[-3]) * 300
            user_trade_info[user_id] = item
            msg = ujson.dumps({'single_trade': item}, ensure_ascii=False)
            users[user_id].send(msg)
        time.sleep(0.5)

def get_future_info(_cur_price):
    global cur_price
    print "================================="
    print _cur_price
    cur_price = _cur_price
    msg_dict = {"trade_info": [], "cur_price": cur_price}
    msg = ujson.dumps(msg_dict, ensure_ascii=False)
    for user_id in users:
        users[user_id].send(msg)
    for user_id in user_trade_info:
        item = user_trade_info[user_id]
        item[-2] = cur_price
        item[-1] = (cur_price - item[-3]) * 300
        user_trade_info[user_id] = item
        msg = ujson.dumps({'single_trade': item}, ensure_ascii=False)
        users[user_id].send(msg)

def get_future_info_t():
    api = CDLL("../traderapi/testx.so")
    pGetFutureInfo = CFUNCTYPE(c_void_p,c_double)
    pGetFutureInfoHandle = pGetFutureInfo(get_future_info)
    api.StartSubjectFutureInfo(pGetFutureInfoHandle)
    while(1):
        time.sleep(2)

#thread.start_new_thread(get_future_info_t)
thread.start_new_thread(send_random_msg)
thread.start_new_thread(update_users_privilege)

@ws.route('/websocket')
def chat(ws):
    users[ws.id] = ws
    print ws.id

    while True:
        msg = ws.receive()
        if msg is not None:
            print msg
            typex, obj, content = "", "", ""
            try:
                typex, obj, content = msg.split(',')
            except:
                pass
            if typex == '1' and obj == 'user':
                global user_match_dict
                user_match_dict[ws.id] = content
                user = pass_users[content]
                total_trade = [user[0], user[2], user[3], user[3]-user[2]]
                msg = ujson.dumps({'total_trade': total_trade}, ensure_ascii=False)
                users[ws.id].send(msg)
            if g_is_open:
                if typex == "2":
                    now_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
                    user_trade_info[ws.id] = [now_time, pass_users[user_match_dict[ws.id]][3], cur_price, cur_price, 0]

                    trade_stream = [now_time, pass_users[user_match_dict[ws.id]][0], typex, "user", cur_price, "1"]
                    MysqlManager.insert_stream_trade(trade_stream)
                    trade_stream = [now_time, pass_users[user_match_dict[ws.id]][0], typex, "server", cur_price, "1"]
                    MysqlManager.insert_stream_trade(trade_stream)

                    msg = ujson.dumps({'single_trade': user_trade_info[ws.id]}, ensure_ascii=False)
                    users[ws.id].send(msg)
                if typex == "3":
                    now_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
                    user_trade_info[ws.id] = [now_time, pass_users[user_match_dict[ws.id]][3], cur_price, cur_price, 0]

                    trade_stream = [now_time, pass_users[user_match_dict[ws.id]][0], typex, "user", cur_price, "1"]
                    MysqlManager.insert_stream_trade(trade_stream)
                    trade_stream = [now_time, pass_users[user_match_dict[ws.id]][0], typex, "server", cur_price, "1"]
                    MysqlManager.insert_stream_trade(trade_stream)

                    msg = ujson.dumps({'single_trade': user_trade_info[ws.id]}, ensure_ascii=False)
                    users[ws.id].send(msg)
                if typex == "4":
                    MysqlManager.update_item([user_trade_info[ws.id][-1], user_match_dict[ws.id]])

                    now_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
                    trade_stream = [now_time, pass_users[user_match_dict[ws.id]][0], typex, "user", cur_price, "1"]
                    MysqlManager.insert_stream_trade(trade_stream)
                    trade_stream = [now_time, pass_users[user_match_dict[ws.id]][0], typex, "server", cur_price, "1"]
                    MysqlManager.insert_stream_trade(trade_stream)

                    user = MysqlManager.get_user_by_name([user_match_dict[ws.id]])[1: ]
                    total_trade = [user[0], user[2], user[3], user[3]-user[2]]
                    msg = ujson.dumps({'total_trade': total_trade}, ensure_ascii=False)
                    users[ws.id].send(msg)
                    del user_trade_info[ws.id]
        else:
            break

    del users[ws.id]
    if ws_id in user_trade_info:
        del user_trade_info[ws.id]

@app.route('/getUserList')
def get_user_list():
    result = []
    online_users = set([user_match_dict[ws_id] for ws_id in users])
    tmp_pass_users = user_manager.get_users()
    for name in tmp_pass_users:
        user = tmp_pass_users[name]
        status = "0"
        if name in online_users:
            status = "1"
        mid = [user[0], user[4], user[2], user[3], user[3]-user[2], user[5], user[7], status]
        result.append(mid)
    return ujson.dumps(result, ensure_ascii=False)

@app.route('/getUserStream')
def get_user_stream():
    result = MysqlManager.get_trade_stream(["user"])
    return ujson.dumps(result, ensure_ascii=False)

@app.route('/getServerStream')
def get_server_stream():
    result = MysqlManager.get_trade_stream(["server"])
    return ujson.dumps(result, ensure_ascii=False)

@app.route('/admin')
def admin():
    username = session.get('username')
    password = session.get('password')
    if username ==  admin[0] and password == admin[1]:
        return render_template('admin.html')
    return redirect('login')

@app.route('/')
def index():
    username = session.get('username')
    password = session.get('password')
    if username in pass_users and password == pass_users[username][1]:
        resp = make_response(render_template('index.html'))
        resp.set_cookie('username', username)
        return resp
    return redirect('login')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        session['username'] = request.form['username']
        session['password'] = request.form['password']
        admin = request.form.get('admin', "")
        if admin == "on":
            return redirect('admin')
        return redirect(url_for('index'))
    return render_template('login.html')

@app.route('/logout')
def logout():
    # remove the username from the session if it's there
    session.pop('username', None)
    session.pop('password', None)
    return redirect('login')

@app.route('/getInitInfo')
def get_init_info():
    return ujson.dumps(list(price_deque), ensure_ascii=False)

@app.route('/setSwitch', methods=['GET', 'POST'])
def set_switch():
    if request.method == 'POST':
        switch = request.form.get('switch', '0')
        global g_is_open
        if switch == '0':
            g_is_open = False
        elif switch == '1':
            g_is_open = True

@app.route('/getSwitch', methods=['GET', 'POST'])
def get_switch():
    return str(g_is_open)

@app.route('/setTradeParams', methods=['GET', 'POST'])
def set_trade_params():
    if request.method == 'POST':
        global g_unit_price
        global g_server_ip
        global g_big_account_name
        global g_big_account_pw
        global g_future_code
        g_unit_price = request.form['unit_price']
        g_server_ip = request.form['server_ip']
        g_big_account_name = request.form['big_account_name']
        g_big_account_pw = request.form['big_account_pw']
        g_future_code = request.form['future_code']

@app.route('/addUser', methods=['GET', 'POST'])
def add_user():
    if request.method == 'POST':
        key_list = ['AccountName', 'Password', 'UserName', 'RechargeMoney', \
                'RechargeMoney', 'Mobile', 'IDCard', 'Star']
        item = [request.form[key] for key in key_list]
        print MysqlManager.insert_user(item)
        return "ok"

@app.route('/addUserMoney', methods=['GET', 'POST'])
def add_user_money():
    if request.method == 'POST':
        user_name = request.form['UserName']
        add_money = request.form['Money']
        print MysqlManager.user_add_money(user_name, add_money)
        return 'ok'

@app.route('/modifyUserStar', methods=['GET', 'POST'])
def modify_user_star():
    if request.method == 'POST':
        user_name = request.form['UserName']
        star = request.form['Star']
        print MysqlManager.user_modify_star(user_name, star)
        return 'ok'

@app.route('/deleteUser', methods=['GET', 'POST'])
def delete_user():
    if request.method == 'POST':
        user_name = request.form['UserName']
        MysqlManager.user_delete(user_name)
        return 'ok'

if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True, gevent=100)
    #app.run(host="0.0.0.0", debug=True)
