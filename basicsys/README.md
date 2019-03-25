# basicsys

## 使用指针与使用索引访问内存
指针访问

	for (i32 j = 0; j < size; ++j) {
		*pointer_ref[j] = j;
	}
	
索引访问
	
	for (i32 j = 0; j < size; ++j) {
		*(data + indexes_ref[j]) = j;
	}
	
指针访问的汇编代码（未优化）

    00007FF778C55437  cvtsi2sd    xmm0,dword ptr [rsp+24h]  
    00007FF778C5543D  movsxd      rax,dword ptr [rsp+24h]  
    00007FF778C55442  mov         rcx,qword ptr [pointer_ref]  
    00007FF778C55447  mov         rax,qword ptr [rcx+rax*8]  
    00007FF778C5544B  movsd       mmword ptr [rax],xmm0  
    
索引访问的汇编代码（未优化）

    00007FF6E1B954F7  cvtsi2sd    xmm0,dword ptr [rsp+30h]  
    00007FF6E1B954FD  movsxd      rax,dword ptr [rsp+30h]  
    00007FF6E1B95502  mov         rcx,qword ptr [indexes_ref]  
    00007FF6E1B95507  mov         rax,qword ptr [rcx+rax*8]  
    00007FF6E1B9550B  mov         rcx,qword ptr [data]  
    00007FF6E1B95510  movsd       mmword ptr [rcx+rax*8],xmm0  
    
索引访问比指针访问多了一条指令，其中加法多了一次，乘法多了一次。
时间上，执行10亿次：

指针
10747
10885
10605
10257
10261
10826
10922
10894
mean = 10674.625ms

索引
11065
10857
10746
10481
10270
10756
10687
10970
mean = 10729ms

多耗时54.375ms，每次多耗时0.000000054375ms

###结论
1. 32位程序优先使用指针
2. 64位程序，权衡内存消耗，如果内存消耗增加较大，则选择索引方式，反之，优先使用指针

## 内存mask
优先使用mask