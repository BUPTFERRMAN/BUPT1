// start.js

Page({
    data: {
        imageSrc:'/pages/image/10013.jpg'  
    },
    //跳转到天气页面
    navigate: function() {
        wx.navigateTo({
            url: '/pages/wifi_station/tianqi/tianqi',
        })
    }
})