var g_pre_menu = ""; //前一个active菜单名称


$(document).ready(function() {
    getAccountInfo("account_info_menu");
});

function menuChange(menu_name){
    if(g_pre_menu.length > 0){
        $("#"+g_pre_menu).removeClass("active");
    }
    $("#"+menu_name).addClass("active");
    g_pre_menu = menu_name;
}

function heredoc(fn) {
    return fn.toString().split('\n').slice(1,-1).join('\n') + '\n'
}

function set_cheat_user_click(){
} 


//获取用户列表
function getUserList(menu_name) {
    menuChange(menu_name);
    $.ajax({
        url: "getUserList",
        type: 'GET', 
        data: {},
        dataType: 'html', 
        timeout: 10000,
        error: function(){alert('Error loading document');},
        success: function(result){ 
            var data = eval("("+result+")");
            $('#right_content').empty();
            var frame = heredoc(function(){/*
            <div class="shadow border content-u" id='div_user_list'>
                <div class="content-user-list">
                    <table class="table table-hover" id="user_data_table">
                    </table>
                </div>
            </div> <!-- 用户列表 -->
        */});
        $('#right_content').append(frame);
        var operate = heredoc(function(){/* 
        <div class="btn-group">
            <button type="button" class="btn-sm btn-primary dropdown-toggle" data-toggle="dropdown" aria-expanded="false">
            <span class="glyphicon glyphicon-cog" aria-hidden="true"></span><span class="caret"></span>
            </button>
            <ul class="dropdown-menu" role="menu">
                <li><a href="#" onclick="addMoney('account_name')">充值</a></li>
                <li><a href="#" onclick="modifyVip('account_name')">修改权限</a></li>
                <li><a href="#" onclick="deleteUser('account_name')">删除</a></li>
           </ul>
        </div>
        */});
        $('#user_data').empty();
        for(var i in data){
            data[i][2] = data[i][2].toFixed(2);
            data[i][3] = data[i][3].toFixed(2);
            data[i][4] = data[i][4].toFixed(2);
            var stat = data[i][6];
            if( "5" == stat){
               data[i][6] = '<span class="label label-success">正单</span>';
            }
            else if( "4" == stat){
               data[i][6] = '<span class="label label-info">反单</span>';
            }
            else{
               data[i][6] = '<span class="label label-warning">模拟</span>';
            }
            var is_online = data[i][7];
            if("1" == is_online){
                data[i][7] = '<span class="label label-success">在线</span>';
            }
            else{
                data[i][7] = '<span class="label label-danger">离线</span>';
            }
            var item_operate = operate.replace("account_name", data[i][0]);
            data[i].push(item_operate);
        }
        var columns = [
            {"title": "用户id",},
            {"title": "姓名",},
            {"title": "累计充值",},
            {"title": "余额",},
            {"title": "盈亏",},
            {"title": "电话号码",},
            {"title": "权限",},
            {"title": "状态",},
            {"title": "操作",}
        ];
        setDataTables("user_data_table", data, 9, columns);
        /*
        for(var i in data) {
            var tr = $('<tr>');
            tr.append($('<td>' + data[i][0] + '</td>')); //用户id
            tr.append($('<td>' + data[i][1] + '</td>')); //用户姓名
            tr.append($('<td>' + data[i][2].toFixed(2) + '</td>')); //累计充值
            tr.append($('<td>' + data[i][3].toFixed(2) + '</td>')); //当前余额
            tr.append($('<td>' + data[i][4].toFixed(2) + '</td>')); //总盈亏
            tr.append($('<td>' + data[i][5] + '</td>')); //电话号码
            if (data[i][7] == '1') {//是否在线
                tr.append($('<td>是</td>'));
            } else {
                tr.append($('<td>否</td>'));
            }
            $('#user_data').append(tr);
        }
        */
        }
    });
}

//
function globalSet(menu_name){
    menuChange(menu_name);
    $('#right_content').empty();
    var frame = heredoc(function(){/*
        <div class="shadow border content-u" id='div_control'>
            <div class="input-group">
                <span class="input-group-addon" id="basic-addon1">账号</span>
                <input type="text" class="form-control" placeholder="Username" aria-describedby="basic-addon1">
            </div>

            <div class="input-group">
                <span class="input-group-addon" id="basic-addon2">密码</span>
                <input type="password" class="form-control" placeholder="password" aria-describedby="basic-addon2">
                
            </div>

            <div class="input-group">
                <span class="input-group-addon">交易变量</span>
                <input type="text" class="form-control" aria-describedby="basic-addon3">
            </div>

            <div class="switch" data-on="success" data-off="warning">
                <input type="checkbox" checked />
            </div>
        </div> <!-- 全局管理 -->
    */});
    $('#right_content').append(frame);
}

