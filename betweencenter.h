#include "ReadFiles.h"
#include <iostream>
#include <vector>

//读入文件到矩阵
void read_struct(TSMatrix& tsmatrix);

//遍历文件夹获取.htm,.html,.shtml结尾的文件作为key，自增的idx作为value
void urlToIndex(string path, map<string, int>& url_to_idx, map<int, string>& idx_to_url);

//计算介数
void computeBetweenCenter(vector<int>* Gm);