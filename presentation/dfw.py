#coding=utf-8
import os
import sys
from collections import deque
from flask import Flask, render_template, session, redirect, url_for, escape, request, make_response
from flask.ext.uwsgi_websocket import GeventWebSocket

import thread
import random
import time
import ujson

FILE_PATH = os.path.split(os.path.realpath(__file__))[0]
sys.path.append('{0}/../data_access'.format(FILE_PATH))
sys.path.append('{0}/../logic'.format(FILE_PATH))

import user_manager
from mysql_manager import MysqlManager

app = Flask(__name__)
ws = GeventWebSocket(app)

#ws 链接的用户
users = {}

#有权限的用户
pass_users = {}

#管理员帐号
admin = []

#pass_users 和 ws连接用户关联
user_match_dict = {}

#用户交易信息
user_trade_info = {}

# 当前价格
cur_price = 0.0

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
        cur_price = random.randint(3500, 4200) + random.random()
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

thread.start_new_thread(send_random_msg)
thread.start_new_thread(update_users_privilege)

@ws.route('/websocket')
def chat(ws):
    users[ws.id] = ws
    print ws.id

    while True:
        print '++++++++++++++++',  ws.id
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
            if typex == "2":
                now_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
                user_trade_info[ws.id] = [now_time, pass_users[user_match_dict[ws.id]][3], cur_price, cur_price, 0]
                trade_stream = [now_time, pass_users[user_match_dict[ws.id]][1], typex, "user", cur_price]
                MysqlManager.insert_stream_trade(trade_stream)
                msg = ujson.dumps({'single_trade': user_trade_info[ws.id]}, ensure_ascii=False)
                users[ws.id].send(msg)
            if typex == "3":
                now_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())
                user_trade_info[ws.id] = [now_time, pass_users[user_match_dict[ws.id]][3], cur_price, cur_price, 0]
                trade_stream = [now_time, pass_users[user_match_dict[ws.id]][1], typex, "user", cur_price]
                MysqlManager.insert_stream_trade(trade_stream)
                msg = ujson.dumps({'single_trade': user_trade_info[ws.id]}, ensure_ascii=False)
                users[ws.id].send(msg)
            if typex == "4":
                MysqlManager.update_item([user_trade_info[ws.id][-1], user_match_dict[ws.id]])
                user = MysqlManager.get_user_by_name([user_match_dict[ws.id]])[1: ]
                total_trade = [user[0], user[2], user[3], user[3]-user[2]]
                msg = ujson.dumps({'total_trade': total_trade}, ensure_ascii=False)
                users[ws.id].send(msg)
                del user_trade_info[ws.id]
        else:
            break

    del users[ws.id]
    del user_trade_info[ws.id]
    print '======================='

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

if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True, gevent=100)
