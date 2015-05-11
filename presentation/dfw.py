#coding=utf-8
import os
import sys
from collections import deque
from flask import Flask, render_template, session, redirect, url_for, escape, request, make_response
from flask.ext.uwsgi_websocket import GeventWebSocket

import thread
import random
import time
import os
import ujson

FILE_PATH = os.path.split(os.path.realpath(__file__))[0]
sys.path.append('{0}/../data_access'.format(FILE_PATH))

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

def send_random_msg():
    while True:
        global pass_users
        global admin
        global cur_price
        pass_users = dict([(item[1], item[1:])  for item in MysqlManager.get_users()])
        admin = MysqlManager.get_admin()[1:]
        cur_price = random.randint(3500, 4200) + random.random()
        sell_5 = []
        buy_5 = []
        for i in range(5):
            sell_5.append([u"卖{}".format(str(i+1)), cur_price + i, random.randint(1, 1000)])
            buy_5.append([u"买{}".format(str(i+1)), cur_price - i, random.randint(1, 1000)])
        sell_5.reverse()
        sell_5.extend(buy_5)
        msg_dict = {"trade_info": sell_5, "cur_price": cur_price}
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
        time.sleep(1)

thread.start_new_thread(send_random_msg)

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
                msg = ujson.dumps({'single_trade': user_trade_info[ws.id]}, ensure_ascii=False)
                users[ws.id].send(msg)
            if typex == "3":
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
