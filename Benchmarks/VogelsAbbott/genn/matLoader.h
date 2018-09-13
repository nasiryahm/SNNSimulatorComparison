#pragma once
#include "va_benchmark_CODE/definitions.h"
#include <sstream>

void connectivity_from_mat(
    std::string filename,
    scalar * array,
    int presize,
    int postsize){

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

  // Placing loaded values into array
  for (int indx = 0; indx < prevec.size(); indx++){
    array[prevec[indx]*postsize + postvec[indx]] = weightvec[indx];
  }
}
