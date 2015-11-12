# alex 语言编程手册

# 前言： #
> 此文本仅仅是作为对alex 脚本语法所做的一个介绍，目的是为一些对alex脚本感兴趣的人所写的一个引导型的文档。
alex 语言是一款基于stack VM的解释型语言。执行的过程与大多数的解释型语言类似，都是lex（词法解析）->yacc(语法分析)->com(编译虚拟指令集)->VM(提交虚拟机进行执行)。语言类型为弱类型：number，string，arraylist， function。同时对变量添加了GC处理，GC采用的是比较古老的引用计数，对于string类型加了特殊处理。添加了string table，避免了同样的一个string被多次申请内存。 为了提升alex的扩展能力，添加了alex能够使用 外部的dll，在外部的dll中能够注册给alex vm 其中的函数接口，使脚本能够调用，加载相应的dll， 只需要使用关键字：using ["dllName"];
## 性能 ##
对于性能的测试，目前是通过对5K 的数据进行选择排序，进行o(n^2)复杂度的运算，一些脚本语言与编译语言的消耗时间对比：
```
# cpu：inter pentium 双核E5400 @ 2.70ghz。
 C                       16MS
LUA                      890MS
PERL                     4281MS
ALEX(AST)                32609MS  # 原来alex上基于语法树递归分析的速度
ALEX(VM)                 3781MS   # 现在基于vm设计的解释器，性能方面提升了88.40%，是原来的8.6倍！如果修改了GC 回收方式应该能再减少些. 
```
最初设计的alex是基于递归解析AST（抽象语法树），来完成语义解析过程。由于是分析AST来运行，所以带来了比较严重的性能和内存泄漏等问题。之后添加了VM，通过解析AST，编译生成对应于VM的虚拟指令集（图灵完备），提交给VM进行解析处理。性能上有了飞跃的提升！对5K数据进行排序操作，超越了perl。但alex整体架构上与perl却相差甚远，毕竟alex是个山寨成品，代码组织上没那么优雅，内部确实有多处设计不合理的地方，呵呵。

## 不足 ##
alex的不足的地方有很多，语法层次上仅仅是能够保证图灵完备，例如：alex语言中，对于循环只提供了while，没有提供for， foreach 等等， 判断语句只提供了if， else，没有提供 switch case， else if ，三元表达式等等。没有class，struct这样的结构，本来设计arraylist时想做的与lua table一样神奇，但最后由于实现上的问题，仅仅做成了一个泛型容器。这个功能会在以后添加，我可以从arraylist中看到他具有这个潜质，呵呵。
因为以上在语法层面上的总总不足，所以在编写脚本时会出现一些比较繁琐的操作。在脚本的简洁与表达性上，alex这点做的确实不好。
作为嵌入语言来说，由于alex vm设计的问题，alex脚本调用dll注册给它的C函数是没有任何问题，但dll中调用alex的function却很困难！用于进行操作与数据交换的stack设计上确实有一定的缺陷。这点在编写snake.alx时已经出现了这样的问题。
GC的回收，由于GC用的是比较古老的引用计数机制，而且也没添加弱引用，所以当遇到相互引用这样的问题时，GC是无法释放这两个对象。而且计数的加入导致了VM运行的缓慢，每次出现赋值运算都要判断两个对象是否可回收，如果可回收则修改引用标记个数。而且这个修改计数的触发是很频繁，以后会逐步把GC修改成标记回收，这样相互引用这个问题就能解决。

PS:总之不足的地方，还是蛮多的，毕竟这个东西是我在空闲时间弄出来的山寨成品，代码上有好几次自己都看不下去了，删了重写，写了重删，但目前代码上依然不是很好看（相对与lite-proto来说，自然lite-proto上也有些不合理）。模块之间的分隔并不清晰，一些地方用了全局的变量，为了追求效率，A模块产生的资源，却不全释放，需要让B模块来进行释放；VM内部对于定位变量操作，pop，push，解析虚拟指令的部分代码，全部都用了宏定义来进行实现，就为了省下call指令的性能开销。这些都让我感觉到这不是个好的设计，他们很容易会产生bug。甚至会更加致命，导致整个设计的重构！ 所以，如果你遇到bug，或者语法上的繁琐时，请你善待他。 :）

# 语法 #

## 1 alex注释符号 ##
符号"#" 为进行单行注释，如果要使用多行注释的人可能会失望，因为目前没有添加多行注释，所以当你写了如此多的
注释文本的话 你就不得不在文本的每行开头添加"#"来实现多行注释， 请原谅我的懒惰， 当我能看懂时，我是觉得过
多的注释是一种浪费···

