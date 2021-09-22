var app = getApp()
Page( {
  data: {
    toastHidden: true,
    modalHidden:true,
    errinfo:null,
    PASSWORD:""
  },
  formSubmit: function( e ) {
    var that=this;
    console.log('form发生了submit事件，携带数据为：', e.detail.value);
    //  检查密码
    if(e.detail.value.password== ''){
      that.setData({
        modalHidden:false,
        errinfo:"登陆North217寝室失败，没有填写密码！"
      })
      return;
    }
    else if(e.detail.value.password== that.data.PASSWORD){
      that.setData({
        toastHidden:false
      }),
      wx.redirectTo({
        url:'../function/function'
      })
    }
    else{
      that.setData({
        modalHidden:false,
        errinfo:"登陆North217寝室失败，密码填写错误！"
      })
      return;
    }
  },
  onLoad: function() {
    console.log( 'onLoad' )
  },
  resetModal:function(){
    var that=this;
    that.setData({
        modalHidden:true,
        errinfo:null
      })
  }

})
