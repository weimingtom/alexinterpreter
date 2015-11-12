# 2012.2.7 update #
update wiki at https://code.google.com/p/alexinterpreter/wiki/alexLanguage

alex snake demo at https://code.google.com/p/alexinterpreter/downloads/detail?name=run2.rar&can=2&q=#makechanges

# 2012.2.6 update #
## faster than perl ##
```
#sort 5K number.  cpu：inter pentium 双核E5400 @ 2.70ghz
PERL   4328MS
ALEX   3781MS
```
# 2011.10 update #
> 一种面向alex语法规则的脚本解释器， 此解释器执行过程是通过内部实现的基于stack设计的虚拟机。与多数基于虚拟机的解释器执行过程大致相同： 词法分析（lex）->语法分析（parse）->编译处理（com，编译成alex虚拟机支持的汇编指令）->虚拟机执行（vm）。由于在编译阶段中没法脱离虚拟机，所以无法直接生成laxc的二进制文件，这样的流程跟一般的编译器的流程不甚相同，所以对此程序命名时由原来的编译器改成了解释器，但从宏观的角度上来说，现在对于编译与解释的定义与经典的编译和解释已经越来越不相同，两者之间的设计多有交集。所以对于脚本语言来说，并没有严格意义上的编译和解释这样的区分。

### alex的特性： ###
> 语法类C，屏蔽了类型定义，以及内存管理；
> 数据类型：number string arraylist 解释器会对string arraylist 进行GC 处理，其中arraylist是集合定义，组员可以泛型存储。
### GC特性： ###
> GC的回收是通过最古老的计数回收算法，对string会有特殊处理，避免了重复字符串。由于是计数回收，所以会存在循环引用导致的内存无法释放问题，后续版本会将GC改为标记遍历机制。
### ANI特性： ###
> 加入了关键字using "XXX.DLL" ，其中"xxx.DLL"为所要引入的DLL路径。可以引入dll中注册给alex虚拟机的函数的接口。从而在脚本中你能够调用dll中的函数; 所引用的dll必须要实现跟alex解释器约定好的接口函数：alex\_dll\_reg（其中实现了dll中注册给alex解释器的函数），同时实现的导出函数必须是int func(void\*p)类型，参数通过ani\_pop\_xxx(p)来获得，返回值通过ani\_push\_xxx()，具体请参阅a\_window.dll的实现代码。（没有JNI强大，看看YY下就行了，呵呵）
### COM特性： ###
> 编译生成alex自定义的汇编二进制指令，其中指令集是基于stack而设计的，指令长度是单地址访问，由于对全局变量和常量string的定义与vm耦合性过高，没法在com编译阶段直接生成可执行的二进制文件。输出的alex汇编指令没有做过多的优化，所以存在一定的效率问题。
### VM 特性： ###
> 虚拟机基于stack设计，输入为alex汇编指令，内部设置5个堆栈：
**code stack：**代码段，vm解析的汇编指令流。
**local stack：**局部变量堆栈段。
**data stack：**vm执行指令时用到的运算堆栈。
**call stack：**函数调用堆栈。
**global stack：**全局变量堆栈，保存全局声明的变量以及函数。
由于com编译生成的指令过程没有过多的优化，大面积的pop push 堆栈；以及设计的访问local 和global地址存在效率问题，导致此虚拟机在执行过程中会出现一定量的冗余。
> VM性能分析：对5000个数据进行o(n^2)复杂度选择排序。测试现有的脚本与编译语言之间性能：
```
C		        16MS
LUA		        890MS
PERL		        4281MS
ALEX-inter		32609MS（原来alex上基于语法树递归分析的速度）
ALEX-VM                 3781MS（现在基于vm设计的解释器，性能方面提升了88.40%，是原来的8.6倍！
如果修改了GC 回收方式应该能再减少些， cpu：inter pentium 双核E5400 @ 2.70ghz）。
```
### alex语法说明: ###
> 参阅 alex-rule文档。
### alex源代码: ###
> 请check out svn。


alex以此纪念我的一位大学室友～