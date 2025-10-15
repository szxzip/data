## 部署GUI环境

基本上是 [gtk 文档](https://www.gtk.org/docs/installations/windows/)里面的步骤。

1. 安装 [msys2](https://www.msys2.org/)。（用来创建一个 linux 环境）

2. 打开 MSYS2 **UCRT64**。（有好几个环境，ucrt64 更常用些。下面的命令都是在这个环境里执行）

3. [gtk 文档](https://www.gtk.org/docs/installations/windows/)第二步，安装 **gtk3**，不是 gtk4。（gtk4 好像有一些奇怪的问题，特别是在 win 上）。

4. [gtk 文档](https://www.gtk.org/docs/installations/windows/)第三步，安装 C 语言编译工具。

5. 文档下面的不用装了，基本上就是主题包。

6. 切换到项目的目录。命令：
   
   cd X:/path/to/codes
   
   cd = change directory。X 意思是盘符，比如 C 盘。要注意是 **/** 不是 **\**，/ 是 linux 路径分隔符，\ 是 win 的，不一样。
   
   比如 cd C:/users/me/Desktop/data/data1
   
7. 编译命令：

   gcc -o run.exe *.c `pkg-config --cflags --libs gtk+-3.0` -mwindows
   
   gcc 是 C 语言编译工具。-o run.exe 代表输出的可执行文件。*.c 编译所有 .c 文件。`` 里面的是把 gtk 加进去。
