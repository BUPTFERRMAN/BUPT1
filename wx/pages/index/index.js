//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
  //这里选择图片路径
imageSrc:'/pages/image/yuanfnag.jpeg'  
},
//跳转页面到彩蛋页面
gotocd:function(){
  wx.navigateTo({
    url:'/pages/cd/cd',
  })
}
})
