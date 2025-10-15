# 数据结构

#### 一

**7. 学生成绩管理**

【问题描述】  
对计算机学院计算机科学与技术专业的 2011 级本科生的学生成绩管理作一个简单的模拟。

【实验要求】  
设计学生成绩管理的模拟程序。  
（1）采用顺序表登录学生成绩。  
（2）可以登记、查询、插入、删除学生成绩。  
（3）将成绩按科目存储到链表中。

#### 二

#### 三

#### 四

## 部署 GUI，以及编译

基本上是 [gtk 文档](https://www.gtk.org/docs/installations/windows/)里面的步骤。

1. 安装 [msys2](https://www.msys2.org/)。（用来创建一个 linux 环境）

2. 打开 MSYS2 **UCRT64**。（有好几个环境，ucrt64 更常用些。）

（下面都是在这个环境里执行）

3. 安装 gtk3。

   ```
   pacman -S mingw-w64-ucrt-x86_64-gtk3
   ```

4. 安装 C 语言编译工具。

   ```
   pacman -S mingw-w64-ucrt-x86_64-toolchain base-devel
   ```

5. 切换到项目的目录。
   
   ```
   cd X:/path/to/codes
   ```
   
   `cd` = change directory。`X:` 意思是盘符，比如 C 盘。路径分隔符是 `/` 不是 `\`。（linux 和 win 不一样）
   
   比如
   
   ```
   cd C:/users/me/Desktop/data/data1
   ```
   
6. 编译。

   ```
   gcc -o run.exe *.c `pkg-config --cflags --libs gtk+-3.0` -mwindows
   ```
   
   `gcc` 是 C 语言编译工具。`-o run.exe` 代表输出的可执行文件。`*.c` 编译所有 .c 文件。`pkg-config --cflags --libs gtk+-3.0` 是把 gtk3 加进去。
