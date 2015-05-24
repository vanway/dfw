# -*- coding: utf-8 -*-
import os

FILE_PATH = os.path.split(os.path.realpath(__file__))[0]

#web uswgi port
uwsgi_port = '9001'

#mysql数据库配置
local_DB = {'host': '127.0.0.1', 'port': 3306, 'user': 'root', 'passwd': '1qaz@WSX', \
        'db': 'Monopoly', 'charset': 'utf8'}
