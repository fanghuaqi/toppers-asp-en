TOPPERS/ASP内核代码英文化及文档中文化

# 项目介绍 #
本项目是作为2011 TOPPERS公募事业在TOPPERS Project的支持下进行的，关于TOPPERS的更多信息请访问TOPPERS官方网站http://www.toppers.jp

TOPPERS Project的所有成果的文档、代码注释等资料主要以日文形式存在，由于语言上的障碍，严重制约了TOPPERS Project在日本以外的推广。TOPPERS/ASP内核是整个TOPPERS Project的基础。了解TOPPERS/ASP内核是了解TOPPERS Project的必经之路。为了让更多的人分享TOPPERS的成果， 我们计划以TOPPERS/ASP Release 1.7针对skyeye的发布包为基础，完成其代码注释英文化以及相关文档的中文化，具体工作如下：
  1. 完成除cfg外其他目录下内核相关.c和.h中代码注释的英文化，关于dev68k的相关代码不在此之列(因为不熟悉dev68k的结构)，以在ARM上的移植取而代之。关于代码注释的风格，我们提出以下两种以供参考：
    * doxygen风格的注释，需在现有基础上增加大量的注释和说明。
    * 把现有日文注释，一一对应全部翻译成英文注释
  1. 完成doc下文档的中文化工作。
  1. 上述工作的基础上，进行测试。
  1. 为TOPPERS/ASP内核增加在S3C2410(ARM920T)，LPC2478(ARM720TDMI-S)的移植。
  1. 关于TOPPERS/ASP内核的一些资料，如报告，ppt，pdf等等我们也会放在项目网站上。


本项目由华中科技大学-瑞萨高级嵌入式控制器实验室主要承担，结合TOPPERS 中国组和TOPPERS 英文组的力量共同完成。由于进行相关翻译工作的人员水平有限，难免会出一些差错，所以如果大家发现什么问题，请指出，这样我们可以及时进行修正。如果有人对本项目很感兴趣，欢迎加入！具体情况可联系：renweihust@gmail.com,或给TOPPERS中国组发邮件chn-wg@toppers.jp!

TOPPERS公益事业邮件列表地址为 http://groups.google.com/group/toppers-asp-en ,欢迎大家加入讨论,大家可以直接发送电子邮件至该论坛： toppers-asp-en@googlegroups.com ,就可以参加讨论.

## 项目进展 ##
  * kernel部分英文化的代码已经提交
  * ASP用户使用说明的中文版本已经提交([user.txt](http://code.google.com/p/toppers-asp-en/source/browse/trunk/asp_en/doc_chn/user.txt))
  * 增加如下文档的中文翻译([asp中文手册](http://code.google.com/p/toppers-asp-en/source/browse/#svn%2Ftrunk%2Fasp_en%2Fdoc_chn))

| **文档** | **说明** |
|:-----------|:-----------|
| configurator\_cn | 配置器使用说明 |
| porting\_cn | asp内核移植文档 |
| version\_cn | 版本说明 |
| asp\_spec\_cn | asp内核规范概要 |
| user\_cn | asp内核使用手册 |
| design\_cn | asp内核设计备忘 |
| extension\_cn | asp内核扩展与调整说明 |