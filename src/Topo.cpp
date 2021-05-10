#include "Topo.h"

void
latencyMatrix(std::vector<std::vector<int>>& matrix, TOPO topo, int num){

    matrix.reserve(num);
    for(int i=0; i<num; i++){
        matrix.push_back(std::vector<int>(num, 0));
    }

    switch(topo){
        case TOPO::RING:
            ringLatency(matrix, num);
            break;
        case TOPO::D_RING:
            dringLatency(matrix, num);
            break;
        case TOPO::MESH:
            meshLatency(matrix, num);
            break;
        case TOPO::TORUS:
            torusLatency(matrix, num);
            break;
        case TOPO::TREES:
            treesLatency(matrix, num);
            break;
        case TOPO::CROSSBAR:
            crossbarLatency(matrix, num);
            break;
        default:
            fprintf(stderr, "invalid topology\n");
            exit(1);
    }
}

void ringLatency(std::vector<std::vector<int>>& matrix, int num){
    for(int i=0; i<num; i++){
        std::vector<int>& row = matrix[i];
        for(int j=0; j<num; j++){
            if(j >= i)
                row[j] = j - i;
            else
                row[j] = j + num - i;
        }
    }
}

void dringLatency(std::vector<std::vector<int>>& matrix, int num){
    for(int i=0; i<num; i++){
        std::vector<int>& row = matrix[i];
        for(int j=0; j<num; j++){
            int diff = abs(j-i);
            row[j] = std::min(diff, num-diff);
        }
    }
}

void meshLatency(std::vector<std::vector<int>>& matrix, int num){
    int length = 1;

    while(length*length < num)
        length++;

    for(int i=0; i<num; i++){
        std::vector<int>& row = matrix[i];
        int x1 = i / length;
        int y1 = i % length;
        for(int j=0; j<num; j++){
            int x2 = j / length;
            int y2 = j % length;
            int x_diff = abs(x1-x2);
            int y_diff = abs(y1-y2);
            row[j] = x_diff + y_diff;
        }
    }
}

void torusLatency(std::vector<std::vector<int>>& matrix, int num){
    int length = 1;

    while(length*length < num)
        length++;

    for(int i=0; i<num; i++){
        std::vector<int>& row = matrix[i];
        int x1 = i / length;
        int y1 = i % length;
        for(int j=0; j<num; j++){
            int x2 = j / length;
            int y2 = j % length;
            int x_diff = abs(x1-x2);
            x_diff = std::min(x_diff, length-x_diff);
            int y_diff = abs(y1-y2);
            y_diff = std::min(y_diff, length-y_diff);
            row[j] = y_diff + x_diff;
        }
    }
}

void crossbarLatency(std::vector<std::vector<int>>& matrix, int num){
    for(int i=0; i<num; i++){
        std::vector<int>& row = matrix[i];
        for(int j=0; j<num; j++){
            row[j] = 1;
        }
    }
}

void treesLatency(std::vector<std::vector<int>>& matrix, int num){
    if(num == 1){
        matrix[0][0] = 0;
        return;
    }
    int height = 0;
    int total = 1;
    while(total < num){
        total *= 2;
        height++;
    }
    
    for(int i=0; i<num; i++){
        std::vector<int>& row = matrix[i];
        for(int j=0; j<num; j++){
            int tmp = 0;
            int mul = 1;
            while(1){
                if(j/mul == i/mul){
                    int dis = tmp * 2;
                    if(tmp == height)
                        dis--;
                    row[j] = dis;
                    break;
                }
                tmp++;
                mul *= 2;
            }
        }
    }
}
