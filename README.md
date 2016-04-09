# SoLuaBind
a lua bind solution in c++
这套代码用来实现C++代码与lua代码的交互，互相调用函数，互相传递数据。
本套代码对应的lua版本为5.1。只需稍加修改，就可以支持lua其他版本。

class SoLua 能够做的：
1，创建并初始化一个lua_State对象；
2，加载并执行一个lua脚本文件；
3，执行一段lua代码；
4，压入参数，执行一个lua脚本函数，然后获取返回值；
5，压入参数，获取lua脚本中的指定的table的某些字段值；

class SoLuaBind 能够做的：
1，向lua环境中注册C++函数；
2，压入数组元素值，向lua环境中注册一个table类型的数组，下标从1开始；
3，压入键值对，向lua环境中注册一个table；

