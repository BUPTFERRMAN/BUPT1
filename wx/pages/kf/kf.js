Page({

  /**
   * 页面的初始数据
   */
  data: {
    imageSrc:'/pages/image/dq.jpeg'  
  },
  //跳转页面到图灵界面
  goto:function(){
    wx.navigateTo({
      url:'/pages/tl/tl',
    })
  }
})