//获取交易流水数据
function getTradeData(menu_name) {
    menuChange(menu_name);
    $('#right_content').empty();
    var frame = heredoc(function(){/*
        <div class="shadow border content-u" id='div_trade_stream' >
            <div class="row padding-x"> 
              <ul class="nav nav-tabs nav-justified" role="tablist">
                <li id="userStreamTab" class="active"><a href="#">用户流水</a></li>
                <li id="tradeTab"><a href="#">交易端回报</a></li>
              </ul>
            </div>
                <div class="content-stream" id="div_user_steam">
                    <table class="table table-hover"  id="user-stream">
                    </table>
                </div>
                <div class="content-stream" id="div_trade_payback" hidden>
                    <table class="table table-hover"  id="trade_payback">
                    </table>
                </div>
            </div> 
        </div>
    */});
    $('#right_content').append(frame);
    //交易流水tab
    $('#userStreamTab').click(function(){
        getUserStream();
        $('.nav li').removeClass('active');
        $('#userStreamTab').addClass('active');
        $('#div_trade_payback').attr('hidden',true);
        $('#div_user_steam').attr('hidden', false);
        $('#userStreamTab').removeAttr('hidden');
        });
    $('#tradeTab').click(function(){
        getServerStream();
        $('.nav li').removeClass('active');
        $('#tradeTab').addClass('active');
        $('#div_user_steam').attr('hidden', true);
        $('#div_trade_payback').attr('hidden',false);
        $('#tradeTab').removeAttr('hidden');
        });
    getUserStream();
}

function getUserStream(){
    $.ajax({
        url: "getUserStream",
        type: 'GET', 
        data: {},
        dataType: 'html', 
        timeout: 10000,
        error: function(){alert('Error loading document');},
        success: function(result){ 
            var user_stream = eval("("+result+")");
            var columns = [
                {"title": "时间",},
                {"title": "用户id",},
                {"title": "操作",},
                {"title": "点位",},
            ];
            setDataTables("user-stream", user_stream, 8, columns);
            }
        });
}
function getServerStream(){
    $.ajax({
        url: "getServerStream",
        type: 'GET', 
        data: {},
        dataType: 'html', 
        timeout: 10000,
        error: function(){alert('Error loading document');},
        success: function(result){ 
            var trade_payback = eval("("+result+")");
            var columns = [
                {"title": "时间",},
                {"title": "用户id",},
                {"title": "操作",},
                {"title": "点位",},
                {"title": "状态",},
            ];
            setDataTables("trade_payback", trade_payback, 8, columns);
        }
    });
}

function getAccountInfo(menu_name){
    menuChange(menu_name);
    $('#right_content').empty();
    var frame = heredoc(function(){/*
            <div class="shadow border content-u" id='account_info'>
                <div class="row" >
                    <div class="col-md-3 content-account-fig">
                        <div class="content-account-inner">
                            <span class="glyphicon glyphicon-home" aria-hidden="true"></span> 
                            <span class="glyphicon-class">账户id: 10001</span>
                        </div>
                    </div>
                    <div class="col-md-3 content-account-fig">
                        <div class="content-account-inner">
                            <span class="glyphicon glyphicon-list-alt" aria-hidden="true"></span>
                            <span class="glyphicon-class">账户持仓: 1000000</span>
                        </div>
                    </div>
                    <div class="col-md-3 content-account-fig">
                        <div class="content-account-inner">
                            <span class="glyphicon glyphicon-yen" aria-hidden="true"></span>
                            <span class="glyphicon-class">账户资产: 2050000</span>
                        </div>
                    </div>
                    <div class="col-md-3 content-account-fig">
                        <div class="content-account-inner">
                            <span class="glyphicon glyphicon-log-in" aria-hidden="true"></span>
                            <span class="glyphicon-class">账户盈亏: 350000</span>
                        </div>
                    </div>
                </div>
                <div class="row">
                    <div class="content-account-detail">
                        <table class="table table-hover">
                            <thead>
                                <tr>
                                    <th>账户id</th>
                                    <th>购买数量</th>
                                    <th>操作方向</th>
                                    <th>持仓成本</th>
                                    <th>总盈亏</th>
                                </tr>
                            </thead>
                            <tbody id='account_data'>
                            </tbody>
                        </table>
                    </div>
                </div><!--account_info -->
    */});
    $('#right_content').append(frame);
}

