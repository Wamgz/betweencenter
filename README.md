# betweencenter

由于本题中图的稀疏性，迪杰斯特拉和floyd算法等不太适合这样大规模节点的运算，参考了https://www.tandfonline.com/doi/abs/10.1080/0022250X.2001.9990249实现了基于BFS
的介数计算

包含了ReadFiles和betweencenter两部分

其中ReadFiles读取所有url文件并将url文件建立图，写入graph.bin文件，考虑到文件稀疏性，采用了三元组方式存储

betweencenter对graph.bin文件进行读取，值得注意的是，graph.bin中存的是三元组形式的图，但是在计算介数时，我发现邻接表更方便计算，因此在读取文件后，我将图转变
了存储方式变成邻接表，具体原理参考其他人的博客和上面附的论文，最后取前20项输出， 方式是使用优先队列取前20项，再输入到栈中逆序。
