#include <iostream>
#include "ReadFiles.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <cstdio>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <exception>
#include <cstring>
#include <queue>
#include <stack>
#include <map>
#include <sys/types.h>

using namespace std;

#define SIZE 140432

int idx = 0;

Node::Node(int idx, float val){
    this -> idx = idx;
    this -> val = val;
}
Triple::Triple(int i, int j, float val){
    this -> i = i;
    this -> j = j;
    this -> val = val;
}
Triple::Triple(){
    this -> i = 0;
    this -> j = 0;
    this -> val = 0.0;
}
TSMatrix::TSMatrix(){
    this -> mu = 0;
    this -> nu = 0;
    this -> tu = 0;
}
TSMatrix::TSMatrix(int mu, int nu, int tu){
    this -> mu = mu;
    this -> nu = nu;
    this -> tu = tu;
}

//遍历文件夹，获取每个.html .htm .shtml的文件和对应idx
void urlToIndex(char *basePath, map<string, int>& url_to_idx, map<int, string>& idx_to_url){
    //path = E:/coding environment/pageranking-dataset/news.sohu.com
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (dir == NULL)
        return;

    struct stat s_buf;
    regex pattern("news\.sohu\.com.*\..?htm.?$");

    smatch sm;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            stat(path, &s_buf);

            string s(path);
            bool match = regex_search(s, sm, pattern);
            if(match){
                for(int i = 0;i < sm.size();i++){
                    url_to_idx.insert(pair <string, int> (sm[i], idx));
                    idx_to_url.insert(pair <int, string> (idx, sm[i]));
                    idx++;
                }
            }
            urlToIndex(path, url_to_idx, idx_to_url);

        }
    }

    closedir(dir);
}

void read_struct(vector<int>* Gm){

    ifstream reader("./graph2.bin", ios::in|ios::binary);

	int t;
	reader.read((char*)&t, sizeof(int));
    cout<<"start read"<<endl;
	for (int i = 0;i < t;i++){
		Triple temp;
		reader.read((char*)&temp, sizeof(Triple));
        int row = temp.i;
        int col = temp.j;
        Gm[row].push_back(col);
	}

    cout<<"end read, total size of edges:  "<<t<<endl;
}

void display(float* arr){
    for(int i = 0;i < SIZE;i++){
        cout<<arr[i]<<" ";
    }

    cout<<endl;
}

//优先队列cmp
struct cmp{
    bool operator() (Node a, Node b){
        return a.val > b.val;   
    }
};

//14.18
int main(){
    time_t start, finish;
	start = clock();
    char* basePath = "./news.sohu.com";  //真实路径
    // char* basePath = "C:/Users/Woods_WANG/Desktop/news.sohu.com/testdir";   //测试数据路径

    //获取每个url对应的idx
    map<string, int> url_to_idx;
    map<int, string> idx_to_url;

    urlToIndex(basePath, url_to_idx, idx_to_url);
    cout<<url_to_idx.size()<<"  "<<idx_to_url.size()<<endl;

    //开始建图
    cout<<"**********************start**********************"<<endl;

    vector<int>* Gm = new vector<int>[SIZE];
    //建图
    read_struct(Gm);
    float *Cb = new float[SIZE];
    for(int i = 0;i < SIZE;i++){
        Cb[i] = 0.0;
    }

    for(int s = 0;s < SIZE;s++){
        stack<int> S;
        vector<int>* P = new vector<int>[SIZE];
        float* sigma = new float[SIZE];
        float* dist = new float[SIZE];
        float* delta = new float[SIZE];

        for(int i = 0;i < SIZE;i++){
            sigma[i] = 0.0;
            dist[i] = -1;
            delta[i] = 0.0;
        }

        sigma[s] = 1.0;
        dist[s] = 0.0;

        queue<int> Q;
        Q.push(s);
        while(!Q.empty()){
            int v = Q.front();
            Q.pop();
            S.push(v);
            for(int j = 0;j < Gm[v].size();j++){
                int w = Gm[v][j];
                if(dist[w] < 0){
                    Q.push(w);
                    dist[w] = dist[v] + 1.0;
                }

                if(dist[w] == dist[v] + 1){
                    sigma[w] += sigma[v];

                    P[w].push_back(v);
                }
            }
        }

        while(!S.empty()){
            int w = S.top();
            S.pop();

            for(int i = 0;i < P[w].size();i++){
                int v = P[w][i];
                delta[v] += (1.0 * sigma[v]) / sigma[w] * (1 + delta[w]);
            }
            if(w != s){
                Cb[w] = Cb[w] + delta[w];
            }
        }


        delete[] P;
        delete[] sigma;
        delete[] dist;
        delete[] delta;
    }
    ofstream OutFile("resultBC.txt");
    

    int k = 20;
    priority_queue<Node,vector<Node>, cmp> pq;
    for(int i = 0;i < SIZE;i++){
        if(pq.size() < k){
            pq.push(Node(i, Cb[i]));
        }
        else{
            if(Cb[i] > pq.top().val){
                pq.pop();
                pq.push(Node(i, Cb[i]));
            }
        }
    }

    stack<Node> st;
    for(int i = 0;i < k;i++){
        int idxOfCb = pq.top().idx;
        float val = pq.top().val;
        st.push(Node(idxOfCb, val));
        pq.pop();
    }
    int i = 0;
    while(!st.empty()){
        Node cur = st.top();
        st.pop();
        OutFile<<"i: "<<i++<<"  "<<cur.val<<"  "<<idx_to_url[cur.idx]<<endl;
    }

    finish = clock();
    
	printf("pagerank runtime: %d\n",(finish - start) );
}