## 2 关键字 ##
{{{var 声明并创建变量关键字：
EXP：` var  var_name;`
同时你可以像C语言那样在一行中定义并初始化多个变量;
EXP：` var v1,v2,v3=123;`
在alex中没有变量类型的区分，（虽然在最初时曾经做过对number和string的区分，但发现在脚本语言中不应当加入编译型
语言的定义方式，尽管你对那些编译型语言是如此的熟悉,但脚本的面向者应当过多的关注与程序的逻辑，而不是程序逻辑中
数据的存储，一旦向脚本的使用者暴露了变量具体的存储方式，这并不是好的开始。），在alex中没有对类型做的区分，所以
当你写下这样一段code时：
`var str = "test string!"; `就将str这个变量定义成了string类型。
类似：`var number = 1234;`则是将number变量定义成了数值类型。
`var al = [ ];`则是将al 变量定义成了数组(此类型会在类型中做解释。)

### function 函数声明关键字： ###
EXP:
```
function main([arglist])
{
} 
```
返回值的类型不需要在定义函数的名字前面进行声明， 您只需要写下function关键字来标识您写下的这个变量值是个函数，函数
的具体返回值是通过函数中的return返回值来获得（当然，如果你在函数中用到了这个关键字（return）的话）。

### return 函数返回值关键字: ###
EXP: `return [exp];`
在函数中返回值。

### while 循环关键字： ###
EXP：
```
			while(logic_exp)
			{
			}
```
while关键字与C语言上的while关键字一样，都是进行循环操作，通过判断逻辑表达式logic\_exP的值来确定是不是要进入下一次的
迭代。 在alex语言中仅仅是做了while 一个处理循环的关键字，因此你可能是无法看到使用犀利的for ，do{}while foreach，以及
更加犀利的goto语句。其原因在于，当我实现了while语法树时，觉得再进行实现另外的控制循环语法树并不会增加语言的适用性。
其他的迭代关键字都可以通过while来进行实现，因此请再一次原谅我的懒惰吧， 呵呵。

### break 关键字： ###
EXP：` break；`
跳出当前所在while循环。

### continue 关键字： ###
EXP ： ` continue；`
跳过当前所在的while循环中当前此次迭代。

### if 逻辑判断关键字： ###
EXP：
```
      if(logic_exp)
			{
			}
```
if关键字与多数语言上的if关键字一样，进行判断逻辑表达式logic\_exp是否为true来决定是否进入其代码段。像C语言一样在if的代码段后，你
可以通过加入else关键字来进行判断是否为false来决定是否进入其代码段。

### else 逻辑判断关键字： ###
EXP：
```
			else
			{
			}
```
当if中的逻辑表达式为false时才会执行到else。

## 3：数值运算符： ##
```
+,			#加法 
-,			#减法
*,			#乘法
/,			#除法
%,			#取余
++,			#自加（有前缀和后缀的区分）
--,			#自减（有前缀和后缀的区分）
=,			#赋值
(),			#括号
```

## 4: 逻辑运算符: ##
```
>,			#大于
==,			#等于
<,			#小于
<=,			#小于等于
>=,			#大于等于
!=,			#不等于
&&,			#与操作
||,			#或操作
```

## 5: 数据类型： ##
当你对一个变量赋值为1234 这样数值类型，标识着此变量是数值类型的变量。数值类型支持浮点型。同时您可以将一个变量定义成"test string"
来表示此变量是个字符串， 在alex中一旦出现 1+"string"这样的表达式，会将number类型1 提升为string类型，之后再再进行字符串的拼接
（类似java中的字符串拼接），因此当你出现以下定义方式 var tt = 123+"test string";  tt 的值是个string 其中存储的内容是123test string。
类型的提升仅仅是只能number提升为string。无法通过string直接降级为number（因为我们无法确定string的合法性，显然这个你可以自己通过实现
函数来进行完成此部分的工作）。


## 6：数组类型： ##
EXP： `var al = [];			#定义一个空的数组.`
当你写下以上表达式后，执行的结果是声明一个数组，但此数组中没有组员的，为空数组。自然您也可以在进行定义时为这个数组进行初始化。
EXP: `var al = [1,2,3,"string", [4,5]]; `你可能通过以上的表达式发现了al中的组员是可以不同类型，yes，正如你说想的那样，我对数组的实现做了增强！
其中的组员是泛型的，你可以向这个组员中添加一个string，一个number，一个数组，显然这个数组组员中的组员也可以是number，string，和 数组··· 所以当你抱怨没有二位数组的定义时，为什么不想一下 这样定义：`var al = [[1,1], [2,2], [3,3], [4,4]];` <sup>_</sup> 自然 三维，四维，五维··N维都可以通过此样的方式来实现，而且每一维中的数据都是泛型的，你可以定义成number 或者string。
当你定义了一个数组，仅仅是可以通过他的下标来进行索引。如：
```
var al = [1,2,3,4]; 
print(al[0]);   # 1
print(al[1]);   # 2
....
print(al);      # [1,2,3,4]
```
自然这个数组并不是可以定死的，是可以扩充的，你可以通过`add(al, 5)` 向数组追加新的组员（无法删除，也没法向lua table那样因为是hash table 可以像
结构体那样像既可以包含数组又可以包含成员变量，抱歉这段我的实现简单了··）。
`len(al);` 返回值为当前al数组的长度.

## 7:变量的作用域 ##
程序的作用域就只分为了两个部分 一个是局部，一个全局。 在全局仅仅允许你定义函数，变量以及调用函数，不支持进行逻辑表达式运算。
因此当你想确定一个程序的入口的话，你可以通过这样：
```
function main()
{

}
main();			#程序入口
```

## 8:print 函数 ##
注册的静态打印函数，会根据参数的类型来进行相应打印操作。
EXP：
```
var  a=1,b="string",c=2;
print(a);
```
同时你也可以使用字符串拼接:
```
print(a+b+c); 
```

## 10：hello world 程序 ##
```
function main()
{
	print("hello world!\n");
}

main();
```