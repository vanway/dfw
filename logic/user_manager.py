# coding=utf-8
import os
import sys

FILE_PATH = os.path.split(os.path.realpath(__file__))[0]
sys.path.append('{0}/../data_access'.format(FILE_PATH))

from mysql_manager import MysqlManager

def get_admin():
    return MysqlManager.get_admin()[1:]

def get_users():
    return dict([(item[1], item[1:])  for item in MysqlManager.get_users()])