function setDataTables(id_name, data, line_num, columns){
    var table = $('#'+id_name).dataTable({
        "autoWidth": false,
        "paging": true,
        "dom": 'TCfrtp',
        "iDisplayLength": line_num,
        "lengthMenu": [[50, 100, 200, -1], [50, 100, 200, "所有"]],
        "stateSave": false,
        "bDestroy": true,
        "data": data,
        "processing": true,
        "order": [
            [0, "asc"]
        ],
        "columns": columns,
        "sPaginationType": "full_numbers",
        "oLanguage": {
            "sProcessing": "<img src='/images/datatable_loading.gif'>  努力加载数据中.",
            "sLengthMenu": "每页显示 _MENU_ 条记录",
            "sZeroRecords": "抱歉， 没有找到",
            "sInfo": "从 _START_ 到 _END_ /共 _TOTAL_ 条数据",
            "sInfoEmpty": "没有数据",
            "sInfoFiltered": "(从 _MAX_ 条数据中检索)",
            "sZeroRecords": "没有检索到数据",
            "sSearch": "",
            "oPaginate": {
                "sFirst": "首页",
                "sPrevious": "前一页",
                "sNext": "后一页",
                "sLast": "尾页"
            }
        },
    });
    $('input[type="search"]').attr("class", "form-control");
    $('input[type="search"]').attr("placeholder", "查询...");
}

function addUser(menu_name){
    menuChange(menu_name);
    $('#right_content').empty();
    var frame = heredoc(function(){/*
    <div class="shadow border content-u" id='add_user'>
    <form class="form-horizontal add-user-form" action='/addUser' method="POST" enctype='multipart/form-data'>
    <div class="form-group">
        <label for="AccountName" class="col-sm-2 control-label">帐号ID</label>
        <div class="col-sm-10">
            <input type="text" class="form-control" name="AccountName" placeholder="AccountName">
        </div>
    </div>
    <div class="form-group">
        <label for="Password" class="col-sm-2 control-label">密码</label>
        <div class="col-sm-10">
            <input type="password" class="form-control" name="Password" placeholder="Password">
        </div>
    </div>
    <div class="form-group">
        <label for="UserName" class="col-sm-2 control-label">用户姓名</label>
        <div class="col-sm-10">
            <input type="text" class="form-control" name="UserName" placeholder="UserName">
        </div>
    </div>
    <div class="form-group">
        <label for="RechargeMoney" class="col-sm-2 control-label">充值金额</label>
        <div class="col-sm-10">
            <input type="text" class="form-control" name="RechargeMoney" placeholder="RechargeMoney">
        </div>
    </div>
    <div class="form-group">
        <label for="Mobile" class="col-sm-2 control-label">电话号码</label>
        <div class="col-sm-10">
            <input type="text" class="form-control" name="Mobile" placeholder="Mobile Number">
        </div>
    </div>
    <div class="form-group">
        <label for="IDCard" class="col-sm-2 control-label">身份证号码</label>
        <div class="col-sm-10">
            <input type="text" class="form-control" name="IDCard" placeholder="IDCard">
        </div>
    </div>
    <div class="form-group">
    <label for="Star" class="col-sm-2 control-label">用户权限</label>
        <div class="col-sm-10">
            <label class="radio-inline">
            <input type="radio" name="Star" value="1"> 1
            </label>
            <label class="radio-inline">
            <input type="radio"  name="Star" value="2"> 2
            </label>
            <label class="radio-inline">
            <input type="radio"  name="Star" value="3"> 3
            </label>
            <label class="radio-inline">
            <input type="radio"  name="Star" value="4"> 4
            </label>
            <label class="radio-inline">
            <input type="radio" name="Star" value="5"> 5
            </label>
        </div>
    </div>
    <div class="form-group">
        <div class="col-sm-offset-2 col-sm-10">
            <button type="submit" class="btn btn-default">添加</button>
        </div>
    </div>
    </form>
    </div>
    */});
    $('#right_content').append(frame);
    $('#Star').raty({ score: 3 });
}
