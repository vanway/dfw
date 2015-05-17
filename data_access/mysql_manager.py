#coding=utf-8
import sys
import os

import pymysql

FILE_PATH = os.path.split(os.path.realpath(__file__))[0]
sys.path.append('{0}/../config'.format(FILE_PATH))
import AppSetting

class MysqlManager(object):
    @staticmethod
    def get_conn():
        return pymysql.connect(**AppSetting.local_DB)

    @staticmethod
    def get_users():
        conn = MysqlManager.get_conn()
        cur = conn.cursor()
        sql = ''' select * from Users'''
        try:
            cur.execute(sql)
            conn.commit()
            result = [i for i in cur.fetchall()]
            return result
        except Exception as e:
            print e
            return None
        finally:
            if conn:
                conn.close()
            if cur:
                cur.close()

    @staticmethod
    def get_user_by_name(params):
        conn = MysqlManager.get_conn()
        cur = conn.cursor()
        sql = ''' select * from Users where AccountName= %s '''
        try:
            cur.execute(sql, params)
            conn.commit()
            result = [i for i in cur.fetchall()]
            return result[0]
        except Exception as e:
            print e
            return None
        finally:
            if conn:
                conn.close()
            if cur:
                cur.close()

    @staticmethod
    def get_admin():
        conn = MysqlManager.get_conn()
        cur = conn.cursor()
        sql = ''' select * from Admin'''
        cur.execute(sql)
        result = [i for i in cur.fetchall()]
        conn.close()
        cur.close()
        return result[0]

    @staticmethod
    def update_item(items):
        conn = MysqlManager.get_conn()
        cur = conn.cursor()
        sql = '''
              Update Users 
              set AccountMoney = AccountMoney + %s
              where AccountName = %s
              '''
        try:
            cur.execute(sql, items)
            conn.commit()
            return True
        except Exception as e:
            print e
            return False
        finally:
            if conn:
                conn.close()
            if cur:
                cur.close()

    @staticmethod
    def insert_stream_trade(items):
        conn = MysqlManager.get_conn()
        cur = conn.cursor()
        sql = '''
              insert Trade_Stream 
              (StatTime, AccountName, Operate, Belong, Point, Status) 
              values(%s, %s, %s, %s, %s, %s)
              '''
        try:
            cur.execute(sql, items)
            conn.commit()
            return True
        except Exception as e:
            print e
            return False
        finally:
            if conn:
                conn.close()
            if cur:
                cur.close()

    @staticmethod
    def get_trade_stream(params):
        conn = MysqlManager.get_conn()
        cur = conn.cursor()
        sql = ''' select convert(StatTime, char),AccountName,Operate,Point,Status from Trade_Stream where Belong = %s '''
        try:
            cur.execute(sql, params)
            conn.commit()
            result = [i for i in cur.fetchall()]
            return result
        except Exception as e:
            print e
            return None
        finally:
            if conn:
                conn.close()
            if cur:
                cur.close()

if __name__ == '__main__':
    print MysqlManager.get_admin()
    print MysqlManager.get_users()
    #items = [('2015-02-28', '1200002', 100)]
    #MysqlManager.insert_items_into_return_cash(items)
    #print MysqlManager.get_return_cash()
