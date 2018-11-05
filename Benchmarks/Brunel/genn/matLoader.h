#pragma once
#include "brunel_benchmark_CODE/definitions.h"
#include <sstream>
#include "utils.h"
#include "sparseUtils.h"
#include <stdlib.h>

#include "sparseProjection.h"

void reset_array(
    float* array,
    unsigned int num_elements)
{
  for (int n=0; n < num_elements; n++){
    array[n] = 0.0f;
  }
};

void random_connectivity(
    unsigned int* ind,
    unsigned int* rowLength,
    unsigned int numPre,
    unsigned int numPost,
    unsigned int numSyns,
    int seed)
{

  srand(seed);
  for (int preid = 0; preid < numPre; preid++){
    rowLength[preid] = numSyns;
    for (int synindx = 0; synindx < numSyns; synindx++){
      ind[preid*numSyns + synindx] = rand() % numPost;
    }
  }
};

void ragged_connectivity_from_mat(
    std::string filename,
    unsigned int* ind,
    unsigned int* rowLength,
    unsigned int numPre,
    unsigned int maxRows)
{

  ifstream weightfile;
  string line;
  stringstream ss;
  std::vector<int> prevec, postvec;
  std::vector<float> weightvec;
  int pre, post;
  float weight;
  int linecount = 0;
  weightfile.open(filename.c_str());

  if (weightfile.is_open()){
    printf("Loading weights from mat file: %s\n", filename.c_str());
    while (getline(weightfile, line)){
      if (line.c_str()[0] == '%'){
        continue;
      } else {
        linecount++;
        if (linecount == 1) continue;
        //printf("%s\n", line.c_str());
        ss.clear();
        ss << line;
        ss >> pre >> post >> weight;
        prevec.push_back(pre - 1);
        postvec.push_back(post - 1);
        weightvec.push_back(weight);
        //printf("%d, %d, %f\n", pre, post, weight);
      }
    }
  }
  
  std::vector<int> precount;
  for (int pre = 0; pre < numPre; pre++)
    precount.push_back(0);

  // Placing loaded values into array
  for (int indx = 0; indx < prevec.size(); indx++){
    ind[prevec[indx]*maxRows + precount[prevec[indx]]] = postvec[indx];
    precount[prevec[indx]]++;
  }
  for (int pre = 0; pre < numPre; pre++){
    rowLength[pre] = precount[pre];
  }

};
