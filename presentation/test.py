from ctypes import *
import time
def get_future_info(_cur_price):

    #global cur_price
    print _cur_price
    '''
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
    '''

def get_future_info_t():
    api = CDLL("../traderapi/testx.so")
    pGetFutureInfo = CFUNCTYPE(c_void_p,c_double)
    pGetFutureInfoHandle = pGetFutureInfo(get_future_info)
    api.StartSubjectFutureInfo(pGetFutureInfoHandle)

get_future_info_t()
while(1):
    time.sleep(2)
