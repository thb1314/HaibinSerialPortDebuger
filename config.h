#ifndef CONFIG_H
#define CONFIG_H
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
//全局图标
//标题栏左上角图标
#define IconMain    QChar(0xf072)
//下拉菜单图标
#define IconMenu    QChar(0xf0d7)
//最小化图标
#define IconMin     QChar(0xf068)
//最大化图标
#define IconMax     QChar(0xf2d2)
//还原图标
#define IconNormal  QChar(0xf2d0)
//关闭图标
#define IconClose   QChar(0xf00d)


#ifdef __arm__
//全局字体名称
#define FontName "Droid Sans Fallback"
//全局字体大小
#define FontSize 13
#else
//全局字体名称
#define FontName "Microsoft Yahei"
//全局字体大小
#define FontSize 9
#endif
#define ConfigFile "config.ini"      //配置文件文件路径及名称

/**
  * 样式表颜色值
  */
//文字颜色
#define TextColor   "#F0F0F0"
//面板颜色
#define PanelColor  "#F0F0F0"
//边框颜色
#define BorderColor "#F0F0F0"
//正常状态开始颜色
#define NormalColorStart    "#F0F0F0"
//正常状态结束颜色
#define NormalColorEnd  "#F0F0F0"
//加深状态开始颜色
#define DarkColorStart  "#F0F0F0"
//加深状态结束颜色
#define DarkColorEnd    "#F0F0F0"
//高亮颜色
#define HighColor   "#F0F0F0"

#endif // CONFIG_H
