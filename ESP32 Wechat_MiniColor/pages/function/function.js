import mqtt from '../../utils/mqtt.js';


var app = getApp() // 取得全局App
const aliyunOpt = require('../../utils/aliyun/aliyun_connect.js')

let that = null;
Page({
  data: {
    canIUseOpenData: wx.canIUse('open-data.type.userAvatarUrl') && wx.canIUse('open-data.type.userNickName'), // 如需尝试获取用户信息可改为false
    client: null,
    isClicked:false,
    //记录重连的次数
    reconnectCounts: 0,
    //MQTT连接的配置
    options: {
      protocolVersion: 4, //MQTT连接协议版本
      clean: false,
      reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
      connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
      resubscribe: true, //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
      clientId: '',
      password: '',
      username: '',
    },
    aliyunInfo: {
      productKey: 'gdn4HYkButt', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      deviceName: 'WechatDemo', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      deviceSecret: '', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      regionId: 'cn-shanghai', //阿里云连接的三元组 ，请自己替代为自己的产品信息!!
      pubTopic: '/gdn4HYkButt/WechatDemo/user/update', //发布消息的主题
      subTopic: '/gdn4HYkButt/WechatDemo/user/get', //订阅消息的主题
    },
    bafayunInfo: {
      uid:"",//用户密钥，巴法云控制台获取
      myTopic:"WechatPic",//图片上传的主题，图片云控制台创建
      num:1,      //获取的图片数量，可随意
      imgList:[], //存储图片地址和时间，用于前端展示
      picArr:[], //存储图片的地址，用于图片点击预览
    }
  },
  onLoad: function() {
    that = this;
    //设置rpxRatio
    wx.getSystemInfo({
      success(res) {
        that.setData({
          rpxRatio: res.screenWidth / 750
        })
      }
    })
    //连接服务器
    //传进去三元组等信息，拿到mqtt连接的各个参数
    let clientOpt = aliyunOpt.getAliyunIotMqttClient({
      productKey: that.data.aliyunInfo.productKey,
      deviceName: that.data.aliyunInfo.deviceName,
      deviceSecret: that.data.aliyunInfo.deviceSecret,
      regionId: that.data.aliyunInfo.regionId,
      port: that.data.aliyunInfo.port,
    });
    //console.log("get data:" + JSON.stringify(clientOpt));
    //得到连接域名
    let host = 'wxs://' + clientOpt.host;
    this.setData({
      'options.clientId': clientOpt.clientId,
      'options.password': clientOpt.password,
      'options.username': clientOpt.username,
    })
    console.log("this.data.options host:" + host);
    console.log("this.data.options data:" + JSON.stringify(this.data.options));
    //开始连接
    wx.showToast({
      title: '开始连接'
    })
    this.data.client = mqtt.connect(host, this.data.options);
    this.data.client.on('connect', function(connack) {
      wx.showToast({
        title: '连接成功'
      })
    })
    //服务器下发消息的回调
    that.data.client.on("message", function(topic, payload) {
      console.log(" 收到 topic:" + topic + " , payload :" + payload)
      wx.showModal({
        content: " 收到topic:[" + topic + "], payload :[" + payload + "]",
        showCancel: false,
      });
    })
    //服务器连接异常的回调
    that.data.client.on("error", function(error) {
      console.log(" 服务器error 的回调" + error)
      wx.showToast({
        title: '服务器error'
      })
    })
    //服务器重连连接异常的回调
    that.data.client.on("reconnect", function() {
      console.log(" 服务器 reconnect的回调")
      wx.showToast({
        title: '服务器reconnect'
      })
    })
    //服务器连接异常的回调
    that.data.client.on("offline", function(errr) {
      console.log(" 服务器offline的回调")
      wx.showToast({
        title: '服务器offline'
      })
    })
  }, 
  onClickOpen() {
    that.sendCommond('set', [1]);
  },
  onClickOff() {
    that.sendCommond('set', [0]);
  },
  onClickShot() {
    that.setData({
      isClicked:true
    })
    that.sendCommond('set', [2]);
    that.getPicture()//获取图片
  },
  sendCommond(cmd, data) {
    let sendData = {
      cmd: cmd,
      data: data,
    };
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish(this.data.aliyunInfo.pubTopic, JSON.stringify(sendData));
      console.log("cmd:",sendData.cmd);
      console.log("cmd:",sendData.data);
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },
  
  getPicture(){    //获取图片函数
    //api 接口详细说明见巴法云接入文档
    wx.request({
      url: 'https://images.bemfa.com/cloud/v1/get/', //获取图片接口，详见巴法云接入文档
      data: {
        uid: that.data.bafayunInfo.uid,       //uid字段
        topic: that.data.bafayunInfo.myTopic,//topic字段
        num:that.data.bafayunInfo.num        //num字段
      },
      header: {
        'content-type': "image/jpeg"
      },
      success (res) {
         console.log(res)     //打印获取结果
         var imgArr = []      //定义空数组，用于临时存储图片地址和时间
         var arr = []         //定义空数组，用于临时存储图片地址
         for(var i = 0;i<res.data.data.length;i++){//遍历获取的结果数组
           var url = res.data.data[i].url          //提取图片地址
           var time = that.time(url.substring(url.lastIndexOf("-")+1,url.lastIndexOf(".")))//提取图时间
           imgArr.push({"url":url,"time":time})    //将存储图片地址和时间存入临时数组
           arr.push(url)                           //将存储图片地址存入临时数组
         }
         
         that.setData({     //把临时数组值复制给变量用于展示
           imgList:imgArr, //将临时存储图片地址和图片时间的数组赋值给用于图片预览的数组
           picArr:arr,    //将临时存储图片地址的数组赋值给用于图片预览的数组
         })
        console.log(that.data.imgList)   //打印赋值结果
      },
      fail(res){
        console.log(res.data)   //打印赋值结果
      }
    })
  },
  clickImg(e){                      //点击预览函数
    console.log(e.currentTarget.dataset.index)    //打印数组索引值
    var nowIndex = e.currentTarget.dataset.index  //获取索引值
    wx.previewImage({           //图片预览接口
      current: this.data.picArr[nowIndex],//当前图片地址
      urls: this.data.picArr    //图片地址数组
    })
  },
  time(time) {                    //时间戳转时间函数
    var date = new Date(parseInt(time)*1000 + 8 * 3600 * 1000);
    return date.toJSON().substr(0, 19).replace('T', ' ');
  },
})